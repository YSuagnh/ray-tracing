// 场景文件导入器实现文件
// 负责导入自定义SCN格式的场景文件，包括材质、模型和光源的定义

#include "importer/ScnImporter.hpp"

#include <fstream>
#include <sstream>

#include <map>

namespace NRenderer
{
    // 解析材质定义部分
    // asset: 资产管理器
    // file: 场景文件输入流
    // mtlMap: 材质名称到索引的映射
    // 返回值: 解析是否成功
    bool ScnImporter::parseMtl(Asset& asset, ifstream& file, map<string, size_t>& mtlMap) {
        string currline;
        
        stringstream ss{};
        SharedMaterial spMaterial{nullptr};  // 当前处理的材质

        bool successFlag = true;
        
        while(getline(file, currline)) {
            ss.str("");
            ss.clear();
            string token;
            ss<<currline;
            ss>>token;
            if (successFlag == false) {
                break;
            }
            if (token=="") continue;
            else if (token[0] == '#') continue;  // 跳过注释行
            else if (token == "Material") {  // 材质定义开始
                // 获取材质名称
                string name;
                ss>>name;
                MaterialItem item;
                spMaterial = SharedMaterial{new Material()};
                item.material = spMaterial;
                item.name = name;
                auto index = asset.materialItems.size();
                asset.materialItems.push_back(item);
                if (mtlMap.find(name) == mtlMap.end()) {
                    mtlMap[name] = index;
                }
                else {
                    lastErrorInfo = "Duplicated Material Key:" + name;
                    successFlag = false;
                    break;
                }
                // 获取材质类型
                unsigned int type = 0;
                if (!ss.eof())
                    ss>>type;
                spMaterial->type = type;
            }
            else if (token == "Prop") {  // 材质属性定义
                using PT = Property::Type;
                using PW = Property::Wrapper;
                string key, type;
                ss>>key>>type;
                // 根据属性类型解析不同格式的数据
                if (type == "Int") {  // 整数类型
                    int v;
                    ss>>v;
                    spMaterial->registerProperty(key, PW::IntType{v});
                }
                else if (type == "Float") {  // 浮点数类型
                    float v;
                    ss>>v;
                    spMaterial->registerProperty(key, PW::FloatType{v});
                }
                else if (type == "Vec3") {  // 三维向量类型
                    float v1, v2, v3;
                    ss>>v1>>v2>>v3;
                    spMaterial->registerProperty(key, PW::Vec3Type{Vec3{v1, v2, v3}});
                }
                else if (type == "Vec4") {  // 四维向量类型
                    float v1, v2, v3, v4;
                    ss>>v1>>v2>>v3>>v4;
                    spMaterial->registerProperty(key, PW::Vec4Type{Vec4{v1, v2, v3, v4}});
                }
                else if (type == "RGB") {  // RGB颜色类型
                    float v1, v2, v3;
                    ss>>v1>>v2>>v3;
                    spMaterial->registerProperty(key, PW::RGBType{Vec3{v1, v2, v3}});
                }
                else if (type == "RGBA") {  // RGBA颜色类型
                    float v1, v2, v3, v4;
                    ss>>v1>>v2>>v3>>v4;
                    spMaterial->registerProperty(key, PW::RGBAType{Vec4{v1, v2, v3, v4}});
                }
            }
            else if (token == "End") {  // 材质定义结束
                break;
            }
            else {
                successFlag = false;
                break;
            }
        }

        return successFlag;
    }

