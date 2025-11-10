#pragma once
#ifndef __NR_IMPORTER_HPP__
#define __NR_IMPORTER_HPP__

// 导入器基类头文件
// 定义了资产导入器的基本接口

#include <string>
#include "asset/Asset.hpp"

namespace NRenderer
{
    using namespace std;

    // 导入器基类
    // 为不同类型的资产导入器提供统一的接口
    class Importer
    {
    protected:
        string lastErrorInfo;   // 最后一次错误信息
    public:
        // 导入资产
        // asset: 目标资产对象
        // path: 资产文件路径
        // 返回: 导入是否成功
        virtual bool import(Asset& asset, const string& path) = 0;

        // 获取错误信息
        // 返回: 最后一次错误信息
        inline
        string getErrorInfo() const {
            return lastErrorInfo;
        }

        // 构造函数
        Importer()
            : lastErrorInfo         ()
        {}

        // 虚析构函数
        virtual ~Importer() = default;
    };
    SHARE(Importer);
}

#endif