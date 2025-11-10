#pragma once
#ifndef __VERTEX_TRANSFORM_HPP__
#define __VERTEX_TRANSFORM_HPP__

// 顶点变换器头文件
// 定义了场景中模型顶点的坐标变换功能

#include "scene/Scene.hpp"

namespace RayCast
{
    using namespace NRenderer;

    // 顶点变换器类
    // 负责将场景中的模型从局部坐标转换到世界坐标
    class VertexTransformer
    {
    private:
    public:
        // 执行顶点变换
        // spScene: 场景指针
        // 将场景中所有模型从局部坐标转换到世界坐标
        void exec(SharedScene spScene);
    };
}

#endif