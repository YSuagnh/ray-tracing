#pragma once
#ifndef __INTERSECTIONS_HPP__
#define __INTERSECTIONS_HPP__

#include "HitRecord.hpp"
#include "Ray.hpp"
#include "scene/Scene.hpp"

namespace SimplePathTracer
{
    /**
     * 光线与几何体相交检测命名空间
     * 提供各种几何体与光线的相交检测算法
     */
    namespace Intersection
    {
        /**
         * 光线与三角形相交检测
         * @param ray 光线
         * @param t 三角形
         * @param tMin 最小参数值
         * @param tMax 最大参数值
         * @return 相交记录
         */
        HitRecord xTriangle(const Ray& ray, const Triangle& t, float tMin = 0.f, float tMax = FLOAT_INF);
        
        /**
         * 光线与球体相交检测
         * @param ray 光线
         * @param s 球体
         * @param tMin 最小参数值
         * @param tMax 最大参数值
         * @return 相交记录
         */
        HitRecord xSphere(const Ray& ray, const Sphere& s, float tMin = 0.f, float tMax = FLOAT_INF);
        
        /**
         * 光线与平面相交检测
         * @param ray 光线
         * @param p 平面
         * @param tMin 最小参数值
         * @param tMax 最大参数值
         * @return 相交记录
         */
        HitRecord xPlane(const Ray& ray, const Plane& p, float tMin = 0.f, float tMax = FLOAT_INF);
        
        /**
         * 光线与区域光源相交检测
         * @param ray 光线
         * @param a 区域光源
         * @param tMin 最小参数值
         * @param tMax 最大参数值
         * @return 相交记录
         */
        HitRecord xAreaLight(const Ray& ray, const AreaLight& a, float tMin = 0.f, float tMax = FLOAT_INF);
    }
}

#endif