    // 解析模型定义部分
    // asset: 资产管理器
    // file: 场景文件输入流
    // mtlMap: 材质名称到索引的映射
    // 返回值: 解析是否成功
    bool ScnImporter::parseMdl(Asset& asset, ifstream& file, map<string, size_t>& mtlMap) {
        string currline;
        
        stringstream ss{};
        
        ModelItem modelItem;

        bool successFlag = true;

        int currNodeType = 0;  // 当前节点类型
        
        while(getline(file, currline)) {
            ss.str("");
            ss.clear();
            string token;
            ss<<currline;
            ss>>token;
            if (token=="") continue;
            else if (token[0] == '#') continue;  // 跳过注释行
            else if (token == "Model") {  // 模型定义开始
                modelItem = ModelItem{};
                ss>>modelItem.name;
                modelItem.model = make_shared<Model>();
                asset.modelItems.push_back(modelItem);
            }
            else if (token == "Translation") {  // 模型平移
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
                (asset.modelItems.end() - 1)->model->translation = {f1, f2, f3};
            }
            else if (token == "Scale") {  // 模型缩放
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
                (asset.modelItems.end() - 1)->model->scale = {f1, f2, f3};
            }
            else if (token == "Sphere") {  // 球体节点
                NodeItem ni{};
                ss>>ni.name;
                ni.node = SharedNode{new Node{}};
                ni.node->type = Node::Type::SPHERE;
                currNodeType = 0;
                string mtlName;
                ss>>mtlName;
                auto mtl = mtlMap.find(mtlName);
                if (mtl == mtlMap.end()) {
                    lastErrorInfo = string("Invalid material name.");
                    successFlag = false;
                    break;
                }
                asset.modelItems[asset.modelItems.size() - 1].model->nodes.push_back(asset.nodeItems.size());
                ni.node->entity = asset.spheres.size();
                ni.node->model = asset.modelItems.size() - 1;
                asset.nodeItems.push_back(ni);
                asset.spheres.push_back(SharedSphere{new Sphere()});
                asset.spheres[asset.spheres.size() - 1]->material = mtl->second;
            }
            else if (token == "Triangle") {  // 三角形节点
                NodeItem ni{};
                ss>>ni.name;
                ni.node = SharedNode{new Node{}};
                ni.node->type = Node::Type::TRIANGLE;
                currNodeType = 1;
                string mtlName;
                ss>>mtlName;
                auto mtl = mtlMap.find(mtlName);
                if (mtl == mtlMap.end()) {
                    lastErrorInfo = string("Invalid material name.");
                    successFlag = false;
                    break;
                }
                ni.node->entity = asset.triangles.size();
                ni.node->model = asset.modelItems.size() - 1;
                asset.modelItems[asset.modelItems.size() - 1].model->nodes.push_back(asset.nodeItems.size());
                asset.nodeItems.push_back(ni);
                asset.triangles.push_back(SharedTriangle{new Triangle()});
                asset.triangles[asset.triangles.size() - 1]->material = mtl->second;
            }
            else if (token == "Plane") {  // 平面节点
                NodeItem ni{};
                ss>>ni.name;
                ni.node = SharedNode{new Node{}};
                ni.node->type = Node::Type::PLANE;
                currNodeType = 2;
                string mtlName;
                ss>>mtlName;
                auto mtl = mtlMap.find(mtlName);
                if (mtl == mtlMap.end()) {
                    lastErrorInfo = string("Invalid material name.");
                    successFlag = false;
                    break;
                }
                asset.modelItems[asset.modelItems.size() - 1].model->nodes.push_back(asset.nodeItems.size());
                ni.node->entity = asset.planes.size();
                ni.node->model = asset.modelItems.size() - 1;
                asset.nodeItems.push_back(ni);
                asset.planes.push_back(SharedPlane{new Plane()});
                asset.planes[asset.planes.size() - 1]->material = mtl->second;
            }
            else if (token == "R") {  // 球体半径
                float f;
                ss>>f;
                auto it = asset.spheres.end()-1;
                (*it)->radius = f;
            }
            else if (token == "N") {  // 法线方向
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
                Vec3 n = {f1, f2, f3};
                if (currNodeType == 0) {  // 球体法线
                    auto it = asset.spheres.end() - 1;
                    (*it)->direction = n;
                }
                else if (currNodeType == 1) {  // 三角形法线
                    auto it = asset.triangles.end() - 1;
                    (*it)->normal = n;
                } 
                else if (currNodeType == 2) {  // 平面法线
                    auto it = asset.planes.end() - 1;
                    (*it)->normal = n;
                }
            }
            else if (token == "V1") {  // 三角形第一个顶点
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
                Vec3 v = {f1, f2, f3};
                auto it = asset.triangles.end() - 1;
                (*it)->v[0] = v;
            }
            else if (token == "V2") {  // 三角形第二个顶点
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
                Vec3 v = {f1, f2, f3};
                auto it = asset.triangles.end() - 1;
                (*it)->v[1] = v;
            }
            else if (token == "V3") {  // 三角形第三个顶点
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
                Vec3 v = {f1, f2, f3};
                auto it = asset.triangles.end() - 1;
                (*it)->v[2] = v;
            }
            else if (token == "P") {  // 位置
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
                Vec3 p = {f1, f2, f3};
                if (currNodeType == 0) {  // 球体位置
                    auto it = asset.spheres.end() - 1;
                    (*it)->position = p;
                }
                else if (currNodeType == 2) {  // 平面位置
                    auto it = asset.planes.end() - 1;
                    (*it)->position = p;
                }
            }
            else if (token == "U") {  // 平面U方向向量
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
                Vec3 u = {f1, f2, f3};
                auto it = asset.planes.end() - 1;
                (*it)->u = u;
            }
            else if (token == "V") {  // 平面V方向向量
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
                Vec3 v = {f1, f2, f3};
                auto it = asset.planes.end() - 1;
                (*it)->v = v;
            }
            else if (token == "End") {  // 模型定义结束
                break;
            }
            else {
                successFlag = false;
                lastErrorInfo = "Syntax Error!";
                break;
            }
        }
        return successFlag;
    }

