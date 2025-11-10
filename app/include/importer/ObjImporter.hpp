#pragma once
#ifndef __OBJ_IMPORTER_HPP__
#define __OBJ_IMPORTER_HPP__

// OBJ文件导入器头文件
// 定义了导入OBJ格式3D模型文件的功能

#include "Importer.hpp"
#include <map>

#include <unordered_map>

namespace NRenderer
{
    using namespace std;

    // OBJ文件导入器类
    // 负责解析和导入OBJ格式的3D模型文件
    class ObjImporter: public Importer
    {
    private:
        // 解析MTL材质文件
        // asset: 目标资产对象
        // path: MTL文件路径
        // file: MTL文件流
        // mtlMap: 材质名称到索引的映射
        // 返回: 解析是否成功
        bool parseMtl(Asset& asset, const string& path, ifstream& file, unordered_map<string, size_t>& mtlMap);
    public:
        // 导入OBJ文件
        // asset: 目标资产对象
        // path: OBJ文件路径
        // 返回: 导入是否成功
        virtual bool import(Asset& asset, const string& path) override;
    };
}

#endif