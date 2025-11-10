#pragma once
#ifndef __NR_GL_IMAGE_HPP__
#define __NR_GL_IMAGE_HPP__

// OpenGL图像工具类头文件
// 提供了OpenGL纹理的加载和管理功能

#include "geometry/vec.hpp"
#include "glad/glad.h"

namespace NRenderer
{
    // OpenGL纹理ID类型
    using GlImageId = unsigned int;

    // OpenGL图像工具类
    // 提供了加载和删除OpenGL纹理的静态方法
    class GlImage
    {
    private:
    public:
        // 加载RGBA格式图像到OpenGL纹理
        // pixels: RGBA格式的像素数据
        // size: 图像尺寸（宽度和高度）
        // 返回生成的纹理ID
        static GlImageId loadImage(const RGBA* pixels, const Vec2& size) {
            GlImageId id  = 0u;
            glGenTextures(1, &id);
            glBindTexture(GL_TEXTURE_2D, id);
            // 设置纹理过滤参数
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // 上传纹理数据
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_FLOAT, pixels);
            glBindTexture(GL_TEXTURE_2D, 0);
            return id;
        }

        // 删除OpenGL纹理
        // id: 要删除的纹理ID
        static void deleteImage(GlImageId id) {
            glDeleteTextures(1, &id);
        }

        // 加载RGB格式图像到OpenGL纹理
        // pixels: RGB格式的像素数据
        // size: 图像尺寸（宽度和高度）
        // 返回生成的纹理ID
        static GlImageId loadImage(const RGB* pixels, const Vec2& size) {
            GlImageId id  = 0u;
            glGenTextures(1, &id);
            glBindTexture(GL_TEXTURE_2D, id);
            // 设置纹理过滤参数
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // 上传纹理数据
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGB, GL_FLOAT, pixels);
            glBindTexture(GL_TEXTURE_2D, 0);
            return id;
        }
    };    
} // namespace NRenderer

#endif