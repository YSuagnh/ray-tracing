#pragma once
#ifndef __PBR_HPP__
#define __PBR_HPP__

#include "Shader.hpp"

namespace RayTracer
{
    /**
     * PBR材质着色器
     * 实现基于物理的渲染（Physically Based Rendering）
     * 使用Cook-Torrance BRDF模型，支持金属度和粗糙度工作流
     */
    class PBR : public Shader
    {
    private:
        Vec3 baseColor;         // 基础颜色（反照率）
        float metallic;         // 金属度 [0,1]
        float roughness;        // 粗糙度 [0,1]
        float ior;              // 折射率（介电质材料）
        Vec3 emissive;          // 自发光颜色
        
        // PBR计算辅助函数
        float DistributionGGX(const Vec3& N, const Vec3& H, float roughness) const;
        float GeometrySchlickGGX(float NdotV, float roughness) const;
        float GeometrySmith(const Vec3& N, const Vec3& V, const Vec3& L, float roughness) const;
        Vec3 FresnelSchlick(float cosTheta, const Vec3& F0) const;
        Vec3 ImportanceSampleGGX(const Vec3& N, float roughness) const;
        
    public:
        /**
         * 构造函数
         * @param material 材质对象
         * @param textures 纹理缓冲区
         */
        PBR(Material& material, vector<Texture>& textures);
        
        /**
         * 计算PBR散射
         * @param ray 入射光线
         * @param hitPoint 相交点
         * @param normal 法向量
         * @return 散射信息
         */
        Scattered shade(const Ray& ray, const Vec3& hitPoint, const Vec3& normal) const override;
    };
}

#endif