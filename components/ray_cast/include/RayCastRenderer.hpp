#pragma once
#ifndef __RAY_CAST_HPP__
#define __RAY_CAST_HPP__

// 光线投射渲染器头文件
// 定义了光线投射渲染器的核心功能

#include "scene/Scene.hpp"
#include "Camera.hpp"
#include "intersections/intersections.hpp"
#include "shaders/ShaderCreator.hpp"

namespace RayCast
{
    using namespace NRenderer;

    // 光线投射渲染器类
    // 实现了基本的光线投射渲染算法
    class RayCastRenderer
    {
    private:
        SharedScene spScene;                    // 场景指针
        Scene& scene;                           // 场景引用
        RayCast::Camera camera;                 // 相机
        vector<SharedShader> shaderPrograms;    // 着色器程序列表

    public:
        // 构造函数
        // spScene: 场景指针
        RayCastRenderer(SharedScene spScene)
            : spScene               (spScene)
            , scene                 (*spScene)
            , camera                (spScene->camera)
        {}

        // 析构函数
        ~RayCastRenderer() = default;

        // 渲染结果类型：像素数据、宽度、高度
        using RenderResult = tuple<RGBA*, unsigned int, unsigned int>;

        // 渲染场景
        // 返回渲染结果
        RenderResult render();

        // 释放渲染结果
        // r: 要释放的渲染结果
        void release(const RenderResult& r);

    private:
        // 伽马校正
        // rgb: 输入的RGB颜色
        // 返回校正后的颜色
        RGB gamma(const RGB& rgb);

        // 光线追踪
        // r: 输入光线
        // 返回该光线对应的颜色
        RGB trace(const Ray& r);

        // 计算最近的相交物体
        // r: 输入光线
        // 返回最近的相交记录
        HitRecord closestHit(const Ray& r);
    };
}

#endif