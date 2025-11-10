#pragma once
#ifndef __SAMPLER_1D_HPP__
#define __SAMPLER_1D_HPP__

#include "Sampler.hpp"

#include <random>

namespace SimplePathTracer
{
    /**
     * 一维采样器基类
     * 提供一维随机数采样的接口
     */
    class Sampler1d : protected Sampler
    {
    public:
        Sampler1d() = default;
        
        /**
         * 生成一维随机数
         * @return 随机数
         */
        virtual float sample1d() = 0;
    };
}

#endif