    // 解析光源定义部分
    // asset: 资产管理器
    // file: 场景文件输入流
    // 返回值: 解析是否成功
    bool ScnImporter::parseLgt(Asset& asset, ifstream& file) {
        bool successFlag = true;

        string currline;
        stringstream ss{};

        int currLightType{0};  // 当前光源类型
        
        while(getline(file, currline)) {
            ss.str("");
            ss.clear();
            string token;
            ss<<currline;
            ss>>token;
            if (token == "") continue;

            else if (token[0] == '#') continue;  // 跳过注释行

            else if (token == "Point") {  // 点光源
                LightItem li{};
                string name;
                ss>>name;
                currLightType = 0;
                li.name = name;
                li.light = SharedLight{new Light()};
                li.light->type = Light::Type::POINT;
                li.light->entity = asset.pointLights.size();
                asset.lightItems.push_back(li);
                asset.pointLights.push_back(make_shared<PointLight>());
            }
            else if (token == "Spot") {  // 聚光灯
                LightItem li{};
                string name;
                ss>>name;
                currLightType = 3;
                li.name = name;
                li.light = SharedLight{new Light()};
                li.light->type = Light::Type::SPOT;
                li.light->entity = asset.spotLights.size();
                asset.lightItems.push_back(li);
                asset.spotLights.push_back(make_shared<SpotLight>());
            }
            else if (token == "Directional") {  // 平行光
                LightItem li{};
                string name;
                ss>>name;
                currLightType = 2;
                li.name = name;
                li.light = SharedLight{new Light()};
                li.light->type = Light::Type::DIRECTIONAL;
                li.light->entity = asset.directionalLights.size();
                asset.lightItems.push_back(li);
                asset.directionalLights.push_back(make_shared<DirectionalLight>());
            }
            else if (token == "Area") {  // 面光源
                LightItem li{};
                string name;
                ss>>name;
                currLightType = 1;
                li.name = name;
                li.light = SharedLight{new Light()};
                li.light->type = Light::Type::AREA;
                li.light->entity = asset.areaLights.size();
                asset.lightItems.push_back(li);
                asset.areaLights.push_back(make_shared<AreaLight>());
            }
            else if (token == "IRV") {  // 光源强度/辐射度/辐照度
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
                Vec3 v{f1, f2, f3};
                if (currLightType == 0) {  // 点光源强度
                    auto& p = *(*(asset.pointLights.end() - 1));
                    p.intensity = v;
                }
                else if (currLightType == 1) {  // 面光源辐射度
                    auto& a = *(*(asset.areaLights.end() - 1));
                    a.radiance = v;
                }
                else if (currLightType == 2) {  // 平行光辐照度
                    auto& d = *(*(asset.directionalLights.end() - 1));
                    d.irradiance = v;
                }
                else if (currLightType == 3) {  // 聚光灯强度
                    auto& s = *(*(asset.spotLights.end() - 1));
                    s.intensity = v;
                }
            }
            else if (token == "P") {  // 光源位置
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
                Vec3 pos{f1, f2, f3};
                if (currLightType == 0) {  // 点光源位置
                    auto& p = *(*(asset.pointLights.end() - 1));
                    p.position = pos;
                }
                else if (currLightType == 1) {  // 面光源位置
                    auto& a = *(*(asset.areaLights.end() - 1));
                    a.position = pos;
                }
                else if (currLightType == 3) {  // 聚光灯位置
                    auto& s = *(*(asset.spotLights.end() - 1));
                    s.position = pos;
                }
            }
            else if (token == "D") {  // 光源方向
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
                Vec3 dir{f1, f2, f3};
                if (currLightType == 2) {  // 平行光方向
                    auto& d = *(*(asset.directionalLights.end() - 1));
                    d.direction = dir;
                }
                else if (currLightType == 3) {  // 聚光灯方向
                    auto& s = *(*(asset.directionalLights.end() - 1));
                    s.direction = dir;
                }
            }
            else if (token == "HotSpot") {  // 聚光灯热点角度
                float r;
                ss>>r;
                auto& s = *(*(asset.spotLights.end() - 1));
                s.hotSpot = r;
            }
            else if (token == "Fallout") {  // 聚光灯衰减角度
                float r;
                ss>>r;
                auto& s = *(*(asset.spotLights.end() - 1));
                s.fallout = r;
            }
            else if (token == "U") {  // 面光源U方向向量
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
                Vec3 u{ f1, f2, f3 };
                auto& a = *(*(asset.areaLights.end() - 1));
                a.u = u;
            }
            else if (token == "V") {  // 面光源V方向向量
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
                Vec3 v{f1, f2, f3};
                auto& a = *(*(asset.areaLights.end() - 1));
                a.v = v;
            }
            else if (token == "End") {  // 光源定义结束
                break;
            }
            else {
                successFlag = false;
                lastErrorInfo = "Syntax Error!";
                break;
            }
        }
        return successFlag;
    }

