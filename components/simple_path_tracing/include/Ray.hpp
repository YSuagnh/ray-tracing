#pragma once
#ifndef __RAY_HPP__
#define __RAY_HPP__

#include "geometry/vec.hpp"

#include <limits>

#define FLOAT_INF numeric_limits<float>::infinity()
namespace SimplePathTracer
{
    using namespace NRenderer;
    using namespace std;

    /**
     * 光线结构体
     * 表示从起点出发沿特定方向传播的光线
     */
    struct Ray
    {
        Vec3 origin;        // 光线起点
        Vec3 direction;     // 光线方向（保持为单位向量）

        /**
         * 设置光线起点
         * @param v 新的起点
         */
        void setOrigin(const Vec3& v) {
            origin = v;
        }

        /**
         * 设置光线方向
         * @param v 新的方向向量（会自动归一化）
         */
        void setDirection(const Vec3& v) {
            direction = glm::normalize(v);
        }

        /**
         * 计算光线在参数t处的点
         * @param t 参数值
         * @return 光线上的点
         */
        inline
        Vec3 at(float t) const {
            return origin + t*direction;
        }

        /**
         * 构造函数
         * @param origin 光线起点
         * @param direction 光线方向
         */
        Ray(const Vec3& origin, const Vec3& direction)
            : origin                (origin)
            , direction             (direction)
        {}
    
        /**
         * 默认构造函数
         */
        Ray()
            : origin        {}
            , direction     {}
        {}
    };
}

#endif