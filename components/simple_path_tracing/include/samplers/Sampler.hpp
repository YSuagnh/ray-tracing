#pragma once
#ifndef __SAMPLER_HPP__
#define __SAMPLER_HPP__

#include <mutex>

namespace SimplePathTracer
{
    using std::mutex;
    
    /**
     * 采样器基类
     * 提供随机数生成的基础功能，确保线程安全
     */
    class Sampler
    {
    protected:
        /**
         * 获取内部种子值，确保线程安全
         * @return 递增的种子值
         */
        static int insideSeed() {
            static mutex m;
            static int seed = 0;
            m.lock();
            seed++;
            m.unlock();
            return seed;
        }
    public:
        virtual ~Sampler() = default;
        Sampler() = default;
    };
}

#endif