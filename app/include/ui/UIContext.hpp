#pragma once
#ifndef __NR_UI_CONTEXT_HPP__
#define __NR_UI_CONTEXT_HPP__

// UI上下文头文件
// 定义了UI的状态和预览模式

#include "geometry/vec.hpp"

namespace NRenderer
{
    // UI上下文结构体
    // 管理UI的状态和预览信息
    struct UIContext
    {
        // UI状态枚举
        enum class State
        {
            HOVER_COMPONENT_PROGRESS,  // 悬停在组件进度条上
            NORMAL                     // 正常状态
        };
        State state;                   // 当前UI状态

        // 预览模式枚举
        enum class PreviewMode {
            PREVIEW_NONE,              // 无预览
            PREVIEW_MODEL,             // 预览模型
            PREVIEW_NODE,              // 预览节点
            PREVIEW_LIGHT              // 预览光源
        };

        PreviewMode previewMode;       // 当前预览模式

        Index previewModel;            // 预览的模型索引
        Index previewNode;             // 预览的节点索引
        Index previewLight;            // 预览的光源索引

        // 构造函数
        // 初始化UI上下文为默认状态
        UIContext()
            : state (State::NORMAL)                    // 初始为正常状态
            , previewNode(-1)                          // 无预览节点
            , previewLight(-1)                         // 无预览光源
            , previewModel(-1)                         // 无预览模型
            , previewMode(PreviewMode::PREVIEW_NONE)   // 无预览模式
        {}

        // 获取当前状态
        // 返回: 当前UI状态
        inline
        State getState() const {
            return state;
        }
    };
} // namespace NRenderer


#endif