#pragma once
#ifndef __NR_ITEM_HPP__
#define __NR_ITEM_HPP__

// 资产项基类头文件
// 定义了场景中所有资产项的基本结构

#include <string>

namespace NRenderer
{
    using namespace std;
    // OpenGL对象ID类型
    using GlId = unsigned int;

    // 资产项基类
    // 所有场景资产（模型、材质、光源等）的基类
    struct Item
    {
        string name;    // 资产项名称

        // 默认构造函数
        // 创建一个未命名的资产项
        Item()
            : name      ("undefined")
        {}
    };

    // OpenGL绘制数据结构
    // 存储用于OpenGL绘制的顶点数据
    struct GlDrawData
    {
        vector<Vec3> positions;  // 顶点位置数组
    };
    SHARE(GlDrawData);  // 声明智能指针类型
}

#endif