#pragma once
#ifndef __VERTEX_TRANSFORM_HPP__
#define __VERTEX_TRANSFORM_HPP__

#include "scene/Scene.hpp"

namespace SimplePathTracer
{
    using namespace NRenderer;
    
    /**
     * 顶点变换器
     * 负责将场景中的几何体从局部坐标系转换到世界坐标系
     * 应用模型的变换矩阵（平移、旋转、缩放）
     */
    class VertexTransformer
    {
    private:
    public:
        /**
         * 执行顶点变换
         * 将场景中所有几何体的顶点从局部坐标转换到世界坐标
         * @param spScene 场景共享指针
         */
        void exec(SharedScene spScene);
    };
}

#endif