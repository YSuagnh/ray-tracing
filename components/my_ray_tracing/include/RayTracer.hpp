#pragma once
#ifndef __RAY_TRACER_HPP__
#define __RAY_TRACER_HPP__

#include "scene/Scene.hpp"
#include "Ray.hpp"
#include "Camera.hpp"
#include "intersections/HitRecord.hpp"

#include "shaders/ShaderCreator.hpp"

#include <tuple>

namespace RayTracer
{
	using namespace NRenderer;
	using namespace std;

	class RayTracerRenderer
	{
	private:
		SharedScene spScene;        // 场景共享指针
		Scene& scene;               // 场景引用
		unsigned int width;         // 图像宽度
		unsigned int height;        // 图像高度
		unsigned int depth;         // 最大递归深度
		unsigned int samples;       // 每像素采样数
		using SCam = RayTracer::Camera;
		SCam camera;                // 相机对象
		vector<SharedShader> shaderPrograms;  // 着色器程序列表

	public:
		/**
		 * 构造函数
		 * @param spScene 场景共享指针
		 */
		RayTracerRenderer(SharedScene spScene)
			: spScene(spScene)
			, scene(*spScene)
			, camera(spScene->camera)
		{
			width = scene.renderOption.width;
			height = scene.renderOption.height;
			depth = scene.renderOption.depth;
			samples = scene.renderOption.samplesPerPixel;
		}
		~RayTracerRenderer() = default;
		using RenderResult = tuple<RGBA*, unsigned int, unsigned int>;  // 渲染结果类型

		RenderResult render();
		void release(const RenderResult& r);
	private:
		/**
		 * 渲染任务（多线程）
		 * @param pixels 像素缓冲区
		 * @param width 图像宽度
		 * @param height 图像高度
		 * @param off 起始偏移
		 * @param step 步长
		 */
		void renderTask(RGBA* pixels, int width, int height, int off, int step);

		/**
		 * Gamma校正
		 * @param rgb 原始颜色
		 * @return 校正后的颜色
		 */
		RGB gamma(const RGB& rgb);

		/**
		 * 路径追踪主函数
		 * @param ray 光线
		 * @param currDepth 当前递归深度
		 * @return 光线颜色
		 */
		RGB trace(const Ray& ray, int currDepth);
		
		/**
		 * 查找最近相交的物体
		 * @param r 光线
		 * @return 相交记录
		 */
		HitRecord closestHitObject(const Ray& r);

		/**
		 * 查找最近相交的光源
		 * @param r 光线
		 * @return 相交距离和发光强度
		 */
		tuple<float, Vec3> closestHitLight(const Ray& r);
	};
}
#endif 