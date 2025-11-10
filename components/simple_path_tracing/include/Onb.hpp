#pragma once
#ifndef __ONB_HPP__
#define __ONB_HPP__

#include "geometry/vec.hpp"

namespace SimplePathTracer
{
    using namespace NRenderer;
    
    /**
     * 正交基（Orthonormal Basis）类
     * 用于在局部坐标系和世界坐标系之间进行转换
     * 主要用于将半球采样结果转换到以法向量为z轴的局部坐标系
     */
    class Onb
    {
    private:
        Vec3 u;  // 局部坐标系的x轴
        Vec3 v;  // 局部坐标系的y轴
        Vec3 w;  // 局部坐标系的z轴（法向量方向）
        
    public:
        /**
         * 构造函数，根据法向量建立正交基
         * @param normal 法向量，将作为局部坐标系的z轴
         */
        Onb(const Vec3& normal) {
            w = normal;  // z轴为法向量方向
            
            // 选择一个与法向量不平行的向量作为参考
            Vec3 a = (fabs(w.x) > 0.9) ? Vec3{0, 1, 0} : Vec3{1, 0, 0};
            
            // 通过叉积计算y轴和x轴
            v = glm::normalize(glm::cross(w, a));  // y轴
            u = glm::cross(w, v);                  // x轴
        }
        
        ~Onb() = default;

        /**
         * 将局部坐标系的向量转换到世界坐标系
         * @param v 局部坐标系中的向量
         * @return 世界坐标系中的向量
         */
        Vec3 local(const Vec3& v) const {
            return v.x*this->u + v.y*this->v + v.z * this->w;
        }
    };
}


#endif