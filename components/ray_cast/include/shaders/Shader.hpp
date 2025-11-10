#pragma once
#ifndef __SHADER_HPP__
#define __SHADER_HPP__

// 着色器基类头文件
// 定义了所有着色器的基本接口

#include "geometry/vec.hpp"
#include "common/macros.hpp"
#include "scene/Scene.hpp"

namespace RayCast
{
    using namespace NRenderer;
    using namespace std;

    constexpr float PI = 3.1415926535898f;  // 圆周率常量

    // 着色器基类
    // 提供了材质着色计算的基本接口
    class Shader
    {
    protected:
        Material& material;          // 材质参数
        vector<Texture>& textureBuffer;  // 纹理数组

    public:
        // 构造函数
        // material: 材质参数
        // textures: 纹理数组
        Shader(Material& material, vector<Texture>& textures)
            : material              (material)
            , textureBuffer         (textures)
        {}

        // 计算着色结果
        // in: 入射方向
        // out: 出射方向
        // normal: 表面法线
        // 返回着色计算的颜色值
        virtual RGB shade(const Vec3& in, const Vec3& out, const Vec3& normal) const = 0;
    };
    SHARE(Shader);  // 声明智能指针类型
}

#endif