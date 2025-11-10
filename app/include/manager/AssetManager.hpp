#pragma once
#ifndef __NR_ASSET_MANAGER_HPP__
#define __NR_ASSET_MANAGER_HPP__

// 资产管理器头文件
// 定义了管理场景资产的功能类

#include "asset/Asset.hpp"
#include "importer/TextureImporter.hpp"
#include "utilities/FileFetcher.hpp"
#include "importer/SceneImporterFactory.hpp"
#include "utilities/File.hpp"
#include "server/Server.hpp"

namespace NRenderer
{
    // 资产管理器结构体
    // 负责管理和操作场景中的所有资产
    struct AssetManager
    {
        Asset asset;  // 场景资产实例

        // 导入场景文件
        // 支持导入 .scn 和 .obj 格式的场景文件
        void importScene() {
            FileFetcher ff;
            auto optPath = ff.fetch("All\0*.scn;*.obj\0");
            if (optPath) {
                auto importer = SceneImporterFactory::instance().importer(File::getFileExtension(*optPath));
                bool success = importer->import(asset, *optPath);
                if (!success) {
                    getServer().logger.error(importer->getErrorInfo());
                }
                else {
                    getServer().logger.success("成功导入:" + *optPath);
                }
            }
        }

        // 导入纹理文件
        // 支持导入 .png 和 .jpg 格式的图片文件
        void importTexture() {
            TextureImporter tImp{};
            FileFetcher ff;
            auto optPath = ff.fetch("image\0*.png;*.jpg\0");
            if (optPath) {
                tImp.import(asset, *optPath);
            }
        }

        // 清除所有资产
        // 包括模型、光源、材质和纹理
        void clearAll() {
            asset.clearModel();
            asset.clearLight();
            asset.clearMaterial();
            asset.clearTexture();
        }

        // 清除模型资产
        void clearModel() {
            asset.clearModel();
        }

        // 清除光源资产
        void clearLight() {
            asset.clearLight();
        }
    };
}


#endif
