#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "utilities/ImageLoader.hpp"

// 图像加载器实现文件
// 使用stb_image库加载图像文件

namespace NRenderer
{
	// 加载图像文件
	// file: 图像文件路径
	// channel: 期望的通道数（3=RGB或4=RGBA）
	// 返回值: 加载的图像数据，加载失败返回nullptr
	Image* ImageLoader::load(const string& file, int channel) {
		// 检查通道数是否有效
		if (channel != 3 && channel != 4) return nullptr;

		// 创建图像对象
		Image* image = new Image();

		// 使用stb_image加载图像
		auto data = stbi_load(file.c_str(), &(image->width), &(image->height), &(image->channel), channel);
		image->channel = channel;

		// 将图像数据转换为浮点数格式（0-1范围）
		image->data = new float[image->width * image->height * image->channel];
		for (unsigned long i = 0; i < image->height * image->width * channel; i++) {
			image->data[i] = (float)(data[i]) / 255.f;  // 将8位整数转换为浮点数
		}

		// 释放stb_image分配的内存
		stbi_image_free(data);

		return image;
	}
}