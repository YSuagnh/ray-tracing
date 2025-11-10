#include "ui/views/ComponentProgressView.hpp"

// 组件进度视图实现文件
// 实现了渲染组件执行进度的显示功能

namespace NRenderer
{
    // 空实现的窗口绘制函数
    void ComponentProgressView::drawBeginWindow() {}
    void ComponentProgressView::drawEndWindow() {}
    void ComponentProgressView::drawSetup() {}
    void ComponentProgressView::drawFinish() {}

    // 设置进度窗口的位置和大小
    void ComponentProgressView::drawPosAndSize() {
        // 计算屏幕中心位置
        ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
        // 设置窗口位置为屏幕中心
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        // 设置窗口初始大小
        ImGui::SetNextWindowSize({size.x, size.y}, ImGuiCond_FirstUseEver);
    }

    // 绘制进度窗口内容
    void ComponentProgressView::draw() {
        auto& componentManager = manager.componentManager;
        // 如果组件处于空闲状态，不显示进度窗口
        if (componentManager.getState() == ComponentManager::State::IDLING) return;

        auto activeComponentInfo = componentManager.getActiveComponentInfo();  // 获取当前活动组件信息
        ImGui::OpenPopup("Executing Component");  // 打开弹出窗口

        // 开始绘制弹出窗口
        if (ImGui::BeginPopupModal("Executing Component", nullptr, 0)) {
            if (componentManager.getState() == ComponentManager::State::RUNNING) {
                // 组件正在运行
                uiContext.state = UIContext::State::HOVER_COMPONENT_PROGRESS;
                ImGui::TextUnformatted(("正在执行: " + activeComponentInfo.id).c_str());
            }
            else if (componentManager.getState() == ComponentManager::State::READY) {
                // 组件准备就绪
                uiContext.state = UIContext::State::HOVER_COMPONENT_PROGRESS;
                ImGui::TextUnformatted(("准备执行: " + activeComponentInfo.id).c_str());
            }
            else if (componentManager.getState() == ComponentManager::State::FINISH) {
                // 组件执行完成
                auto execTime = componentManager.getLastExecTime();  // 获取执行时间
                componentManager.finish();  // 完成组件执行

                // 生成并显示完成信息
                string logInfo{};
                logInfo = activeComponentInfo.id + "执行完毕. Time: " + to_string(execTime.count()) + "s";
                getServer().logger.success(logInfo);

                uiContext.state = UIContext::State::NORMAL;  // 恢复正常状态
                ImGui::CloseCurrentPopup();  // 关闭弹出窗口
            }
            ImGui::EndPopup();
        }
    }
}