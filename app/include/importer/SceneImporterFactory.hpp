#pragma once
#ifndef __NR_SCENE_IMPORTER_FACTORY_HPP__
#define __NR_SCENE_IMPORTER_FACTORY_HPP__

// 场景导入器工厂头文件
// 定义了创建和管理不同类型场景导入器的工厂类

#include <string>
#include <unordered_map>

#include "Importer.hpp"
#include "ScnImporter.hpp"
#include "ObjImporter.hpp"

namespace NRenderer
{
    using namespace std;

    // 场景导入器工厂类
    // 负责创建和管理不同文件格式的场景导入器
    class SceneImporterFactory
    {
    private:
        unordered_map<string, SharedImporter> importerMap;  // 文件扩展名到导入器的映射
    public:
        // 获取工厂单例
        // 返回: 工厂实例的引用
        static SceneImporterFactory& instance() {
            static SceneImporterFactory f;
            return f;
        }

        // 构造函数
        // 初始化支持的导入器类型
        SceneImporterFactory() {
            importerMap["scn"] = make_shared<ScnImporter>();  // 添加SCN格式导入器
            importerMap["obj"] = make_shared<ObjImporter>();  // 添加OBJ格式导入器
        }

        // 获取指定文件格式的导入器
        // ext: 文件扩展名
        // 返回: 对应的导入器指针，如果不支持该格式则返回nullptr
        SharedImporter importer(const string& ext) {
            auto it = importerMap.find(ext);
            if (it!=importerMap.end()) {
                return it->second;
            }
            return nullptr;
        }
    };
}

#endif