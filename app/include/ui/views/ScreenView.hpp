#pragma once
#ifndef __NR_SCREEN_VIEW_HPP__
#define __NR_SCREEN_VIEW_HPP__

// 屏幕视图头文件
// 定义了渲染结果和预览的显示视图

#include "View.hpp"
#include "utilities/GlImage.hpp"
#include "utilities/GlShader.hpp"

namespace NRenderer
{
    // 屏幕视图类
    // 负责显示渲染结果和场景预览
    class ScreenView: public View
    {
    private:
        // 重写基类虚函数
        virtual void drawBeginWindow() override;  // 开始绘制窗口
        virtual void draw() override;            // 绘制内容

        void drawGlPreview();                    // 绘制OpenGL预览
        void preview();                          // 预览模式
        void result();                           // 结果模式
        float getShrinkNum();                    // 获取缩放比例

        GlShader nodeShader;                     // 节点着色器
        GlShader lightShader;                    // 光源着色器

        GlId glFBO;                              // 帧缓冲对象
        GlId glRBO;                              // 渲染缓冲对象

        unsigned int prePreviewHeight;            // 预览前高度
        unsigned int prePreviewWidth;             // 预览前宽度

        unsigned int previewHeight;               // 预览高度
        unsigned int previewWidth;                // 预览宽度

        // 坐标系类型枚举
        enum class CoordinateType {
            LEFT_HANDED,                          // 左手坐标系
            RIGHT_HANDED                          // 右手坐标系
        };
        CoordinateType previewCoordinateType;     // 预览坐标系类型

        bool isPreviewSizeChange();               // 检查预览尺寸是否改变
        void updatePreviewSize(unsigned int x, unsigned int y);  // 更新预览尺寸
        void updatePrePreviewSize();              // 更新预览前尺寸

        void genFB();                             // 生成帧缓冲
        void previewNode(const NodeItem& n);      // 预览节点
        void previewLight(const LightItem& l);    // 预览光源

        void align(const Vec2& size);             // 对齐视图大小

        // 视图类型枚举
        enum class ViewType
        {
            PREVIEW,                              // 预览模式
            RESULT                                // 结果模式
        };

        ViewType viewType;                        // 当前视图类型

        int shrinkLevel;                          // 缩放级别

        GlImageId renderResult;                   // 渲染结果纹理
        GlImageId previewResult;                  // 预览结果纹理
    public:
        // 构造函数
        // position: 视图位置
        // size: 视图大小
        // uiContext: UI上下文
        // manager: 管理器
        ScreenView(const Vec2& position, const Vec2& size, UIContext& uiContext, Manager& manager);
    };
} // namespace NRenderer


#endif