#pragma once
#ifndef __NR_COMPONENT_PROGRESS_VIEW_HPP__
#define __NR_COMPONENT_PROGRESS_VIEW_HPP__

// 组件进度视图头文件
// 定义了显示渲染组件进度的界面

#include "View.hpp"

namespace NRenderer
{
   // 组件进度视图类
   // 负责显示渲染组件的执行进度
   class ComponentProgressView: public View
   {
    private:
        // 重写基类虚函数
        virtual void drawSetup();         // 绘制前的设置
        virtual void drawFinish();        // 绘制后的清理
        virtual void drawPosAndSize();    // 绘制位置和大小
        virtual void drawBeginWindow();   // 开始绘制窗口
        virtual void drawEndWindow();     // 结束绘制窗口
        virtual void draw();              // 绘制内容
    public:
        using View::View;                 // 使用基类的构造函数
   };
}

#endif