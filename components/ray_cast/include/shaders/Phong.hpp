#pragma once
#ifndef __PHONG_HPP__
#define __PHONG_HPP__

// Phong材质着色器头文件
// 定义了Phong光照模型的着色计算

#include "Shader.hpp"

namespace RayCast
{
    // Phong材质着色器类
    // 实现了Phong光照模型，包括漫反射和镜面反射
    class Phong : public Shader
    {
    private:
        Vec3 diffuseColor;   // 漫反射颜色
        Vec3 specularColor;  // 镜面反射颜色
        float specularEx;    // 镜面反射指数

    public:
        // 构造函数
        // material: 材质参数
        // textures: 纹理数组
        Phong(Material& material, vector<Texture>& textures);

        // 计算着色结果
        // in: 入射方向
        // out: 出射方向
        // normal: 表面法线
        // 返回着色计算的颜色值
        virtual RGB shade(const Vec3& in, const Vec3& out, const Vec3& normal) const;
    };
}

#endif