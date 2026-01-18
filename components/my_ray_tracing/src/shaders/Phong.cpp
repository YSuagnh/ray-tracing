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

        // Fresnel reflectance at normal incidence (F0)
        {
            auto f0Float = material.getProperty<Property::Wrapper::FloatType>("F0");
            if (f0Float) {
                const float v = glm::clamp((*f0Float).value, 0.0f, 0.999f);
                f0 = Vec3(v);
            }
            else {
                // reasonable dielectric default
                f0 = Vec3(0.04f);
            }
        }
        
        // 获取材质的高光指数
        auto shininessValue = material.getProperty<Property::Wrapper::FloatType>("shininess");
        if (shininessValue) shininess = (*shininessValue).value;
        else shininess = 10.0f;  // 默认中等光泽度
        
        // 获取环境光颜色
        auto ambientColor = material.getProperty<Property::Wrapper::RGBType>("ambientColor");
        if (ambientColor) ambient = (*ambientColor).value;
        else ambient = {0.1f, 0.1f, 0.1f};  // 默认低强度环境光
    }

    static inline Vec3 fresnelSchlick(float cosTheta, const Vec3& F0) {
        const float ct = glm::clamp(cosTheta, 0.0f, 1.0f);
        return F0 + (Vec3(1.0f) - F0) * pow(1.0f - ct, 5.0f);
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

        // Fresnel decides likelihood of reflection (specular) vs diffuse
        const float cosNI = glm::max(0.0f, glm::dot(normal, wi));
        const Vec3  Fv = fresnelSchlick(cosNI, f0);
        const float F = glm::clamp((Fv.x + Fv.y + Fv.z) / 3.0f, 0.0f, 1.0f);
        const float pDiffuse = 1.0f - F;

        const float xi = defaultSamplerInstance<UniformSampler>().sample1d();
        if (xi < pDiffuse) {
            // Cosine-weighted hemisphere sampling around the surface normal
            Vec3 localDir = defaultSamplerInstance<HemiSphere>().sample3d();
            Onb onb{ normal };
            wo = glm::normalize(onb.local(localDir));

            const float cosTheta = glm::max(0.0f, glm::dot(normal, wo));

            // Lambert BRDF: f = kd / PI
            f = diffuse / PI;

            // pdf for cosine-weighted hemisphere
            const float pdfDiffuse = cosTheta / PI;
            pdf = glm::max(pDiffuse * pdfDiffuse, 1e-8f);
        }
        else {
            // Phong specular lobe importance sampling around reflection direction r
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
            // Use Fresnel term to modulate specular energy.
            f = (specular * Fv) * ((n + 2.0f) / (2.0f * PI)) * pow(alphaCos, n);

            // pdf from the sampler (localDir is already in the +Z hemisphere)
            const float pdfSpec = lobeSampler.pdf(localDir);
            pdf = glm::max((1.0f - pDiffuse) * pdfSpec, 1e-8f);
        }

        return {
            Ray{ origin, wo },
            f,
            Vec3(0.0f),
            pdf
        };
    }
}