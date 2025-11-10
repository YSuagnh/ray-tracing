// 渲染组件基类定义
// 所有渲染器组件的基类，提供渲染功能的接口
#pragma once
#ifndef __NR_RENDER_COMPONENT_HPP__
#define __NR_RENDER_COMPONENT_HPP__

#include "Component.hpp"
#include "scene/Scene.hpp"

#include <functional>

namespace NRenderer
{
    // 渲染组件基类
    // 定义了渲染器的基本接口，包括渲染方法和执行流程
    class DLL_EXPORT RenderComponent: public Component
    {
    private:
        // 纯虚函数，具体的渲染实现
        // 子类必须实现此方法来定义具体的渲染逻辑
        virtual void render(SharedScene spScene) = 0;
    public:
        // 执行渲染流程
        // onStart: 渲染开始时的回调
        // onFinish: 渲染结束时的回调
        // spScene: 要渲染的场景
        void exec(function<void()> onStart, function<void()> onFinish, SharedScene spScene);
    };
}

// 渲染器注册宏
// 用于注册新的渲染器组件
#define REGISTER_RENDERER(__NAME__, __DESCRIPTION__, __CLASS__)          REGISTER_COMPONENT(Render, __NAME__, __DESCRIPTION__, __CLASS__)

// 无描述渲染器注册宏
// 简化版的渲染器注册宏，不需要提供描述信息
#define REGISTER_RENDERER_NO_DESCRIPTION(__NAME__, __CLASS__)            REGISTER_RENDERER(__NAME__, "" ,__CLASS__)
#endif