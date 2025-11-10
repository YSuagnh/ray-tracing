#pragma once
#ifndef __SHADER_HPP__
#define __SHADER_HPP__

#include "geometry/vec.hpp"
#include "common/macros.hpp"
#include "scene/Scene.hpp"

#include "Scattered.hpp"

namespace SimplePathTracer
{
    using namespace NRenderer;
    using namespace std;

    constexpr float PI = 3.1415926535898f;  // 圆周率常量

    /**
     * 着色器基类
     * 定义材质与光线交互的接口，实现不同的光照模型
     */
    class Shader
    {
    protected:
        Material& material;              // 材质引用
        vector<Texture>& textureBuffer;  // 纹理缓冲区引用
        
    public:
        /**
         * 构造函数
         * @param material 材质对象
         * @param textures 纹理缓冲区
         */
        Shader(Material& material, vector<Texture>& textures)
            : material              (material)
            , textureBuffer         (textures)
        {}
        
        /**
         * 计算光线与材质的交互结果
         * @param ray 入射光线
         * @param hitPoint 相交点
         * @param normal 法向量
         * @return 散射信息
         */
        virtual Scattered shade(const Ray& ray, const Vec3& hitPoint, const Vec3& normal) const = 0;
    };
    SHARE(Shader);  // 定义共享指针类型
}

#endif