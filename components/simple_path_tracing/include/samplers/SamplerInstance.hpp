#pragma once
#ifndef __SAMPLER_INSTANCE_HPP__
#define __SAMPLER_INSTANCE_HPP__

#include "HemiSphere.hpp"
#include "Marsaglia.hpp"
#include "UniformSampler.hpp"
#include "UniformInCircle.hpp"
#include "UniformInSquare.hpp"

namespace SimplePathTracer
{
    /**
     * 获取默认采样器实例的模板函数
     * 使用线程局部存储确保多线程安全
     * @tparam T 采样器类型，必须是Sampler1d、Sampler2d或Sampler3d的子类
     * @return 采样器实例的引用
     */
    template<typename T>
    T& defaultSamplerInstance() {
        static_assert(
            is_base_of<Sampler1d, T>::value ||
            is_base_of<Sampler2d, T>::value ||
            is_base_of<Sampler3d, T>::value, "Not a sampler type.");
        thread_local static T t{};  // 线程局部静态变量，确保多线程安全
        return t;
    }
}

#endif