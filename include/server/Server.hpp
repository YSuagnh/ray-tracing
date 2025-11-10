// 服务器类定义
// 提供全局服务和资源管理，包括日志、屏幕和组件系统
#pragma once
#ifndef __NR_SERVER_HPP__
#define __NR_SERVERHPP__

#include "Screen.hpp"
#include "Logger.hpp"
#include "component/ComponentFactory.hpp"

namespace NRenderer
{
    // 服务器结构体
    // 集中管理渲染系统的核心服务
    struct DLL_EXPORT Server
    {
        Logger logger = {};             // 日志系统
        Screen screen = {};             // 屏幕管理
        ComponentFactory componentFactory = {};  // 组件工厂
        Server() = default;
    };
} // namespace NRenderer

// 全局服务器访问函数
// 提供对服务器实例的全局访问点
extern "C" {
    DLL_EXPORT
    NRenderer::Server& getServer();
}

#endif