#pragma once
#ifndef __NR_VIEW_HPP__
#define __NR_VIEW_HPP__

// 视图基类头文件
// 定义了所有UI视图的基本接口和功能

#include "ui/UIContext.hpp"
#include "manager/Manager.hpp"
#include "geometry/vec.hpp"
#include "imgui.h"

namespace NRenderer
{
    // 视图基类
    // 为所有UI视图提供基本功能和接口
    class View
    {
    protected:
        Vec2 position;           // 视图位置
        Vec2 size;              // 视图大小
        UIContext& uiContext;    // UI上下文引用
        Manager& manager;        // 管理器引用

        bool disable;           // 是否禁用
        bool visible;           // 是否可见
        bool resizable;         // 是否可调整大小

        ImGuiWindowFlags windowFlag = 0;  // ImGui窗口标志

        // 绘制前的设置
        virtual void drawSetup();
        // 绘制后的清理
        virtual void drawFinish();
        // 绘制位置和大小
        virtual void drawPosAndSize();
        // 开始绘制窗口（纯虚函数）
        virtual void drawBeginWindow() = 0;
        // 结束绘制窗口
        virtual void drawEndWindow();
        // 绘制内容（纯虚函数）
        virtual void draw() = 0;

        // 创建帮助提示
        // desc: 帮助文本
        void makeHelper(const char* desc);
    public:
        // 构造函数
        // position: 视图位置
        // size: 视图大小
        // uiContext: UI上下文
        // manager: 管理器
        View(const Vec2& position, const Vec2& size, UIContext& uiContext, Manager& manager);
        // 禁用拷贝构造函数
        View(const View&) = delete;
        
        // 显示视图
        void display();
        
        // 设置视图可见性
        // flag: 是否可见
        inline
        void setVisibility(bool flag) {
            visible = flag;
        }

        // 设置视图是否禁用
        // flag: 是否禁用
        inline
        void setDisable(bool flag) {
            disable = flag;
        }

        // 设置视图是否可调整大小
        // flag: 是否可调整大小
        inline
        void setResizable(bool flag) {
            resizable = flag;
        }
    };
} // namespace NRenderer


#endif