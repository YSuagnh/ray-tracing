#pragma once
#ifndef __SCATTERED_HPP__
#define __SCATTERED_HPP__

#include "Ray.hpp"

namespace SimplePathTracer
{
    /**
     * 散射信息结构体
     * 记录光线与材质交互后的散射信息
     */
    struct Scattered
    {
        Ray ray = {};           // 散射后的光线方向
        Vec3 attenuation = {};  // 衰减系数（BRDF）
        Vec3 emitted = {};      // 自发光强度
        float pdf = {0.f};      // 概率密度函数值
    };
    
}

#endif