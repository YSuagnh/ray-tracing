// 光源类型定义
// 定义了场景中的各种光源类型，包括点光源、聚光灯、平行光和面积光
#pragma once
#ifndef __NR_LIGHT_HPP__
#define __NR_LIGHT_HPP__

#include "geometry/vec.hpp"
#include <numbers>

#include <memory>
#include <string>

#include "Model.hpp"

namespace NRenderer
{
    // 基础光源结构体
    // 定义了光源的基本属性和类型
    struct Light
    {
        // 光源类型枚举
        enum class Type
        {
            POINT = 0x0,                // 点光源：从一个点向所有方向发射光线
            SPOT,                       // 聚光灯：从一个点向特定方向发射锥形光线
            DIRECTIONAL,                // 平行光：所有光线平行，模拟远处光源（如太阳）
            AREA                        // 面积光：从一个面积发射光线，产生软阴影
        };
        Type type;        // 光源类型
        Index entity;     // 光源实体的索引，用于关联具体的光源数据

        // 构造函数
        Light(Type type)
            : type                  (type)
            , entity                (0)
        {}
        Light() = default;
    };
    SHARE(Light);

    // 面积光源
    // 定义了一个矩形面积光源，可以产生软阴影效果
    struct AreaLight
    {
        Vec3 radiance = {1, 1, 1};  // 辐射度（光源强度）
        Vec3 position = {};         // 光源位置（矩形的一个角点）
        Vec3 u = {};               // 矩形的第一个边向量
        Vec3 v = {};               // 矩形的第二个边向量
    };
    SHARE(AreaLight);

    // 点光源
    // 从一个点向所有方向均匀发射光线
    struct PointLight
    {
        Vec3 intensity = {1, 1, 1};  // 光源强度
        Vec3 position = {};          // 光源位置
    };
    SHARE(PointLight);

    // 平行光源
    // 所有光线平行，用于模拟远处的光源
    struct DirectionalLight
    {
        Vec3 irradiance = {1, 1, 1}; // 辐照度
        Vec3 direction = {};         // 光线方向
    };
    SHARE(DirectionalLight);

    // 聚光灯
    // 从一个点向特定方向发射锥形光线
    struct SpotLight
    {
        Vec3 intensity = {1, 1, 1};  // 光源强度
        Vec3 position = {};          // 光源位置
        Vec3 direction = {};         // 光照方向
        float hotSpot = 3.1415926f/4.f;  // 热点角度（内锥角）
        float fallout = 3.1415926f/3.f;  // 衰减角度（外锥角）
    };
    SHARE(SpotLight);
} // namespace Light


#endif