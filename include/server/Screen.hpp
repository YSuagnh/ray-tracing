// 屏幕类定义
// 管理渲染输出的像素缓冲区，提供线程安全的访问机制
#pragma once
#ifndef __NR_SCREEN_HPP__
#define __NR_SCREEN_HPP__

#include "geometry/vec.hpp"
#include "common/macros.hpp"
#include <mutex>

namespace NRenderer
{
    // 屏幕类
    // 管理渲染结果的像素数据，支持线程安全的更新和访问
    class DLL_EXPORT Screen
    {
    private:
        RGBA* pixels;           // 像素数据缓冲区
        unsigned int width;     // 屏幕宽度
        unsigned int height;    // 屏幕高度
        mutable bool updated;   // 更新标志
        mutable mutex mtx;      // 互斥锁，保证线程安全

    public:
        // 默认构造函数
        Screen();
        // 禁用拷贝构造函数
        Screen(const Screen&) = delete;
        // 析构函数，释放像素缓冲区
        ~Screen();

        // 设置屏幕数据
        // 更新像素缓冲区的内容和尺寸
        void set(RGBA* pixels, int width, int height);

        // 获取屏幕宽度
        unsigned int getWidth() const;
        // 获取屏幕高度
        unsigned int getHeight() const;
        // 获取像素数据
        const RGBA* getPixels() const;
        // 释放像素缓冲区
        void release();
        // 检查是否有更新
        bool isUpdated() const;
    };  
} // namespace NRenderer


#endif