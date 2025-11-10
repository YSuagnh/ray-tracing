#pragma once
#ifndef __SAMPLER_3D_HPP__
#define __SAMPLER_3D_HPP__

#include "Sampler.hpp"

#include <random>
#include "geometry/vec.hpp"

namespace SimplePathTracer
{
    using NRenderer::Vec3;
    
    /**
     * 三维采样器基类
     * 提供三维随机数采样的接口
     */
    class Sampler3d : public Sampler
    {
        
    public:
        Sampler3d() = default;
        
        /**
         * 生成三维随机数
         * @return 三维随机向量
         */
        virtual Vec3 sample3d() = 0;
    };
}

#endif