#pragma once
#ifndef __NR_IMAGE_LOADER_HPP__
#define __NR_IMAGE_LOADER_HPP__

// 图像加载器头文件
// 提供了从文件加载图像的功能

#include <string>

#include "geometry/vec.hpp"
#include "Image.hpp"

namespace NRenderer {
	using namespace std;

    // 图像加载器类
    // 负责从文件系统加载图像数据
	class ImageLoader
	{
	public:
        // 默认构造函数
		ImageLoader() = default;
        // 默认析构函数
		~ImageLoader() = default;

        // 加载图像文件
        // file: 图像文件路径
        // channel: 期望的图像通道数（默认为4，即RGBA）
        // 返回加载的图像数据，如果加载失败返回nullptr
		Image* load(const string& file, int channel = 4);
	};
}

#endif