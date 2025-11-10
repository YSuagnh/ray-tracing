// 基础宏定义和通用工具
// 包含跨平台DLL导入导出宏定义和智能指针相关的辅助宏
#pragma once
#ifndef __NR_MACROS_HPP__
#define __NR_MACROS_HPP__

// Windows平台下的DLL导入导出定义
#ifdef _WIN32
    #define DLL_IMPORT __declspec(dllimport)
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_IMPORT
    #define DLL_EXPORT
#endif

#include <memory>

// 创建类型的智能指针别名
// 使用示例：SHARE(MyClass) 将创建 SharedMyClass 类型，等价于 std::shared_ptr<MyClass>
#define SHARE(__T__) using Shared##__T__ = std::shared_ptr<__T__>

#endif