#pragma once
#ifndef __NR_MANAGER_HPP__
#define __NR_MANAGER_HPP__

// 管理器总控头文件
// 定义了整个渲染系统的主管理器类

#include "AssetManager.hpp"
#include "RenderSettingsManager.hpp"
#include "ComponentManager.hpp"

namespace NRenderer
{
    // 主管理器类
    // 负责协调资产、渲染设置和组件的管理
    class Manager
    {
    public:
        AssetManager assetManager;              // 资产管理器实例
        RenderSettingsManager renderSettingsManager;  // 渲染设置管理器实例
        ComponentManager componentManager;       // 组件管理器实例

        // 构造函数
        Manager();
        // 默认析构函数
        ~Manager() = default;
    };
} // namespace NRenderer


#endif