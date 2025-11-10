// Phong材质着色器实现
// 实现了Phong光照模型的着色计算
#include "shaders/Phong.hpp"

namespace RayCast
{
    // 计算反射向量
    // normal: 表面法线
    // dir: 入射方向
    // 返回反射方向
    Vec3 reflect(const Vec3& normal, const Vec3& dir) {
        return dir - 2*glm::dot(dir, normal)*normal;
    }

    // 构造函数
    // material: 材质参数
    // textures: 纹理数组
    Phong::Phong(Material& material, vector<Texture>& textures)
        : Shader                (material, textures)
    {
        using PW = Property::Wrapper;
        // 获取漫反射颜色
        auto optDiffuseColor = material.getProperty<PW::RGBType>("diffuseColor");
        if (optDiffuseColor) diffuseColor = (*optDiffuseColor).value;
        else diffuseColor = {1, 1, 1};  // 默认为白色
        
        // 获取镜面反射颜色
        auto optSpecularColor = material.getProperty<PW::RGBType>("specularColor");
        if (optSpecularColor) specularColor = (*optSpecularColor).value;
        else specularColor = {1, 1, 1};  // 默认为白色

        // 获取镜面反射指数
        auto optSpecularEx = material.getProperty<PW::FloatType>("specularEx");
        if (optSpecularEx) specularEx = (*optSpecularEx).value;
        else specularEx = 1;  // 默认为1
    }

    // 计算着色结果
    // in: 视线方向
    // out: 光源方向
    // normal: 表面法线
    // 返回着色计算的颜色值
    RGB Phong::shade(const Vec3& in, const Vec3& out, const Vec3& normal) const {
        Vec3 v = in;  // 视线方向
        Vec3 r = reflect(normal, out);  // 反射方向
        // 计算漫反射项：diffuseColor * cos(theta)
        auto diffuse = diffuseColor * glm::dot(out, normal);
        // 计算镜面反射项：specularColor * (cos(alpha))^n
        auto specular = specularColor * fabs(glm::pow(glm::dot(v, r), specularEx));
        // 返回漫反射和镜面反射的叠加
        return diffuse + specular;
    }
}