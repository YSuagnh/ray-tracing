#include "ui/views/View.hpp"

// 视图基类实现文件
// 实现了所有UI视图的基本功能

namespace NRenderer
{
    // 显示视图
    // 处理视图的显示状态并调用绘制函数
    void View::display() {
        drawSetup();  // 绘制前的设置

        // 处理禁用状态
        if (disable) {
            windowFlag |= ImGuiWindowFlags_NoBringToFrontOnFocus;  // 禁止窗口前置
            windowFlag |= ImGuiWindowFlags_NoInputs;               // 禁止输入
            windowFlag |= ImGuiWindowFlags_NoFocusOnAppearing;     // 禁止获取焦点
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.7f);  // 设置半透明
        }

        // 处理大小调整
        if (!resizable) {
            windowFlag |= ImGuiWindowFlags_NoResize;  // 禁止调整大小
        }

        // 绘制可见的视图
        if (this->visible) {
            this->drawPosAndSize();     // 设置位置和大小
            this->drawBeginWindow();    // 开始绘制窗口
            this->draw();               // 绘制内容
            this->drawEndWindow();      // 结束绘制窗口
        }

        drawFinish();  // 绘制后的清理
    }

    // 绘制前的设置
    void View::drawSetup() {
    }

    // 结束绘制窗口
    void View::drawEndWindow() {
        ImGui::End();
    }

    // 设置视图的位置和大小
    void View::drawPosAndSize() {
        ImGui::SetNextWindowPos({position.x, position.y}, ImGuiCond_FirstUseEver);    // 设置初始位置
        ImGui::SetNextWindowSize({size.x, size.y}, ImGuiCond_FirstUseEver);          // 设置初始大小
    }

    // 绘制后的清理工作
    void View::drawFinish() {
        windowFlag = 0;  // 重置窗口标志
        if (disable) {
            ImGui::PopStyleVar();  // 恢复样式
        }
    }

    // 创建帮助提示
    // desc: 帮助文本
    void View::makeHelper(const char* desc)
    {
        ImGui::TextDisabled("(?)");  // 显示帮助图标
        if (ImGui::IsItemHovered())  // 当鼠标悬停时
        {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);  // 设置文本换行
            ImGui::TextUnformatted(desc);                          // 显示帮助文本
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }

    // 构造函数
    // 初始化视图的基本属性
    View::View(const Vec2& position, const Vec2& size, UIContext& uiContext, Manager& manager)
        : position              (position)     // 视图位置
        , size                  (size)         // 视图大小
        , uiContext             (uiContext)    // UI上下文
        , manager               (manager)      // 管理器
        , disable               (false)        // 默认启用
        , visible               (true)         // 默认可见
        , resizable             (true)         // 默认可调整大小
    {}
} // namespace NRenderer
