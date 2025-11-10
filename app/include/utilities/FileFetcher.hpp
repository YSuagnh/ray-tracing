#pragma once
#ifndef __NR_FILE_FETCHER_HPP__
#define __NR_FILE_FETCHER_HPP__

// 文件获取器头文件
// 提供了文件选择和获取的功能

#include <optional>
#include <string>

namespace NRenderer
{
    using namespace std;

    // 文件获取器类
    // 用于打开文件选择对话框并获取用户选择的文件路径
    class FileFetcher
    {
    public:
        // 默认构造函数
        FileFetcher() = default;
        // 默认析构函数
        ~FileFetcher() = default;

        // 获取文件路径
        // filter: 文件过滤器字符串（如 "*.obj"）
        // 返回用户选择的文件路径，如果用户取消则返回空
        optional<string> fetch(const char* filter) const;
    };
} // namespace NRenderer

#endif