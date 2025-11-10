#pragma once
#ifndef __NR_SCENE_VIEW_HPP__
#define __NR_SCENE_VIEW_HPP__

// 场景视图头文件
// 定义了场景设置和配置的界面

#include "View.hpp"

namespace NRenderer
{
    // 场景视图类
    // 负责显示和编辑场景的相机、渲染、环境光和组件设置
    class SceneView: public View
    {
    private:
        // 重写基类虚函数
        virtual void drawBeginWindow() override;  // 开始绘制窗口
        virtual void draw() override;            // 绘制内容

        void cameraSetting();                    // 相机设置界面
        void renderSetting();                    // 渲染设置界面
        void ambientSetting();                   // 环境光设置界面
        void componentSetting();                 // 组件设置界面

        int currComponentSelected;               // 当前选中的组件索引
    public:
        // 构造函数
        // position: 视图位置
        // size: 视图大小
        // uiContext: UI上下文
        // manager: 管理器
        SceneView(const Vec2& position, const Vec2& size, UIContext& uiContext, Manager& manager);
    };
} // namespace NRenderer


#endif