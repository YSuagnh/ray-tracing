#pragma once
#ifndef __LAMBERTIAN_HPP__
#define __LAMBERTIAN_HPP__

// Lambert材质着色器头文件
// 定义了Lambert漫反射材质的着色计算

#include "Shader.hpp"

namespace RayCast
{
    // Lambert材质着色器类
    // 实现了Lambert漫反射模型
    class Lambertian : public Shader
    {
    private:
        Vec3 diffuseColor;  // 漫反射颜色

    public:
        // 构造函数
        // material: 材质参数
        // textures: 纹理数组
        Lambertian(Material& material, vector<Texture>& textures);

        // 计算着色结果
        // in: 入射方向
        // out: 出射方向
        // normal: 表面法线
        // 返回着色计算的颜色值
        virtual RGB shade(const Vec3& in, const Vec3& out, const Vec3& normal) const;
    };
}

#endif