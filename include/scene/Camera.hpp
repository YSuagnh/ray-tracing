// 相机类定义
// 定义了场景中的相机属性和行为，支持景深效果
#pragma once
#ifndef __NR_CAMERA_HPP__
#define __NR_CAMERA_HPP__

#include <memory>
#include "geometry/vec.hpp"

namespace NRenderer
{
    using namespace std;
    
    // 相机结构体
    // 定义了相机的位置、朝向、视场角等属性，支持景深效果的模拟
    struct Camera
    {
        Vec3 position;       // 相机位置
        Vec3 up;            // 相机上方向
        Vec3 lookAt;        // 相机注视点
        float fov;          // 视场角（度）
        float aperture;     // 光圈大小（用于景深效果）
        float focusDistance;// 焦距（用于景深效果）
        float aspect;       // 宽高比

        // 默认构造函数
        // 创建一个位于(0,0,-3)处，朝向原点的相机
        Camera() noexcept
            : position      (0.f, 0.f, -3.f)
            , up            (0.f, 1.f, 0.f)
            , lookAt        (0.f, 0.f, 0.f)
            , fov           (40)
            , aperture      (0.f)
            , focusDistance (0.1f)
            , aspect        (1.f)
        {}

        // 带参数的构造函数
        Camera(
            const Vec3& position,    // 相机位置
            const Vec3& up,          // 相机上方向
            const Vec3& lookAt,      // 相机注视点
            float fov,               // 视场角（度）
            float aperture,          // 光圈大小
            float focusDistance,     // 焦距
            float aspect            // 宽高比
        )
            : position      (position)
            , up            (up)
            , lookAt        (lookAt)
            , fov           (fov)
            , aperture      (aperture)
            , focusDistance (focusDistance)
            , aspect        (aspect)
        {}
    };
    using SharedCamera = shared_ptr<Camera>;    // 相机的智能指针类型
}

#endif
