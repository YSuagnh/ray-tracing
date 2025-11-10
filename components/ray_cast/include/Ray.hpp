#pragma once
#ifndef __RAY_HPP__
#define __RAY_HPP__

// 光线类头文件
// 定义了光线的基本属性和操作

#include "geometry/vec.hpp"
#include <limits>

#define FLOAT_INF numeric_limits<float>::infinity()

namespace RayCast
{
    using namespace NRenderer;
    using namespace std;

    // 光线结构体
    // 包含光线的起点和方向
    struct Ray
    {
        Vec3 origin;      // 光线起点
        Vec3 direction;   // 光线方向（单位向量）

        // 设置光线起点
        // v: 新的起点位置
        void setOrigin(const Vec3& v) {
            origin = v;
        }

        // 设置光线方向
        // v: 新的方向向量（会被归一化）
        void setDirection(const Vec3& v) {
            direction = glm::normalize(v);
        }

        // 计算光线上的点
        // t: 参数值（距离）
        // 返回光线上距离起点t距离的点
        inline
        Vec3 at(float t) const {
            return origin + t*direction;
        }

        // 构造函数
        // origin: 光线起点
        // direction: 光线方向
        Ray(const Vec3& origin, const Vec3& direction)
            : origin                (origin)
            , direction             (direction)
        {}
    
        // 默认构造函数
        // 创建一个起点和方向都为零向量的光线
        Ray()
            : origin        {}
            , direction     {}
        {}
    };
}

#endif