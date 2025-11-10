#pragma once
#ifndef __CAMERA_HPP__
#define __CAMERA_HPP__

#include "scene/Camera.hpp"
#include "geometry/vec.hpp"

#include "samplers/SamplerInstance.hpp"

#include "Ray.hpp"

namespace SimplePathTracer
{
    using namespace std;
    using namespace NRenderer;
    
    /**
     * 简单路径追踪相机类
     * 负责从相机位置发射光线，支持景深效果
     */
    class Camera
    {
    private:
        const NRenderer::Camera& camera;  // 原始相机对象引用
        float lenRadius;                  // 镜头半径，用于景深效果
        Vec3 u, v, w;                     // 相机的三个正交基向量
        Vec3 vertical;                    // 垂直方向向量
        Vec3 horizontal;                  // 水平方向向量
        Vec3 lowerLeft;                   // 成像平面左下角位置
        Vec3 position;                    // 相机位置
    public:
        /**
         * 构造函数，初始化相机参数
         * @param camera 原始相机对象
         */
        Camera(const NRenderer::Camera& camera)
            : camera                (camera)
        {
            position = camera.position;
            lenRadius = camera.aperture / 2.f;  // 计算镜头半径
            
            // 计算视场角，限制在20-160度之间
            auto vfov = camera.fov;
            vfov = clamp(vfov, 160.f, 20.f);
            auto theta = glm::radians(vfov);
            auto halfHeight = tan(theta/2.f);
            auto halfWidth = camera.aspect*halfHeight;
            
            // 建立相机坐标系
            Vec3 up = camera.up;
            w = glm::normalize(camera.position - camera.lookAt);  // 相机朝向
            u = glm::normalize(glm::cross(up, w));                // 右方向
            v = glm::cross(w, u);                                 // 上方向

            // 计算成像平面参数
            auto focusDis = camera.focusDistance;
            lowerLeft = position - halfWidth*focusDis*u
                - halfHeight*focusDis*v
                - focusDis*w;
            horizontal = 2*halfWidth*focusDis*u;
            vertical = 2*halfHeight*focusDis*v;
        }

        /**
         * 从摄像机发射光线
         * @param s 水平方向参数 [0,1]
         * @param t 垂直方向参数 [0,1]
         * @return 发射的光线
         */
        Ray shoot(float s, float t) const {
            // 使用圆形采样器生成镜头偏移，实现景深效果
            auto r = defaultSamplerInstance<UniformInCircle>().sample2d();
            float rx = r.x * lenRadius;
            float ry = r.y * lenRadius;
            Vec3 offset = u*rx + v*ry;
            
            // 计算光线起点和方向
            return Ray{
                position + offset,  // 光线起点（考虑镜头偏移）
                glm::normalize(
                    lowerLeft + s*horizontal + t*vertical - position - offset
                )
            };
        }
    };
}

#endif