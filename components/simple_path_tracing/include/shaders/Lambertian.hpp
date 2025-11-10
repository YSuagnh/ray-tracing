#pragma once
#ifndef __LAMBERTIAN_HPP__
#define __LAMBERTIAN_HPP__

#include "Shader.hpp"

namespace SimplePathTracer
{
    /**
     * Lambertian材质着色器
     * 实现理想的漫反射材质，遵循Lambert余弦定律
     * 光线在所有方向上均匀散射，散射强度与入射角度的余弦成正比
     */
    class Lambertian : public Shader
    {
    private:
        Vec3 albedo;  // 反射率（漫反射颜色）
        
    public:
        /**
         * 构造函数
         * @param material 材质对象
         * @param textures 纹理缓冲区
         */
        Lambertian(Material& material, vector<Texture>& textures);
        
        /**
         * 计算Lambertian散射
         * @param ray 入射光线
         * @param hitPoint 相交点
         * @param normal 法向量
         * @return 散射信息
         */
        Scattered shade(const Ray& ray, const Vec3& hitPoint, const Vec3& normal) const;
    };
}

#endif