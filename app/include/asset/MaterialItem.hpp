#pragma once
#ifndef __NR_MATERIAL_ITEM_HPP__
#define __NR_MATERIAL_ITEM_HPP__

// 材质资产项头文件
// 定义了场景中材质的数据结构

#include "scene/Material.hpp"
#include "Item.hpp"

namespace NRenderer
{
    // 材质资产项
    // 包含了一个场景材质的引用
    struct MaterialItem : public Item
    {
        SharedMaterial material;  // 材质数据的智能指针
    };
} // namespace NRenderer

#endif