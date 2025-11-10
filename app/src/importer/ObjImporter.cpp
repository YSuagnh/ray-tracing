// OBJ文件导入器实现文件
// 负责导入OBJ格式的3D模型文件及其相关的MTL材质文件

#include <fstream>
#include <sstream>

#include "utilities/File.hpp"
#include "utilities/ImageLoader.hpp"
#include "utilities/GlImage.hpp"

#include "importer/ObjImporter.hpp"

namespace NRenderer
{
    // 加载纹理文件并创建纹理项
    // filePath: 纹理文件所在目录路径
    // fileName: 纹理文件名
    // 返回值: 创建的纹理项
    inline
    TextureItem loadTexture(const string& filePath, const string& fileName) {
        ImageLoader imageLoader{};
        auto pImage = imageLoader.load(filePath+fileName, 4);  // 加载RGBA格式图像
        auto& image = *pImage;
        if (pImage == nullptr) 
            return {};
        auto ti = TextureItem{};
        ti.texture = SharedTexture{new Texture{}};
        ti.name = fileName;
        ti.texture->width = image.width;
        ti.texture->height = image.height;
        ti.texture->rgba = new RGBA[image.width*image.height];
        // 复制图像数据到纹理
        for (int i = 0; i < image.width * image.height; i++) {
            ti.texture->rgba[i].r = image.data[i*4];
            ti.texture->rgba[i].g = image.data[i*4 + 1];
            ti.texture->rgba[i].b = image.data[i*4 + 2];
            ti.texture->rgba[i].a = image.data[i*4 + 3];
        }
        ti.glId = GlImage::loadImage(ti.texture->rgba, {image.width, image.height});
        return ti;
    }

    // 解析MTL材质文件
    // asset: 资产管理器
    // path: MTL文件所在目录路径
    // file: MTL文件输入流
    // mtlMap: 材质名称到索引的映射
    // 返回值: 解析是否成功
    bool ObjImporter::parseMtl(Asset& asset, const string& path, ifstream& file, unordered_map<string, size_t>& mtlMap) {
        MaterialItem* currMaterialItem = nullptr;  // 当前正在处理的材质项

        using PW = Property::Wrapper;

        string currLine;
        string token;

        stringstream ss{};

        bool successFlag = true;

        while(getline(file, currLine)) {
            ss<<currLine;
            ss>>token;

            transform(token.begin(), token.end(), token.begin(), ::tolower);

            // 解析不同类型的材质属性
            if(token == "newmtl") {  // 新材质定义
                ss>>token;
                mtlMap.insert({token, asset.materialItems.size() - 1});

                asset.materialItems.push_back({});
                currMaterialItem = &asset.materialItems[asset.materialItems.size() - 1];
                currMaterialItem->material = make_shared<Material>();
                currMaterialItem->material->type = 1;
                currMaterialItem->name = token;
            }
            else if (token == "ka") {  // 环境光颜色（暂不支持）
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
            }
            else if (token == "kd") {  // 漫反射颜色
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
                auto diffuseColor = Vec3{f1, f2, f3};
                currMaterialItem->material->registerProperty("diffuseColor", PW::RGBType{diffuseColor});
            }
            else if (token == "ks") {  // 镜面反射颜色
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
                auto specularColor = Vec3{f1, f2, f3};
                currMaterialItem->material->registerProperty("specularColor", PW::RGBType{specularColor});
            }
            else if (token == "ns") {  // 镜面反射指数
                float f;
                ss>>f;
                auto specularEx = f;
                currMaterialItem->material->registerProperty("specularEx", PW::FloatType{ f });
            }
            else if (token == "d") {  // 透明度（暂不支持）
                float f;
                ss>>f;
            }
            else if (token == "ke") {  // 发射颜色（暂不支持）
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
            }
            else if (token == "tr") {  // 透明度（暂不支持）
                float f;
                ss>>f;
            }
            else if (token == "tf") {  // 透射滤波颜色（暂不支持）
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
            }
            else if (token == "ni") {  // 折射率（暂不支持）
                float f;
                ss>>f;
            }
            else if (token == "map_ka" || token == "map_d" || token == "disp" || token == "decal") {
                // 这些纹理映射暂不支持
            }
            else if (token == "map_kd") {  // 漫反射纹理贴图
                ss>>token;
                auto ti = loadTexture(path, token);
                if (ti.texture != nullptr) {
                    Handle t{ (unsigned int)asset.textureItems.size() };
                    Property p{ "diffuseMap", PW::TextureIdType{t} };
                    currMaterialItem->material->registerProperty(p);
                    asset.textureItems.push_back(ti);
                }
            }
            else if (token == "map_ks") {  // 镜面反射纹理贴图
                ss>>token;
                auto ti = loadTexture(path, token);
                if (ti.texture != nullptr) {
                    Handle t{ (unsigned int)asset.textureItems.size() };
                    Property p{ "specularMap", PW::TextureIdType{t} };
                    currMaterialItem->material->registerProperty(p);
                    asset.textureItems.push_back(ti);
                }
            }
            else if (token == "map_bump" || token == "bump") {  // 凹凸贴图
                ss>>token;
                auto ti = loadTexture(path, token);
                if (ti.texture != nullptr) {
                    Handle t{ (unsigned int)asset.textureItems.size() };
                    Property p{ "bumpMap", PW::TextureIdType{t} };
                    currMaterialItem->material->registerProperty(p);
                    asset.textureItems.push_back(ti);
                }
            }
            ss.clear();
            ss.str("");
        }
        return successFlag;
    }

