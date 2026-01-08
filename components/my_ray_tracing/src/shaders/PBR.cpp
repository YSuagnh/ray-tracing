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
        auto metallicValue = material.getProperty<Property::Wrapper::FloatType>("metallic");
        if (metallicValue) metallic = glm::clamp((*metallicValue).value, 0.0f, 1.0f);
        else metallic = 0.0f;  // 默认为介电质
        
        // 获取粗糙度
        auto roughnessValue = material.getProperty<Property::Wrapper::FloatType>("roughness");
        if (roughnessValue) roughness = glm::clamp((*roughnessValue).value, 0.01f, 1.0f);
        else roughness = 0.5f;  // 默认中等粗糙度
        
        // 获取折射率
        auto iorValue = material.getProperty<Property::Wrapper::FloatType>("ior");
        if (iorValue) ior = (*iorValue).value;
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
        float cosTheta = sqrt((1.0f - Xi2) / (1.0f + (a*a - 1.0f) * Xi2));
        float sinTheta = sqrt(1.0f - cosTheta * cosTheta);
        
        // 转换为笛卡尔坐标（切线空间）
        Vec3 H;
        H.x = cos(phi) * sinTheta;
        H.y = sin(phi) * sinTheta;
        H.z = cosTheta;
        
        // 转换到世界坐标
        Onb onb{N};
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
        Vec3 origin = hitPoint;
        Vec3 V = glm::normalize(-ray.direction);  // 视线方向
        Vec3 N = normal;
        
        // 计算F0（垂直入射时的Fresnel反射率）
        Vec3 F0 = Vec3(0.04f);  // 介电质的平均F0
        F0 = glm::mix(F0, baseColor, metallic);  // 金属材料使用基础颜色作为F0
        
        Vec3 outgoingDirection;
        Vec3 attenuation;
        float pdf;
        
        // 重要性采样：根据BRDF的主要贡献选择采样策略
        float specularWeight = (F0.x + F0.y + F0.z) / 3.0f;
        float diffuseWeight = (1.0f - metallic) * (baseColor.x + baseColor.y + baseColor.z) / 3.0f;
        float totalWeight = specularWeight + diffuseWeight;
        
        if (totalWeight > 0) {
            float specularProbability = specularWeight / totalWeight;
            float random = defaultSamplerInstance<UniformSampler>().sample1d();
            
            if (random < specularProbability) {
                // 镜面反射采样
                Vec3 H = ImportanceSampleGGX(N, roughness);
                Vec3 L = glm::normalize(glm::reflect(-V, H));
                
                // 确保在正确的半球
                if (glm::dot(L, N) <= 0) {
                    L = glm::reflect(L, N);
                }
                
                outgoingDirection = L;
                
                // 计算Cook-Torrance BRDF
                float NdotL = glm::max(glm::dot(N, L), 0.0f);
                float NdotV = glm::max(glm::dot(N, V), 0.0f);
                float HdotV = glm::max(glm::dot(H, V), 0.0f);
                
                if (NdotL > 0 && NdotV > 0) {
                    // 计算BRDF的各个分量
                    float D = DistributionGGX(N, H, roughness);
                    float G = GeometrySmith(N, V, L, roughness);
                    Vec3 F = FresnelSchlick(HdotV, F0);
                    
                    // 镜面反射BRDF
                    Vec3 numerator = D * G * F;
                    float denominator = 4.0f * NdotV * NdotL;
                    Vec3 specular = numerator / glm::max(denominator, 0.001f);
                    
                    // 漫反射BRDF（只有非金属材料才有）
                    Vec3 kS = F;  // Fresnel项就是镜面反射的比例
                    Vec3 kD = Vec3(1.0f) - kS;
                    kD *= 1.0f - metallic;  // 金属材料没有漫反射
                    Vec3 diffuse = kD * baseColor / PI;
                    
                    attenuation = (diffuse + specular) * NdotL;
                    
                    // PDF计算
                    float NdotH = glm::max(glm::dot(N, H), 0.0f);
                    pdf = (D * NdotH / (4.0f * HdotV)) * specularProbability;
                } else {
                    attenuation = Vec3(0.0f);
                    pdf = 0.001f;
                }
            } else {
                // 漫反射采样（余弦加权）
                Vec3 randomDirection = defaultSamplerInstance<HemiSphere>().sample3d();
                Onb onb{N};
                Vec3 L = glm::normalize(onb.local(randomDirection));
                outgoingDirection = L;
                
                float NdotL = glm::max(glm::dot(N, L), 0.0f);
                float NdotV = glm::max(glm::dot(N, V), 0.0f);
                
                if (NdotL > 0 && NdotV > 0) {
                    // 简化的漫反射BRDF
                    Vec3 kD = Vec3(1.0f - metallic);
                    Vec3 diffuse = kD * baseColor / PI;
                    
                    attenuation = diffuse * NdotL;
                    pdf = NdotL / PI * (1.0f - specularProbability);
                } else {
                    attenuation = Vec3(0.0f);
                    pdf = 0.001f;
                }
            }
        } else {
            // 备用：简单的漫反射
            Vec3 randomDirection = defaultSamplerInstance<HemiSphere>().sample3d();
            Onb onb{N};
            outgoingDirection = glm::normalize(onb.local(randomDirection));
            attenuation = baseColor / PI;
            pdf = 1.0f / (2.0f * PI);
        }
        
        return {
            Ray{origin, outgoingDirection},     // 散射光线
            attenuation,                        // 衰减系数（BRDF值）
            emissive,                          // 自发光
            glm::max(pdf, 0.001f)              // 概率密度函数值
        };
    }
}