#include "shaders/Phong.hpp"
#include "samplers/SamplerInstance.hpp"

#include "Onb.hpp"
#include <algorithm>

namespace RayTracer
{
    /**
     * Phong着色器构造函数
     * @param material 材质对象
     * @param textures 纹理缓冲区
     */
    Phong::Phong(Material& material, vector<Texture>& textures)
        : Shader(material, textures)
    {
        // 获取材质的漫反射颜色
        auto diffuseColor = material.getProperty<Property::Wrapper::RGBType>("diffuseColor");
        if (diffuseColor) diffuse = (*diffuseColor).value;
        else diffuse = {1.0f, 1.0f, 1.0f};  // 默认灰色
        
        // 获取材质的镜面反射颜色
        auto specularColor = material.getProperty<Property::Wrapper::RGBType>("specularColor");
        if (specularColor) specular = (*specularColor).value;
        else specular = {0.05f, 0.05f, 0.05f};  // 默认低强度镜面反射
        
        // 获取材质的高光指数
        auto shininessValue = material.getProperty<Property::Wrapper::FloatType>("shininess");
        if (shininessValue) shininess = (*shininessValue).value;
        else shininess = 10.0f;  // 默认中等光泽度
        
        // 获取环境光颜色
        auto ambientColor = material.getProperty<Property::Wrapper::RGBType>("ambientColor");
        if (ambientColor) ambient = (*ambientColor).value;
        else ambient = {0.1f, 0.1f, 0.1f};  // 默认低强度环境光
    }
    
    /**
     * Phong光照散射计算
     * 结合概率性采样实现Phong BRDF
     * @param ray 入射光线
     * @param hitPoint 相交点
     * @param normal 法向量
     * @return 散射信息
     */
    Scattered Phong::shade(const Ray& ray, const Vec3& hitPoint, const Vec3& normal) const {
        Vec3 origin = hitPoint;
        Vec3 wi = glm::normalize(-ray.direction); // incoming (towards surface)

        // Ideal reflection direction (around which the Phong lobe is defined)
        Vec3 r = glm::normalize(glm::reflect(-wi, normal));

        Vec3 wo{};
        Vec3 f{};
        float pdf = 0.0f;

        // Choose between diffuse/specular by energy (very simple mixture)
        float diffuseWeight = (diffuse.x + diffuse.y + diffuse.z) / 3.0f;
        float specularWeight = (specular.x + specular.y + specular.z) / 3.0f;
        float totalWeight = diffuseWeight + specularWeight;

        float pDiffuse = 1.0f;
        if (totalWeight > 0.0f) pDiffuse = diffuseWeight / totalWeight;

        const float xi = defaultSamplerInstance<UniformSampler>().sample1d();
        if (xi < pDiffuse) {
            // Cosine-weighted hemisphere sampling around the surface normal
            // (current HemiSphere sampler returns z = cos(theta) in [0,1])
            Vec3 localDir = defaultSamplerInstance<HemiSphere>().sample3d();
            Onb onb{ normal };
            wo = glm::normalize(onb.local(localDir));

            const float cosTheta = glm::max(0.0f, glm::dot(normal, wo));

            // Lambert BRDF: f = kd / PI
            f = diffuse / PI;

            // pdf for cosine-weighted hemisphere
            const float pdfDiffuse = cosTheta / PI;
            pdf = pDiffuse * pdfDiffuse;
        }
        else {
            // Phong specular lobe importance sampling around reflection direction r
            // Use CosPowerHemisphere sampler: p(ω) = (n+1)/(2π) * cos^n(θ), where θ is from +Z.
            const float n = glm::max(0.0f, shininess);

            auto& lobeSampler = defaultSamplerInstance<CosPowerHemisphere>();
            lobeSampler.setExponent(n);

            // sample in local frame where +Z == r
            const Vec3 localDir = lobeSampler.sample3d();
            Onb onb{ r };
            wo = glm::normalize(onb.local(localDir));

            // Ensure the sampled direction is above the surface
            if (glm::dot(wo, normal) <= 0.0f) {
                wo = r;
            }

            // alphaCos = cos(alpha) between wo and r
            const float alphaCos = glm::max(0.0f, glm::dot(glm::normalize(r), glm::normalize(wo)));

            // Phong specular BRDF (normalized): f = ks * (n+2)/(2PI) * cos(alpha)^n
            f = specular * ((n + 2.0f) / (2.0f * PI)) * pow(alphaCos, n);

            // pdf from the sampler (localDir is already in the +Z hemisphere)
            const float pdfSpec = lobeSampler.pdf(localDir);
            pdf = (1.0f - pDiffuse) * pdfSpec;
        }

        // Avoid division by zero / NaNs
        pdf = glm::max(pdf, 1e-8f);

        return {
            Ray{ origin, wo },
            f,              // attenuation is treated as BRDF in integrator
            Vec3(0.0f),
            pdf
        };
    }
}