    // 导入OBJ文件
    // asset: 资产管理器
    // path: OBJ文件路径
    // 返回值: 导入是否成功
    bool ObjImporter::import(Asset& asset, const string& path) {
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

        // 创建新的模型项
        auto modelName = File::getFileName(path);
        ModelItem modelItem;
        modelItem.name = modelName;
        modelItem.model = make_shared<Model>();

        string currLine;
        bool hasMtl = false;
        
        string token;
        stringstream ss{};

        unordered_map<string, size_t> mtlMap;  // 材质名称到索引的映射
        
        // 顶点数据缓存
        vector<Vec3> positions;  // 顶点位置
        vector<Vec3> normals;    // 顶点法线
        vector<Vec2> uvs;        // 纹理坐标

        // 顶点索引映射
        using _mp = shared_ptr<unordered_map<Index, Index>>;
        _mp pMap{nullptr};  // 位置索引映射
        _mp tMap{nullptr};  // 纹理坐标索引映射
        _mp nMap{nullptr};  // 法线索引映射

        NodeItem* currNodePtr = nullptr;  // 当前处理的节点

        Handle currUsedMtl{};  // 当前使用的材质

        // 逐行解析OBJ文件
        while (getline(file, currLine)) {
            ss<<currLine;
            ss>>token;

            if (token == "mtllib") {  // 材质库文件
                string mtlFileName;
                ss>>mtlFileName;
                auto npos = path.find_last_of("\\/");
                string mtlPath;
                mtlPath = path.substr(0, npos + 1);
                auto mtlFilePath = mtlPath + mtlFileName;
                ifstream mtlFile(mtlFilePath);
                if (!mtlFile.is_open()) {
                    successFlag = false;
                    lastErrorInfo = "Cannot file .mtl file";
                    break;
                }
                successFlag = parseMtl(asset, mtlPath, mtlFile, mtlMap);
                if (successFlag) {
                    hasMtl = true;
                }
                else {
                    break;
                }
            }
            else if (token == "usemtl") {  // 使用材质
                string mtlName;
                ss>>mtlName;
                auto mtlItr = mtlMap.find(mtlName);
                if (mtlItr != mtlMap.end()) {
                    currUsedMtl.setIndex(mtlItr->second);
                    asset.meshes[currNodePtr->node->entity]->material = currUsedMtl;
                }
                else {
                    successFlag = false;
                    lastErrorInfo = "Cannot find material: " + mtlName;
                    break;
                }
            }
            else if (token == "v") {  // 顶点位置
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
                positions.push_back({ f1, f2, f3 });
            }
            else if (token == "vt") {  // 顶点纹理坐标
                float f1, f2;
                ss>>f1>>f2;
                uvs.push_back({ f1, f2 });
            }
            else if (token == "vn") {  // 顶点法线
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
                normals.push_back({ f1, f2, f3 });
            }
            else if (token == "o" || token == "g") {  // 对象或组
                string name;
                if (ss.eof()) {
                    name = "undefined";
                }
                else {
                    ss>>name;
                }
                // 创建新节点
                modelItem.model->nodes.push_back(asset.nodeItems.size());
                asset.nodeItems.push_back({});
                pMap = make_shared<unordered_map<Index, Index> >();
                tMap = make_shared<unordered_map<Index, Index> >();
                nMap = make_shared<unordered_map<Index, Index> >();
                currNodePtr = &(*(asset.nodeItems.end() - 1));
                currNodePtr->name = name;
                currNodePtr->node = make_shared<Node>();
                currNodePtr->node->type = Node::Type::MESH;
                currNodePtr->node->model = asset.modelItems.size();
                currNodePtr->node->entity = asset.meshes.size();
                asset.meshes.push_back(make_shared<Mesh>());
            }
            else if (token == "f") {  // 面（三角形）
                // 如果还没有创建节点，创建一个默认节点
                if (currNodePtr == nullptr) {
                    modelItem.model->nodes.push_back(asset.nodeItems.size());
                    asset.nodeItems.push_back({});
                    pMap = make_shared<unordered_map<Index, Index> >();
                    tMap = make_shared<unordered_map<Index, Index> >();
                    nMap = make_shared<unordered_map<Index, Index> >();
                    currNodePtr = &(*(asset.nodeItems.end() - 1));
                    currNodePtr->name = "Undefined";
                    currNodePtr->node = make_shared<Node>();
                    currNodePtr->node->type = Node::Type::MESH;
                    currNodePtr->node->model = asset.modelItems.size();
                    currNodePtr->node->entity = asset.meshes.size();
                    asset.meshes.push_back(make_shared<Mesh>());
                }

                long v{0}, t{0}, n{0};  // 顶点、纹理、法线索引
                char c = '\0';

                exception e(".obj file must be triangulated.");

                Index vpi[3];  // 顶点位置索引
                Index vti[3];  // 顶点纹理坐标索引
                Index vni[3];  // 顶点法线索引

                // 解析三角形的三个顶点
                for (int i=0; i<3; i++) {
                    vpi[i] = -1;
                    vti[i] = -1;
                    vni[i] = -1;

                    if(ss.eof()) throw e;

                    string vertexToken;
                    ss>>vertexToken;
                    stringstream vertexStream{vertexToken};
                    auto begin = vertexToken.find_first_of('/');
                    auto end = vertexToken.find_last_of('/');

                    // 解析不同格式的顶点数据
                    // f v v v
                    if (begin == vertexToken.npos) {
                        vertexStream>>v;
                        t = -1;
                        n = -1;
                    }
                    // f v/t v/t v/t
                    else if (begin == end) {
                        vertexStream>>v>>c>>t;
                        n = -1;
                    }
                    // f v//n v//n v//n
                    else if (begin + 1 == end) {
                        vertexStream>>v>>c>>c>>n;
                        t = -1;
                    }
                    // f v/t/n v/t/n v/t/n
                    else {
                        vertexStream>>v>>c>>t>>c>>n;
                    }

                    // 处理顶点位置
                    if (v != -1) {
                        auto finded = pMap->find(v);
                        if (finded != pMap->end()) {
                            vpi[i] = finded->second;
                        }
                        else {
                            auto idx = asset.meshes[currNodePtr->node->entity]->positions.size();
                            vpi[i] = idx;
                            pMap->insert({v, idx});
                            asset.meshes[currNodePtr->node->entity]->positions.push_back(positions[v - 1]);
                        }
                        asset.meshes[currNodePtr->node->entity]->positionIndices.push_back(vpi[i]);
                    }

                    // 处理纹理坐标
                    if (t != -1) {
                        auto finded = tMap->find(t);
                        if (finded != tMap->end()) {
                            vti[i] = finded->second;
                        }
                        else {
                            auto idx = asset.meshes[currNodePtr->node->entity]->uvs.size();
                            vti[i] = idx;
                            tMap->insert({t, idx});
                            asset.meshes[currNodePtr->node->entity]->uvs.push_back(uvs[t - 1]);
                        }
                        asset.meshes[currNodePtr->node->entity]->uvIndices.push_back(vti[i]);
                    }

                    // 处理法线
                    if (n != -1) {
                        auto finded = nMap->find(n);
                        if (finded != nMap->end()) {
                            vni[i] = finded->second;
                        }
                        else {
                            auto idx = asset.meshes[currNodePtr->node->entity]->normals.size();
                            vni[i] = idx;
                            nMap->insert({n, idx});
                            asset.meshes[currNodePtr->node->entity]->normals.push_back(normals[n - 1]);
                        }
                        asset.meshes[currNodePtr->node->entity]->normalIndices.push_back(vni[i]);
                    }
                }

                // 检查是否为三角形
                if (!ss.eof()) {
                    successFlag = false;
                    lastErrorInfo = "Only Triangulated mesh is supported!";
                    break;
                }
            }
            ss.clear();
            ss.str("");
        }

        // 添加模型到资产
        asset.modelItems.push_back(modelItem);

        // 为新添加的节点生成OpenGL预览缓冲区
        if (successFlag) {
            for (auto i = beginNode; i < asset.nodeItems.size(); i++) {
                asset.genPreviewGlBuffersPerNode(asset.nodeItems[i]);
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