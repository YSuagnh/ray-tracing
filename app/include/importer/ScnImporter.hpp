#pragma once
#ifndef __NR_SCN_IMPORTER_HPP__
#define __NR_SCN_IMPORTER_HPP__

// SCN场景文件导入器头文件
// 定义了导入SCN格式场景文件的功能

#include "Importer.hpp"
#include <map>

namespace NRenderer
{
    using namespace std;

    // SCN场景文件导入器类
    // 负责解析和导入SCN格式的场景文件
    class ScnImporter: public Importer
    {
    private:
        // 解析材质部分
        // asset: 目标资产对象
        // file: 文件流
        // mtlMap: 材质名称到索引的映射
        // 返回: 解析是否成功
        bool parseMtl(Asset& asset, ifstream& file, map<string, size_t>& mtlMap);

        // 解析模型部分
        // asset: 目标资产对象
        // file: 文件流
        // mtlMap: 材质名称到索引的映射
        // 返回: 解析是否成功
        bool parseMdl(Asset& asset, ifstream& file, map<string, size_t>& mtlMap);

        // 解析光源部分
        // asset: 目标资产对象
        // file: 文件流
        // 返回: 解析是否成功
        bool parseLgt(Asset& asset, ifstream& file);
    public:
        // 导入SCN文件
        // asset: 目标资产对象
        // path: SCN文件路径
        // 返回: 导入是否成功
        virtual bool import(Asset& asset, const string& path) override;
    };
}

#endif