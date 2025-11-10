#pragma once
#ifndef __NR_LIGHT_ITEM_HPP__
#define __NR_LIGHT_ITEM_HPP__

// 光源资产项头文件
// 定义了场景中光源的数据结构

#include "scene/Light.hpp"
#include "Item.hpp"

namespace NRenderer
{
    using namespace std;

    // 光源资产项
    // 包含了光源的OpenGL渲染数据和光源属性
    struct LightItem : public Item
    {
        GlId glVAO{0};  // 顶点数组对象
        GlId glVBO{0};  // 顶点缓冲对象
        GlId glEBO{0};  // 索引缓冲对象
        SharedLight light;  // 光源数据的智能指针
        SharedGlDrawData externalDrawData;  // 外部绘制数据（用于可视化光源）
    };
    
} // namespace NRenderer

#endif