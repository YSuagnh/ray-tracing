#include "importer/TextureImporter.hpp"
#include "utilities/ImageLoader.hpp"
#include "utilities/GlImage.hpp"
#include "asset/TextureItem.hpp"

// 纹理导入器实现文件
// 负责从文件中导入纹理并创建OpenGL纹理对象

namespace NRenderer
{
    // 导入纹理文件
    // asset: 资产管理器
    // path: 纹理文件路径
    // 返回值: 导入是否成功
    bool TextureImporter::import(Asset& asset, const string& path) {
        ImageLoader imgLoader;
        auto img = imgLoader.load(path);                // 加载图像文件

        // 创建纹理对象
        SharedTexture spTexture{new Texture()};
        spTexture->width = img->width;                 // 设置纹理宽度
        spTexture->height = img->height;               // 设置纹理高度
        spTexture->rgba = (RGBA*)img->data;           // 设置纹理数据

        // 创建OpenGL纹理
        auto id = GlImage::loadImage(spTexture->rgba, {spTexture->width, spTexture->height});

        // 创建纹理项
        TextureItem ti;
        ti.glId = id;                                 // OpenGL纹理ID
        ti.name = path;                               // 纹理名称（文件路径）
        ti.texture = spTexture;                       // 纹理数据
        asset.textureItems.push_back(move(ti));       // 添加到资产管理器

        return true;
    }
}