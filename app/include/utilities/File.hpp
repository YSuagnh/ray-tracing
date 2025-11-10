#pragma once
#ifndef __NR_FILE_HPP__
#define __NR_FILE_HPP__

// 文件工具类头文件
// 提供了文件路径处理的基本功能

#include "scene/Model.hpp"
#include "scene/Light.hpp"

namespace NRenderer
{
    using namespace std;

    // 文件工具类
    // 提供了获取文件扩展名和文件名的静态方法
    struct File
    {
        // 获取文件扩展名
        // filePath: 文件路径
        // 返回文件的扩展名（不包含点号）
        static string getFileExtension(const string& filePath) {
			auto pos = filePath.find_last_of('.');
			return filePath.substr(pos+1, filePath.size()-pos-1);
		}

        // 获取文件名
        // filePath: 文件路径
        // 返回文件名（包含扩展名，不包含路径）
        static string getFileName(const string& filePath) {
            auto pos = filePath.find_last_of("\\/");
            string fileName;
            if(pos != string::npos)
                fileName = filePath.substr(pos + 1, filePath.size() - pos - 1);
            else
                fileName = filePath;
            return fileName;
        }
    };

    // 文件智能指针类型定义
    using SharedFile = shared_ptr<File>;
} // namespace NRenderer

#endif