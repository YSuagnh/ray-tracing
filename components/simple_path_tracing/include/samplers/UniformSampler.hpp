#pragma once
#ifndef __UNIFORM_SAMPLER_HPP__
#define __UNIFORM_SAMPLER_HPP__

#include "Sampler1d.hpp"
#include <ctime>

namespace SimplePathTracer
{
    using namespace std;
    
    /**
     * 均匀分布一维采样器
     * 在[0,1]区间内生成均匀分布的随机数
     */
    class UniformSampler : public Sampler1d
    {
    private:
        default_random_engine e;                    // 随机数引擎
        uniform_real_distribution<float> u;         // 均匀分布生成器
        
    public:
        /**
         * 构造函数，初始化随机数生成器
         */
        UniformSampler()
            : e                 ((unsigned int)time(0) + insideSeed())
            , u                 (0, 1)
        {}
        
        /**
         * 生成[0,1]区间内的均匀分布随机数
         * @return 随机数
         */
        float sample1d() override {
            return u(e);
        }
    };
}

#endif