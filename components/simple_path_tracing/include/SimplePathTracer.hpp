#pragma once
#ifndef __SIMPLE_PATH_TRACER_HPP__
#define __SIMPLE_PATH_TRACER_HPP__

#include "scene/Scene.hpp"
#include "Ray.hpp"
#include "Camera.hpp"
#include "intersections/HitRecord.hpp"

#include "shaders/ShaderCreator.hpp"

#include <tuple>
namespace SimplePathTracer
{
    using namespace NRenderer;
    using namespace std;

    /**
     * 简单路径追踪渲染器
     * 实现基于蒙特卡洛方法的路径追踪算法
     * 支持多种几何体和Lambertian材质
     */
    class SimplePathTracerRenderer
    {
    public:
    private:
        SharedScene spScene;        // 场景共享指针
        Scene& scene;               // 场景引用

        unsigned int width;         // 图像宽度
        unsigned int height;        // 图像高度
        unsigned int depth;         // 最大递归深度
        unsigned int samples;       // 每像素采样数

        using SCam = SimplePathTracer::Camera;
        SCam camera;                // 相机对象

        vector<SharedShader> shaderPrograms;  // 着色器程序列表
        
    public:
        /**
         * 构造函数
         * @param spScene 场景共享指针
         */
        SimplePathTracerRenderer(SharedScene spScene)
            : spScene               (spScene)
            , scene                 (*spScene)
            , camera                (spScene->camera)
        {
            width = scene.renderOption.width;
            height = scene.renderOption.height;
            depth = scene.renderOption.depth;
            samples = scene.renderOption.samplesPerPixel;
        }
        ~SimplePathTracerRenderer() = default;

        using RenderResult = tuple<RGBA*, unsigned int, unsigned int>;  // 渲染结果类型
        
        /**
         * 执行渲染
         * @return 渲染结果（像素数据、宽度、高度）
         */
        RenderResult render();
        
        /**
         * 释放渲染结果内存
         * @param r 渲染结果
         */
        void release(const RenderResult& r);

    private:
        /**
         * 渲染任务（多线程）
         * @param pixels 像素缓冲区
         * @param width 图像宽度
         * @param height 图像高度
         * @param off 起始偏移
         * @param step 步长
         */
        void renderTask(RGBA* pixels, int width, int height, int off, int step);

        /**
         * Gamma校正
         * @param rgb 原始颜色
         * @return 校正后的颜色
         */
        RGB gamma(const RGB& rgb);
        
        /**
         * 路径追踪主函数
         * @param ray 光线
         * @param currDepth 当前递归深度
         * @return 光线颜色
         */
        RGB trace(const Ray& ray, int currDepth);
        
        /**
         * 查找最近相交的物体
         * @param r 光线
         * @return 相交记录
         */
        HitRecord closestHitObject(const Ray& r);
        
        /**
         * 查找最近相交的光源
         * @param r 光线
         * @return 相交距离和发光强度
         */
        tuple<float, Vec3> closestHitLight(const Ray& r);
    };
}

#endif