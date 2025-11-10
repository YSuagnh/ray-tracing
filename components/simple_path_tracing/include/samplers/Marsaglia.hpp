#pragma once
#ifndef __MARSAGLIA_HPP__
#define __MARSAGLIA_HPP__

#include "Sampler3d.hpp"
#include <ctime>

namespace SimplePathTracer
{
    using namespace std;
    
    /**
     * Marsaglia球面均匀分布采样器
     * 在单位球面上生成均匀分布的随机方向
     * 使用Marsaglia方法，通过拒绝采样在单位圆内生成点，然后映射到球面
     */
    class Marsaglia : public Sampler3d
    {
    private:
        default_random_engine e;                    // 随机数引擎
        uniform_real_distribution<float> u;         // 均匀分布生成器
        
    public:
        /**
         * 构造函数，初始化随机数生成器
         */
        Marsaglia()
            : e               ((unsigned int)time(0) + insideSeed())
            , u               (-1, 1)
        {}

        /**
         * 生成单位球面上的均匀分布随机方向
         * 使用Marsaglia方法：在单位圆内采样，然后映射到球面
         * @return 三维随机向量（单位向量）
         */
        Vec3 sample3d() override {
            float u_{0}, v_{0};
            float r2{0};
            do {
                u_ = u(e);
                v_ = u(e);
                r2 = u_*u_ + v_*v_;
            } while (r2 > 1);  // 拒绝采样，确保点在单位圆内
            
            // Marsaglia变换：将圆内点映射到球面
            float x = 2 * u_ * sqrt(1 - r2);
            float y = 2 * v_ * sqrt(1 - r2);
            float z = 1 - 2 * r2;
            return { x, y, z };
        }
    };
}

#endif