    // 导入场景文件
    // asset: 资产管理器
    // path: 场景文件路径
    // 返回值: 导入是否成功
    bool ScnImporter::import(Asset& asset, const string& path) {
        ifstream file(path);
        if (!file.is_open()) {
            lastErrorInfo = "File does not exist!";
            return false;
        }

        // 记录导入前的资产状态
        size_t beginModel = asset.modelItems.size();
        size_t beginNode = asset.nodeItems.size();
        size_t beginMaterial = asset.materialItems.size();
        size_t beginTexture = asset.textureItems.size();

        size_t beginSph = asset.spheres.size();
        size_t beginTri = asset.triangles.size();
        size_t beginPln = asset.planes.size();
        size_t beginMsh = asset.meshes.size();

        size_t beginLight = asset.lightItems.size();
        size_t beginPnt = asset.pointLights.size();
        size_t beginArea = asset.areaLights.size();
        size_t beginDir = asset.directionalLights.size();
        size_t beginSpt = asset.spotLights.size();

        bool successFlag = true;

        string currline;
        stringstream ss{};

        map<string, size_t> mtlMap;  // 材质名称到索引的映射
        
        // 解析场景文件
        while(getline(file, currline)) {
            ss.str("");
            ss.clear();
            string token;
            ss<<currline;
            ss>>token;
            if (successFlag == false) {
                break;
            }
            if (token == "") continue;

            else if (token[0] == '#') continue;  // 跳过注释行

            else if (token == "Begin") {  // 开始定义块
                ss>>token;
                if (token == "Material") {  // 材质定义块
                    successFlag = parseMtl(asset, file, mtlMap);
                }
                else if (token == "Model") {  // 模型定义块
                    successFlag = parseMdl(asset, file, mtlMap);
                }
                else if (token == "Light") {  // 光源定义块
                    successFlag = parseLgt(asset, file);
                }
                else {
                    successFlag = false;
                }
            }
            else {
                successFlag = false;
                lastErrorInfo = "Syntax Error!";
            }

            ss.clear();
            ss.str("");
        }

        // 为新添加的节点和光源生成OpenGL预览缓冲区
        if (successFlag) {
            for (auto i = beginNode; i < asset.nodeItems.size(); i++) {
                asset.genPreviewGlBuffersPerNode(asset.nodeItems[i]);
            }

            for (auto i = beginLight; i < asset.lightItems.size(); i++) {
                asset.genPreviewGlBuffersPerLight(asset.lightItems[i]);
            }
        }

        // 如果导入失败，回滚所有更改
        if (!successFlag) {
            asset.modelItems        .erase(asset.modelItems         .begin() + beginModel,      asset.modelItems.end());
            asset.nodeItems         .erase(asset.nodeItems          .begin() + beginNode,       asset.nodeItems.end());
            asset.materialItems     .erase(asset.materialItems      .begin() + beginMaterial,   asset.materialItems.end());
            asset.textureItems      .erase(asset.textureItems       .begin() + beginTexture,    asset.textureItems.end());
            
            asset.spheres           .erase(asset.spheres            .begin() + beginSph,        asset.spheres.end());
            asset.triangles         .erase(asset.triangles          .begin() + beginTri,        asset.triangles.end());
            asset.planes            .erase(asset.planes             .begin() + beginPln,        asset.planes.end());
            asset.meshes            .erase(asset.meshes             .begin() + beginMsh,        asset.meshes.end());
            
            asset.lightItems        .erase(asset.lightItems         .begin() + beginLight,      asset.lightItems.end());
            asset.pointLights       .erase(asset.pointLights        .begin() + beginPnt,        asset.pointLights.end());
            asset.areaLights        .erase(asset.areaLights         .begin() + beginArea,       asset.areaLights.end());
            asset.directionalLights .erase(asset.directionalLights  .begin() + beginDir,        asset.directionalLights.end());
            asset.spotLights        .erase(asset.spotLights         .begin() + beginSpt,        asset.spotLights.end());
        }

        return successFlag;
    }
}