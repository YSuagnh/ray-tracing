#pragma once
#ifndef __NR_ASSET_VIEW_HPP__
#define __NR_ASSET_VIEW_HPP__

// 资产视图头文件
// 定义了场景资产的管理和编辑界面

#include "View.hpp"

namespace NRenderer
{
    // 资产视图类
    // 负责显示和编辑场景中的模型、材质、纹理和光源
    class AssetView: public View
    {
    private:
        // 重写基类虚函数
        virtual void drawBeginWindow() override;  // 开始绘制窗口
        virtual void draw() override;            // 绘制内容

        void menu();                            // 绘制菜单
        void modelTab();                        // 模型标签页
        void materialTab();                     // 材质标签页
        void textureTab();                      // 纹理标签页
        void lightTab();                        // 光源标签页

        int currMtlIndex;                       // 当前选中的材质索引
        int currLightIndex;                     // 当前选中的光源索引

        Property tempProp;                      // 临时属性
        int tempPropTypeIndex;                  // 临时属性类型索引
        bool isPropKeyValid;                    // 属性键是否有效
        void tempPropEditor();                  // 临时属性编辑器

        void resetTempProp();                   // 重置临时属性

        MaterialItem tempMaterialItem;          // 临时材质项

        // 材质属性编辑器
        // spMaterial: 要编辑的材质指针
        void materialPropEditor(SharedMaterial spMaterial);

    public:
        // 构造函数
        // position: 视图位置
        // size: 视图大小
        // uiContext: UI上下文
        // manager: 管理器
        AssetView(const Vec2& position, const Vec2& size, UIContext& uiContext, Manager& manager);
    };
} // namespace NRenderer


#endif