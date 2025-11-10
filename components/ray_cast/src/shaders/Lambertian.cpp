// Lambert材质着色器实现
// 实现了Lambert漫反射材质的着色计算
#include "shaders/Lambertian.hpp"

namespace RayCast
{
    // 构造函数
    // material: 材质参数
    // textures: 纹理数组
    Lambertian::Lambertian(Material& material, vector<Texture>& textures)
        : Shader                (material, textures)
    {
        // 获取漫反射颜色
        auto optDiffuseColor = material.getProperty<Property::Wrapper::RGBType>("diffuseColor");
        if (optDiffuseColor) diffuseColor = (*optDiffuseColor).value;
        else diffuseColor = {1, 1, 1};  // 默认为白色
    }

    // 计算着色结果
    // in: 入射方向
    // out: 出射方向
    // normal: 表面法线
    // 返回着色计算的颜色值
    RGB Lambertian::shade(const Vec3& in, const Vec3& out, const Vec3& normal) const {
        // 计算Lambert漫反射：diffuseColor * cos(theta)
        return diffuseColor * glm::dot(out, normal);
    }
}