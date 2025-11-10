#pragma once
#ifndef __CAMERA_HPP__
#define __CAMERA_HPP__

// 相机类头文件
// 实现了光线投射渲染中的相机模型，支持景深效果

#include "scene/Camera.hpp"
#include "geometry/vec.hpp"
#include "Ray.hpp"

namespace RayCast
{
    using namespace std;
    using namespace NRenderer;

    // 相机类
    // 负责生成从相机位置发射的光线
    class Camera
    {
    private:
        const NRenderer::Camera& camera;  // 场景相机参数
        float lenRadius;                  // 镜头半径（用于景深效果）
        Vec3 u, v, w;                    // 相机坐标系基向量
        Vec3 vertical;                    // 视平面垂直方向向量
        Vec3 horizontal;                  // 视平面水平方向向量
        Vec3 lowerLeft;                  // 视平面左下角位置
        Vec3 position;                   // 相机位置

    public:
        // 构造函数
        // camera: 场景相机参数
        Camera(const NRenderer::Camera& camera)
            : camera                (camera)
        {
            position = camera.position;
            // 计算镜头半径（用于景深）
            lenRadius = camera.aperture / 2.f;
            // 计算视场角
            auto vfov = camera.fov;
            vfov = clamp(vfov, 160.f, 20.f);
            auto theta = glm::radians(vfov);
            auto halfHeight = tan(theta/2.f);
            auto halfWidth = camera.aspect*halfHeight;

            // 构建相机坐标系
            Vec3 up = camera.up;
            w = glm::normalize(camera.position - camera.lookAt);  // 观察方向的反方向
            u = glm::normalize(glm::cross(up, w));               // 右方向
            v = glm::cross(w, u);                                // 上方向

            // 计算焦距
            auto focusDis = camera.focusDistance;

            // 计算视平面的位置和大小
            lowerLeft = position - halfWidth*focusDis*u
                - halfHeight*focusDis*v
                - focusDis*w;
            horizontal = 2*halfWidth*focusDis*u;
            vertical = 2*halfHeight*focusDis*v;
        }

        // 从相机发射光线
        // s: 归一化的水平坐标 [0,1]
        // t: 归一化的垂直坐标 [0,1]
        // 返回生成的光线
        Ray shoot(float s, float t) const {
            return Ray{
                position,
                glm::normalize(
                    lowerLeft + s*horizontal + t*vertical - position
                )
            };
        }
    };
}

#endif