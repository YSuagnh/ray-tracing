#include "ui/views/LogView.hpp"

// 日志视图实现文件
// 实现了系统日志的显示功能

#include "server/Server.hpp"

#undef ERROR

namespace NRenderer
{
    // 开始绘制日志窗口
    void LogView::drawBeginWindow() {
        ImGui::Begin("Log", nullptr, windowFlag);  // 创建标题为"Log"的窗口
    }

    // 绘制日志内容
    void LogView::draw() {
        // 添加清除按钮
        bool clear = ImGui::Button("Clear");
        if (clear) {
            getServer().logger.clear();  // 清除所有日志
        }
            
        // 创建日志内容滚动区域
        ImGui::BeginChild("Log Content", {}, true);
        {
            // 定义不同类型日志的颜色
            const ImVec4 green{ 0, 1, 0, 1 };   // 成功信息为绿色
            const ImVec4 red{1, 0, 0, 1};       // 错误信息为红色
            const ImVec4 yellow{1, 1, 0, 1};    // 警告信息为黄色

            // 获取并显示所有日志消息
            auto msgs = getServer().logger.get();
            for (int i=0; i<msgs.nums; i++) {
                auto& msg = msgs.msgs[i];
                switch (msg.type)
                {
                case Logger::LogType::SUCCESS:  // 成功信息
                    ImGui::PushStyleColor(ImGuiCol_Text, green);
                    ImGui::TextWrapped(msg.message.c_str());
                    ImGui::PopStyleColor();
                    break;
                case Logger::LogType::WARNING:  // 警告信息
                    ImGui::PushStyleColor(ImGuiCol_Text, yellow);
                    ImGui::TextWrapped(msg.message.c_str());
                    ImGui::PopStyleColor();
                    break;
                case Logger::LogType::ERROR:    // 错误信息
                    ImGui::PushStyleColor(ImGuiCol_Text, red);
                    ImGui::TextWrapped(msg.message.c_str());
                    ImGui::PopStyleColor();
                    break;
                default:                        // 普通信息
                    ImGui::TextUnformatted(msg.message.c_str());
                    break;
                }
            } 
        }
        ImGui::EndChild();  // 结束滚动区域
    }
    
} // namespace NRenderer
