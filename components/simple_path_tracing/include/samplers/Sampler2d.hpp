#pragma once
#ifndef __SAMPLER_2D_HPP__
#define __SAMPLER_2D_HPP__

#include "Sampler.hpp"

#include <random>
#include "geometry/vec.hpp"

namespace SimplePathTracer
{
    using NRenderer::Vec2;
    
    /**
     * 二维采样器基类
     * 提供二维随机数采样的接口
     */
    class Sampler2d : public Sampler
    {
    public:
        Sampler2d() = default;
        
        /**
         * 生成二维随机数
         * @return 二维随机向量
         */
        virtual Vec2 sample2d() = 0;
    };
}

#endif