// 实例基类定义
// 所有可实例化对象的基类，支持智能指针管理
#pragma once
#ifndef __NR_INSTANCE_HPP__
#define __NR_INSTANCE_HPP__

#include <memory>
#include "common/macros.hpp"

namespace NRenderer
{
    using namespace std;
    // 实例基类
    // 继承自 enable_shared_from_this 以支持安全的智能指针转换
    struct DLL_EXPORT Instance: public enable_shared_from_this<Instance>
    {
        virtual ~Instance() = default;
    };
    using SharedInstance = shared_ptr<Instance>;  // 实例的智能指针类型
}

#endif