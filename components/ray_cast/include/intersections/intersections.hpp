#pragma once
#ifndef __INTERSECTIONS_HPP__
#define __INTERSECTIONS_HPP__

// 相交测试函数头文件
// 声明了各种几何体与光线的相交测试函数

#include "HitRecord.hpp"
#include "Ray.hpp"
#include "scene/Scene.hpp"

namespace RayCast
{
    // 相交测试命名空间
    // 包含了所有几何体的相交测试函数
    namespace Intersection
    {
        // 光线与三角形的相交测试
        // ray: 光线
        // t: 三角形
        // tMin, tMax: 相交距离范围
        // 返回相交记录
        HitRecord xTriangle(const Ray& ray, const Triangle& t, float tMin = 0.f, float tMax = FLOAT_INF);

        // 光线与球体的相交测试
        // ray: 光线
        // s: 球体
        // tMin, tMax: 相交距离范围
        // 返回相交记录
        HitRecord xSphere(const Ray& ray, const Sphere& s, float tMin = 0.f, float tMax = FLOAT_INF);

        // 光线与平面的相交测试
        // ray: 光线
        // p: 平面
        // tMin, tMax: 相交距离范围
        // 返回相交记录
        HitRecord xPlane(const Ray& ray, const Plane& p, float tMin = 0.f, float tMax = FLOAT_INF);

        // 光线与面光源的相交测试
        // ray: 光线
        // a: 面光源
        // tMin, tMax: 相交距离范围
        // 返回相交记录
        HitRecord xAreaLight(const Ray& ray, const AreaLight& a, float tMin = 0.f, float tMax = FLOAT_INF);
    }
}

#endif