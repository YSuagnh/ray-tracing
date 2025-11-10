#pragma once
#ifndef __NR_LOG_VIEW_HPP__
#define __NR_LOG_VIEW_HPP__

// 日志视图头文件
// 定义了显示系统日志的界面

#include "View.hpp"

namespace NRenderer
{
    // 日志视图类
    // 负责显示系统运行时的日志信息
    class LogView : public View
    {
    protected:
        // 重写基类虚函数
        virtual void draw() override;             // 绘制日志内容
        virtual void drawBeginWindow() override;  // 开始绘制窗口
    public:
        using View::View;                         // 使用基类的构造函数
    };
} // namespace NRenderer


#endif