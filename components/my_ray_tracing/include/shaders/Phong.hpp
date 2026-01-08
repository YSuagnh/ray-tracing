#pragma once
#ifndef __PHONG_HPP__
#define __PHONG_HPP__

#include "Shader.hpp"

namespace RayTracer
{
    /**
     * Phong材质着色器
     * 实现经典的Phong光照模型，包含环境光、漫反射和镜面反射分量
     * 适用于具有高光特性的材质渲染
     */
    class Phong : public Shader
    {
    private:
        Vec3 diffuse;           // 漫反射颜色
        Vec3 specular;          // 镜面反射颜色
        float shininess;        // 高光指数（光泽度）
        Vec3 ambient;           // 环境光颜色
        
    public:
        /**
         * 构造函数
         * @param material 材质对象
         * @param textures 纹理缓冲区
         */
        Phong(Material& material, vector<Texture>& textures);
        
        /**
         * 计算Phong光照散射
         * @param ray 入射光线
         * @param hitPoint 相交点
         * @param normal 法向量
         * @return 散射信息
         */
        Scattered shade(const Ray& ray, const Vec3& hitPoint, const Vec3& normal) const;
    };
}

#endif