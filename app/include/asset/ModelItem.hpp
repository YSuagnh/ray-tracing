#pragma once
#ifndef __NR_MODEL_ITEM_HPP__
#define __NR_MODEL_ITEM_HPP__

// 模型资产项头文件
// 定义了场景中模型和节点的数据结构

#include "scene/Model.hpp"
#include "Item.hpp"

#include "glad/glad.h"

namespace NRenderer
{
    using namespace std;

    // 模型资产项
    // 包含了一个场景模型的引用
    struct ModelItem : public Item
    {
        SharedModel model{nullptr};  // 模型数据的智能指针
    };

    // 节点资产项
    // 包含了模型节点的OpenGL渲染数据
    struct NodeItem : public Item
    {
        GlId glVAO{0};  // 顶点数组对象
        GlId glVBO{0};  // 顶点缓冲对象
        GlId glEBO{0};  // 索引缓冲对象
        SharedNode node{nullptr};  // 节点数据的智能指针
        SharedGlDrawData externalDrawData{nullptr};  // 外部绘制数据
    };
    
} // namespace NRenderer

#endif