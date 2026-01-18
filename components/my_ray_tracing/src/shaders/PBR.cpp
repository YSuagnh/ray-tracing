#include "shaders/PBR.hpp"
#include "samplers/SamplerInstance.hpp"

#include "Onb.hpp"
#include <algorithm>

namespace RayTracer
{
    /**
     * PBR着色器构造函数
     * @param material 材质对象
     * @param textures 纹理缓冲区
     */
    PBR::PBR(Material& material, vector<Texture>& textures)
        : Shader(material, textures)
    {
        // 获取基础颜色
        auto baseColorValue = material.getProperty<Property::Wrapper::RGBType>("baseColor");
        if (baseColorValue) baseColor = (*baseColorValue).value;
        else baseColor = {0.8f, 0.8f, 0.8f};  // 默认浅灰色
        
        // 获取金属度
        // Support both `metallic` and misspelled `matallic` (legacy scene files)
        {
            auto metallicValue = material.getProperty<Property::Wrapper::FloatType>("metallic");
            if (!metallicValue) metallicValue = material.getProperty<Property::Wrapper::FloatType>("matallic");

            if (metallicValue) metallic = glm::clamp((*metallicValue).value, 0.0f, 1.0f);
            else metallic = 0.0f;  // 默认为介电质
        }

        // 获取粗糙度
        auto roughnessValue = material.getProperty<Property::Wrapper::FloatType>("roughness");
        if (roughnessValue) roughness = glm::clamp((*roughnessValue).value, 0.01f, 1.0f);
        else roughness = 0.5f;  // 默认中等粗糙度
        
        // 获取折射率 (avoid invalid ior like 0)
        auto iorValue = material.getProperty<Property::Wrapper::FloatType>("ior");
        if (iorValue) ior = glm::max((*iorValue).value, 1.0f);
        else ior = 1.5f;  // 默认玻璃折射率
        
        // 获取自发光颜色
        auto emissiveColor = material.getProperty<Property::Wrapper::RGBType>("emissive");
        if (emissiveColor) emissive = (*emissiveColor).value;
        else emissive = {0.0f, 0.0f, 0.0f};  // 默认无自发光
    }
    
    /**
     * GGX/Trowbridge-Reitz 法线分布函数
     */
    float PBR::DistributionGGX(const Vec3& N, const Vec3& H, float roughness) const {
        float a = roughness * roughness;
        float a2 = a * a;
        float NdotH = glm::max(glm::dot(N, H), 0.0f);
        float NdotH2 = NdotH * NdotH;
        
        float nom = a2;
        float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
        denom = PI * denom * denom;
        
        return nom / denom;
    }
    
    /**
     * Schlick-GGX 几何函数的单向版本
     */
    float PBR::GeometrySchlickGGX(float NdotV, float roughness) const {
        float r = (roughness + 1.0f);
        float k = (r * r) / 8.0f;
        
        float nom = NdotV;
        float denom = NdotV * (1.0f - k) + k;
        
        return nom / denom;
    }
    
    /**
     * Smith G几何函数
     */
    float PBR::GeometrySmith(const Vec3& N, const Vec3& V, const Vec3& L, float roughness) const {
        float NdotV = glm::max(glm::dot(N, V), 0.0f);
        float NdotL = glm::max(glm::dot(N, L), 0.0f);
        float ggx2 = GeometrySchlickGGX(NdotV, roughness);
        float ggx1 = GeometrySchlickGGX(NdotL, roughness);
        
        return ggx1 * ggx2;
    }
    
    /**
     * Fresnel-Schlick 近似
     */
    Vec3 PBR::FresnelSchlick(float cosTheta, const Vec3& F0) const {
        float oneMinusCosTheta = 1.0f - cosTheta;
        float oneMinusCosTheta5 = oneMinusCosTheta * oneMinusCosTheta * oneMinusCosTheta * oneMinusCosTheta * oneMinusCosTheta;
        return F0 + (Vec3(1.0f) - F0) * oneMinusCosTheta5;
    }
    
    /**
     * 基于GGX分布的重要性采样
     */
    Vec3 PBR::ImportanceSampleGGX(const Vec3& N, float roughness) const {
        float a = roughness * roughness;

        // 生成随机数
        float Xi1 = defaultSamplerInstance<UniformSampler>().sample1d();
        float Xi2 = defaultSamplerInstance<UniformSampler>().sample1d();

        // 在球面坐标中采样
        float phi = 2.0f * PI * Xi1;
        float cosTheta = sqrt((1.0f - Xi2) / (1.0f + (a * a - 1.0f) * Xi2));
        float sinTheta = sqrt(glm::max(0.0f, 1.0f - cosTheta * cosTheta));

        // 转换为笛卡尔坐标（切线空间）
        Vec3 H;
        H.x = cos(phi) * sinTheta;
        H.y = sin(phi) * sinTheta;
        H.z = cosTheta;

        // 转换到世界坐标
        Onb onb{ N };
        return glm::normalize(onb.local(H));
    }

