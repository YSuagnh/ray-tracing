#pragma once
#ifndef __NR_IMAGE_HPP__
#define __NR_IMAGE_HPP__

// 图像数据结构头文件
// 定义了存储和管理图像数据的基本结构

namespace NRenderer
{
    using namespace std;

    // 图像数据结构
    // 存储图像的尺寸、通道数和像素数据
	struct Image {
		int height;     // 图像高度
		int width;      // 图像宽度
		int channel;    // 图像通道数
		float* data;    // 像素数据（范围：0.0 - 1.0）

        // 默认构造函数
        // 创建一个空图像
		Image()
			: height(0)
			, width(0)
			, channel(0)
			, data(nullptr)
		{}

        // 拷贝构造函数
        // image: 源图像
        // 创建一个源图像的深拷贝
        Image(const Image& image) {
            this->height = image.height;
            this->width = image.width;
            this->channel = image.channel;
            int size = image.height*image.width*image.channel;
            this->data = new float[size];
            for (int i = 0; i < size; i++) {
                this->data[i] = image.data[i];
            }
        }

        // 移动构造函数
        // image: 源图像
        // 从源图像获取资源所有权
        Image(Image&& image) noexcept {
            this->height = image.height;
            this->width = image.width;
            this->channel = image.channel;
            int size = image.height*image.width*image.channel;
            this->data = image.data;
            image.data = nullptr;  // 源图像放弃所有权
        }

        // 析构函数
        // 释放像素数据内存
        ~Image() {
            delete[] data;
        }
	};
}

#endif