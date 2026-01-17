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
        else ambient = {0.0f, 0.0f, 0.0f};  // 默认低强度环境光
    }
    

    static inline float saturate(float x) {
        return glm::clamp(x, 0.0f, 1.0f);
    }

    static inline float fresnelSchlick(float cosTheta, float F0) {
        cosTheta = saturate(cosTheta);
        const float m = 1.0f - cosTheta;
        const float m2 = m * m;
        const float m5 = m2 * m2 * m;
        return F0 + (1.0f - F0) * m5;
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

        Vec3 n = glm::normalize(normal);
        float cosThetaI = saturate(glm::dot(wi, n));

        // Ideal reflection direction (around which the Phong lobe is defined)
        Vec3 r = glm::normalize(glm::reflect(-wi, n));

        Vec3 wo{};
        Vec3 f{};
        float pdf = 0.0f;

        // Fresnel-based mixing:
        // Use average specular as F0 (in [0,1]) and Schlick to get Fr.
        const float F0 = saturate((specular.x + specular.y + specular.z) / 3.0f);
        const float Fr = fresnelSchlick(cosThetaI, F0);
        const float pSpecular = glm::clamp(Fr, 0.0f, 1.0f);
        const float pDiffuse = 1.0f - pSpecular;

        const float xi = defaultSamplerInstance<UniformSampler>().sample1d();
        if (xi < pDiffuse) {
            // Diffuse: cosine-weighted hemisphere sampling around the surface normal
            Vec3 localDir = defaultSamplerInstance<HemiSphere>().sample3d();
            Onb onb{ n };
            wo = glm::normalize(onb.local(localDir));

            const float cosTheta = glm::max(0.0f, glm::dot(n, wo));

            // Lambert BRDF
            f = diffuse / PI;

            // mixture pdf
            const float pdfDiffuse = cosTheta / PI;
            pdf = pDiffuse * pdfDiffuse;
        }
        else {
            // Specular: Phong lobe importance sampling around reflection direction r
            const float expN = glm::max(0.0f, shininess);

            auto& lobeSampler = defaultSamplerInstance<CosPowerHemisphere>();
            lobeSampler.setExponent(expN);

            const Vec3 localDir = lobeSampler.sample3d();
            Onb onb{ r };
            wo = glm::normalize(onb.local(localDir));

            // Ensure above surface
            if (glm::dot(wo, n) <= 0.0f) wo = r;

            const float alphaCos = glm::max(0.0f, glm::dot(glm::normalize(r), glm::normalize(wo)));

            // Normalized Phong specular BRDF
            f = specular * ((expN + 2.0f) / (2.0f * PI)) * pow(alphaCos, expN);

            const float pdfSpec = lobeSampler.pdf(localDir);
            pdf = pSpecular * pdfSpec;
        }

        pdf = glm::max(pdf, 1e-8f);

        return {
            Ray{ origin, wo },
            f,
            Vec3(0.0f),
            pdf
        };
    }
}