    /**
     * PBR散射计算
     * 实现Cook-Torrance BRDF模型
     * @param ray 入射光线
     * @param hitPoint 相交点
     * @param normal 法向量
     * @return 散射信息
     */
    Scattered PBR::shade(const Ray& ray, const Vec3& hitPoint, const Vec3& normal) const {
        const Vec3 origin = hitPoint;
        const Vec3 V = glm::normalize(-ray.direction);  // view dir (from hit -> camera)
        const Vec3 N = glm::normalize(normal);

        // F0 at normal incidence
        Vec3 F0 = Vec3(0.04f);
        F0 = glm::mix(F0, baseColor, metallic);

        Vec3 outgoingDirection(0.0f);
        Vec3 f(0.0f);
        float pdf = 0.0f;

        // Mixture probabilities (simple energy heuristic)
        const float specularWeight = (F0.x + F0.y + F0.z) / 3.0f;
        const float diffuseWeight = (1.0f - metallic) * (baseColor.x + baseColor.y + baseColor.z) / 3.0f;
        const float totalWeight = specularWeight + diffuseWeight;

        const float pSpec = (totalWeight > 0.0f) ? glm::clamp(specularWeight / totalWeight, 0.0f, 1.0f) : 1.0f;
        const float pDiff = 1.0f - pSpec;

        const float xiMix = defaultSamplerInstance<UniformSampler>().sample1d();

        if (xiMix < pSpec) {
            // -------- Specular sampling (GGX half-vector) --------
            Vec3 H(0.0f);
            Vec3 L(0.0f);

            // Do NOT "fix" invalid directions by reflecting again (breaks BRDF/pdf consistency).
            // Instead, resample a few times.
            bool valid = false;
            for (int attempt = 0; attempt < 8; ++attempt) {
                H = ImportanceSampleGGX(N, roughness);
                L = glm::normalize(glm::reflect(-V, H));
                if (glm::dot(L, N) > 0.0f && glm::dot(H, N) > 0.0f) {
                    valid = true;
                    break;
                }
            }
            if (!valid) {
                // fallback: perfect reflection around normal
                H = glm::normalize(N);
                L = glm::normalize(glm::reflect(-V, N));
            }

            outgoingDirection = L;

            const float NdotL = glm::max(glm::dot(N, L), 0.0f);
            const float NdotV = glm::max(glm::dot(N, V), 0.0f);
            const float NdotH = glm::max(glm::dot(N, H), 0.0f);
            const float HdotV = glm::max(glm::dot(H, V), 0.0f);

            if (NdotL > 0.0f && NdotV > 0.0f && HdotV > 0.0f) {
                const float D = DistributionGGX(N, H, roughness);
                const float G = GeometrySmith(N, V, L, roughness);
                const Vec3  F = FresnelSchlick(HdotV, F0);

                // Cook-Torrance specular BRDF (f_s)
                const float denom = glm::max(4.0f * NdotV * NdotL, 1e-6f);
                const Vec3  specular = (D * G * F) / denom;

                // Diffuse part (for non-metals)
                Vec3 kS = F;
                Vec3 kD = (Vec3(1.0f) - kS) * (1.0f - metallic);
                const Vec3 diffuse = kD * baseColor / PI;

                f = diffuse + specular;

                // PDF for sampling L via half-vector sampling
                // p(L) = p(H) / (4 * dot(V,H)) ; p(H) = D * NdotH (for this sampler)
                const float pdfSpec = (D * NdotH) / glm::max(4.0f * HdotV, 1e-6f);
                pdf = pSpec * pdfSpec;
            }
        }
        else {
            // -------- Diffuse sampling (cosine-weighted hemisphere) --------
            Vec3 localDir = defaultSamplerInstance<HemiSphere>().sample3d();
            Onb onb{ N };
            outgoingDirection = glm::normalize(onb.local(localDir));

            const float NdotL = glm::max(glm::dot(N, outgoingDirection), 0.0f);

            // Lambert BRDF
            const Vec3 kd = (Vec3(1.0f) - Vec3(metallic)) * baseColor;
            f = kd / PI;

            // Cosine-weighted pdf
            const float pdfDiff = NdotL / PI;
            pdf = pDiff * pdfDiff;
        }

        pdf = glm::max(pdf, 1e-6f);

        return {
            Ray{ origin, outgoingDirection },
            f,
            emissive,
            pdf
        };
    }
}