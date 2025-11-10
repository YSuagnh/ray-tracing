#include "asset/SceneBuilder.hpp"

// 场景构建器实现文件
// 负责将资产管理器中的数据构建成渲染场景

namespace NRenderer
{
    // 构建渲染选项
    // 设置渲染深度、采样数、分辨率等参数
    void SceneBuilder::buildRenderOption() {
        RenderOption ro;
        ro.depth = renderSettings.depth;                    // 光线追踪深度
        ro.samplesPerPixel = renderSettings.samplesPerPixel;  // 每像素采样数
        ro.width = renderSettings.width;                    // 渲染宽度
        ro.height = renderSettings.height;                  // 渲染高度
        this->scene->renderOption = ro;
    }

    // 构建场景缓冲区
    // 将所有资产数据复制到场景中
    void SceneBuilder::buildBuffer() {
        // 复制材质、纹理和模型数据
        for (auto& mi : asset.materialItems) {
            this->scene->materials.push_back(*mi.material);
        }
        for (auto& ti : asset.textureItems) {
            this->scene->textures.push_back(*ti.texture);
        }
        for (auto& mi : asset.modelItems) {
            this->scene->models.push_back(*mi.model);
        }

        // 复制节点数据并检查材质有效性
        for (auto& ni : asset.nodeItems) {
            this->scene->nodes.push_back(*ni.node);
            if (ni.node->type == Node::Type::SPHERE) {
                if (!asset.spheres[ni.node->entity]->material.valid()) {
                    success = false;
                    string err{};
                    err = "No material is specified for node "+ni.name+".";
                }
            }
            if (ni.node->type == Node::Type::PLANE) {
                if (!asset.planes[ni.node->entity]->material.valid()) {
                    success = false;
                    string err{};
                    err = "No material is specified for node "+ni.name+".";
                }
            }
            if (ni.node->type == Node::Type::TRIANGLE) {
                if (!asset.triangles[ni.node->entity]->material.valid()) {
                    success = false;
                    string err{};
                    err = "No material is specified for node "+ni.name+".";
                }
            }
            if (ni.node->type == Node::Type::MESH) {
                if (!asset.meshes[ni.node->entity]->material.valid()) {
                    success = false;
                    string err{};
                    err = "No material is specified for node "+ni.name+".";
                }
            }
        }

        // 复制光源数据
        for (auto& li : asset.lightItems) {
            this->scene->lights.push_back(*li.light);
        }

        // 复制几何体数据
        for (auto& s : asset.spheres) {
            this->scene->sphereBuffer.push_back(*s);
        }
        for (auto& t : asset.triangles) {
            this->scene->triangleBuffer.push_back(*t);
        }
        for (auto& p : asset.planes) {
            this->scene->planeBuffer.push_back(*p);
        }
        for (auto& m : asset.meshes) {
            this->scene->meshBuffer.push_back(*m);
        }

        // 复制各类光源数据
        for (auto& p : asset.pointLights) {
            this->scene->pointLightBuffer.push_back(*p);
        }
        for (auto& a : asset.areaLights) {
            this->scene->areaLightBuffer.push_back(*a);
        }
        for (auto& d : asset.directionalLights) {
            this->scene->directionalLightBuffer.push_back(*d);
        }
        for (auto& s : asset.spotLights) {
            this->scene->spotLightBuffer.push_back(*s);
        }
    }

    // 构建相机
    // 将相机设置复制到场景中
    void SceneBuilder::buildCamera() {
        this->scene->camera = this->camera;
    }

    // 构建环境光
    // 设置环境光类型和参数
    void SceneBuilder::buildAmbient() {
        this->scene->ambient.constant = ambientSettings.ambient;          // 环境光常量
        this->scene->ambient.environmentMap = ambientSettings.mapTexture;  // 环境贴图
        if (ambientSettings.type == AmbientSettings::Type::CONSTANT) {
            this->scene->ambient.type = Ambient::Type::CONSTANT;          // 常量环境光
        }
        else {
            this->scene->ambient.type = Ambient::Type::ENVIROMENT_MAP;    // 环境贴图光照
        }
    }

    // 构建完整场景
    // 返回构建好的场景对象
    SharedScene SceneBuilder::build() {
        this->scene = make_shared<Scene>();  // 创建新场景
        this->buildRenderOption();           // 构建渲染选项
        this->buildCamera();                 // 构建相机
        this->buildBuffer();                 // 构建场景缓冲区
        this->buildAmbient();               // 构建环境光
        if (success)
            return this->scene;              // 构建成功返回场景
        else 
            return nullptr;                  // 构建失败返回空
    }
}