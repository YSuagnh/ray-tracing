#pragma once
#ifndef __NR_TEXTURE_IMPORTER_HPP__
#define __NR_TEXTURE_IMPORTER_HPP__

// 纹理导入器头文件
// 定义了导入纹理文件的功能

#include "Importer.hpp"

namespace NRenderer
{
    // 纹理导入器类
    // 负责导入和解析纹理文件
    class TextureImporter : public Importer
    {
    public:
        // 默认构造函数
        TextureImporter() = default;
        // 默认析构函数
        ~TextureImporter() = default;

        // 导入纹理文件
        // asset: 目标资产对象
        // path: 纹理文件路径
        // 返回: 导入是否成功
        virtual bool import(Asset& asset, const string& path) override;
    };
}

#endif