#pragma once
#ifndef __NR_ASSET_HPP__
#define __NR_ASSET_HPP__

// 资产管理头文件
// 定义了场景中所有资产的数据结构和管理功能

#include "common/macros.hpp"

#include "ModelItem.hpp"
#include "MaterialItem.hpp"
#include "TextureItem.hpp"
#include "LightItem.hpp"

namespace NRenderer
{
    // 资产结构体
    // 包含了场景中所有的模型、材质、纹理和光源数据
    struct Asset
    {
        // 基础资产项
        vector<ModelItem> modelItems;         // 模型项列表
        vector<NodeItem> nodeItems;           // 节点项列表
        vector<MaterialItem> materialItems;    // 材质项列表
        vector<TextureItem> textureItems;     // 纹理项列表
        vector<LightItem> lightItems;         // 光源项列表

        // 几何体资产
        vector<SharedSphere> spheres;         // 球体列表
        vector<SharedTriangle> triangles;     // 三角形列表
        vector<SharedPlane> planes;           // 平面列表
        vector<SharedMesh> meshes;            // 网格列表

        // 光源资产
        vector<SharedPointLight> pointLights;          // 点光源列表
        vector<SharedAreaLight> areaLights;           // 面光源列表
        vector<SharedDirectionalLight> directionalLights;  // 平行光列表
        vector<SharedSpotLight> spotLights;           // 聚光灯列表

        // 清除模型数据
        // 删除所有模型相关的OpenGL缓冲区并清空模型列表
        void clearModel() {
            for (auto& node : nodeItems) {
                if (node.glVAO != 0) {
                    glDeleteVertexArrays(1, &node.glVAO);
                }
                if (node.glVBO != 0) {
                    glDeleteBuffers(1, &node.glVBO);
                }
                if (node.glEBO != 0) {
                    glDeleteBuffers(1, &node.glEBO);
                }
            }
            for (auto& light : lightItems) {
                if (light.glVAO != 0) {
                    glDeleteVertexArrays(1, &light.glVAO);
                    light.glVAO = 0;
                }
                if (light.glVBO != 0) {
                    glDeleteBuffers(1, &light.glVBO);
                    light.glVBO = 0;
                }
                if (light.glEBO != 0) {
                    glDeleteBuffers(1, &light.glEBO);
                    light.glEBO = 0;
                }
            }
            modelItems.clear();
            nodeItems.clear();

            spheres.clear();
            triangles.clear();
            planes.clear();
            meshes.clear();
        }

        // 清除光源数据
        // 清空所有光源列表
        void clearLight() {
            lightItems.clear();

            pointLights.clear();
            areaLights.clear();
            directionalLights.clear();
            spotLights.clear();
        }

        // 清除材质数据
        // 清空材质列表
        void clearMaterial() {
            materialItems.clear();
        }

        // 清除纹理数据
        // 清空纹理列表
        void clearTexture() {
            textureItems.clear();
        }

        // 为节点生成预览用的OpenGL缓冲区
        void genPreviewGlBuffersPerNode(NodeItem& node);
        // 为光源生成预览用的OpenGL缓冲区
        void genPreviewGlBuffersPerLight(LightItem& light);

        // 更新节点的OpenGL绘制数据
        void updateNodeGlDrawData(NodeItem& node);
        // 更新光源的OpenGL绘制数据
        void updateLightGlDrawData(LightItem& light);
    };
    
} // namespace NRenderer


#endif