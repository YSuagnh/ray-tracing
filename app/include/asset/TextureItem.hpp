#pragma once
#ifndef __NR_TEXTURE_ITEM_HPP__
#define __NR_TEXTURE_ITEM_HPP__

// 纹理资产项头文件
// 定义了场景中纹理的数据结构

#include "scene/Texture.hpp"
#include "Item.hpp"
#include "utilities/GlImage.hpp"

namespace NRenderer
{
    // 纹理资产项
    // 包含了纹理数据和对应的OpenGL纹理对象
    struct TextureItem : public Item
    {
        SharedTexture texture;  // 纹理数据的智能指针
        GlImageId glId;        // OpenGL纹理对象ID
    };
} // namespace NRenderer

#endif