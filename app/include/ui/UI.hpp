#pragma once
#ifndef __NR_UI_HPP__
#define __NR_UI_HPP__

// 用户界面头文件
// 定义了渲染器的主界面类

#include "glad/glad.h"
#include "glfw3.h"
#include "glfw3native.h"

#include "UIContext.hpp"
#include "manager/Manager.hpp"

#include <string>
#include <vector>

#include "views/View.hpp"

namespace NRenderer
{
    using namespace std;

    // 用户界面类
    // 负责管理和渲染主窗口及其所有视图
    class UI
    {
    private:
        unsigned int height;      // 窗口高度
        unsigned int width;       // 窗口宽度
        string title;            // 窗口标题
        GLFWwindow* window;      // GLFW窗口句柄

        Manager manager;         // 管理器实例
        UIContext uiContext;     // UI上下文实例

        vector<View*> views;     // 视图列表
    public:
        // 默认构造函数
        UI();
        // 带参数构造函数
        // width: 窗口宽度
        // height: 窗口高度
        // title: 窗口标题
        UI(unsigned int width, unsigned int height, const string& title);
        // 禁用拷贝构造函数
        UI(const UI&) = delete;
        // 析构函数
        ~UI();

        // 初始化UI
        void init();
        // 运行UI主循环
        void run();
    };
} // namespace NRenderer


#endif