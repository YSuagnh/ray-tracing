#pragma once
#ifndef __UNIFORM_IN_SQUARE_HPP__
#define __UNIFORM_IN_SQUARE_HPP__

#include "Sampler2d.hpp"
#include <ctime>

namespace SimplePathTracer
{
    using namespace std;
    
    /**
     * 正方形内均匀分布采样器
     * 在[-1,1]×[-1,1]正方形区域内生成均匀分布的随机点
     */
    class UniformInSquare: public Sampler2d
    {
    private:
        default_random_engine e;                    // 随机数引擎
        uniform_real_distribution<float> u;         // 均匀分布生成器
        
    public:
        /**
         * 构造函数，初始化随机数生成器
         */
        UniformInSquare()
            : e               ((unsigned int)time(0) + insideSeed())
            , u               (-1, 1)
        {}
        
        /**
         * 生成正方形内的均匀分布随机点
         * @return 二维随机向量
         */
        Vec2 sample2d() override {
            return {u(e), u(e)};
        }
    };
}

#endif