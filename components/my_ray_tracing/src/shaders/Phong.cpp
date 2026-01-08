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
        Vec3 incomingDirection = glm::normalize(-ray.direction);
        Vec3 outgoingDirection;
        Vec3 attenuation;
        float pdf;
        
        // 计算反射方向
        Vec3 reflectDirection = glm::reflect(-incomingDirection, normal);
        
        // 根据材质特性决定散射类型（简化的重要性采样）
        float diffuseWeight = (diffuse.x + diffuse.y + diffuse.z) / 3.0f;
        float specularWeight = (specular.x + specular.y + specular.z) / 3.0f;
        float totalWeight = diffuseWeight + specularWeight;
        
        if (totalWeight > 0) {
            float diffuseProbability = diffuseWeight / totalWeight;
            
            // 随机选择漫反射或镜面反射
            float random = defaultSamplerInstance<UniformSampler>().sample1d();
            
            if (random < diffuseProbability) {
                // 漫反射散射
                Vec3 randomDirection = defaultSamplerInstance<HemiSphere>().sample3d();
                Onb onb{normal};
                outgoingDirection = glm::normalize(onb.local(randomDirection));
                
                // Phong模型漫反射分量
                float cosTheta = glm::max(0.0f, glm::dot(outgoingDirection, normal));
                attenuation = diffuse * cosTheta / PI;
                pdf = cosTheta / PI * diffuseProbability;
            } else {
                // 镜面反射散射（围绕理想反射方向）
                Vec3 perturbedReflect = reflectDirection;
                
                // 添加轻微扰动以模拟不完全镜面反射
                if (shininess < 1000.0f) {
                    Vec3 randomOffset = defaultSamplerInstance<HemiSphere>().sample3d() * (1.0f / shininess);
                    perturbedReflect = glm::normalize(reflectDirection + randomOffset * 0.1f);
                    
                    // 确保方向在正确的半球内
                    if (glm::dot(perturbedReflect, normal) < 0) {
                        perturbedReflect = reflectDirection;
                    }
                }
                
                outgoingDirection = perturbedReflect;
                
                // Phong模型镜面反射分量
                float cosAlpha = glm::max(0.0f, glm::dot(outgoingDirection, reflectDirection));
                float specularTerm = pow(cosAlpha, shininess);
                attenuation = specular * specularTerm * (shininess + 2) / (2 * PI);
                pdf = specularTerm * (shininess + 1) / (2 * PI) * (1.0f - diffuseProbability);
            }
        } else {
            // 备用：纯漫反射
            Vec3 randomDirection = defaultSamplerInstance<HemiSphere>().sample3d();
            Onb onb{normal};
            outgoingDirection = glm::normalize(onb.local(randomDirection));
            attenuation = Vec3{0.5f, 0.5f, 0.5f};
            pdf = 1.0f / (2 * PI);
        }
        
        return {
            Ray{origin, outgoingDirection},     // 散射光线
            attenuation,                        // 衰减系数
            Vec3(0.0f),                           // 发射光（环境光）
            pdf                                // 概率密度函数值
        };
    }
}