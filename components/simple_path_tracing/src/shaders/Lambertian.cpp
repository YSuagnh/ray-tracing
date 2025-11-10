#include "shaders/Lambertian.hpp"
#include "samplers/SamplerInstance.hpp"

#include "Onb.hpp"

namespace SimplePathTracer
{
    /**
     * Lambertian着色器构造函数
     * @param material 材质对象
     * @param textures 纹理缓冲区
     */
    Lambertian::Lambertian(Material& material, vector<Texture>& textures)
        : Shader                (material, textures)
    {
        // 获取材质的漫反射颜色
        auto diffuseColor = material.getProperty<Property::Wrapper::RGBType>("diffuseColor");
        if (diffuseColor) albedo = (*diffuseColor).value;
        else albedo = {1, 1, 1};  // 默认白色
    }
    
    /**
     * Lambertian散射计算
     * 实现理想的漫反射，光线在所有方向上均匀散射
     * @param ray 入射光线
     * @param hitPoint 相交点
     * @param normal 法向量
     * @return 散射信息
     */
    Scattered Lambertian::shade(const Ray& ray, const Vec3& hitPoint, const Vec3& normal) const {
        Vec3 origin = hitPoint;
        
        // 在半球内均匀采样散射方向
        Vec3 random = defaultSamplerInstance<HemiSphere>().sample3d();
        
        // 使用正交基将半球采样结果转换到以法向量为z轴的局部坐标系
        Onb onb{normal};
        Vec3 direction = glm::normalize(onb.local(random));

        // Lambertian散射的概率密度函数：1/(2π)
        float pdf = 1/(2*PI);

        // Lambertian BRDF：albedo/π
        auto attenuation = albedo / PI;

        return {
            Ray{origin, direction},  // 散射光线
            attenuation,             // 衰减系数
            Vec3{0},                // 无自发光
            pdf                     // 概率密度函数值
        };
    }
}