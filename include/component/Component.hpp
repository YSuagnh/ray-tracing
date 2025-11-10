// 组件基类定义
// 提供了组件系统的基础功能和组件注册机制
#pragma once
#ifndef __NR_COMPONENT_HPP__
#define __NR_COMPONENT_HPP__

#include "Instance.hpp"
#include <string>
#include "ComponentFactory.hpp"
namespace NRenderer
{
    using namespace std;
    // 组件基类
    // 所有具体组件都应该继承自这个类
    class DLL_EXPORT Component: public Instance
    {
    private:
    protected:
    public:
        Component() = default;
        ~Component() = default;
    };
} // namespace NRenderer

#include "server/Server.hpp"

// 组件注册宏
// 用于将组件注册到组件工厂中，使其可以被系统识别和创建
// __TYPE__: 组件类型
// __NAME__: 组件名称
// __DESCRIPTION__: 组件描述
// __CLASS__: 组件类
#define REGISTER_COMPONENT(__TYPE__, __NAME__, __DESCRIPTION__ , __CLASS__)                                                     \
        struct ComponentRegister {                                                                                              \
            ComponentRegister() {                                                                                               \
                getServer().componentFactory.registerComponent(#__TYPE__, #__NAME__, __DESCRIPTION__, std::make_shared<__CLASS__>);    \
            }                                                                                                                   \
            ~ComponentRegister() {                                                                                              \
                getServer().componentFactory.unregisterComponent(#__TYPE__, #__NAME__);                                                \
            }                                                                                                                   \
        };                                                                                                                      \
        static ComponentRegister __cm_reg__{};                                                                                  \

// 无描述组件注册宏
// 简化版的组件注册宏，不需要提供描述信息
#define REGISTER_COMPONENT_NO_DESCRIPTION(__TYPE__, __NAME__, __CLASS__)   REGISTER_COMPONENT(__TYPE__, __NAME__, "", __CLASS__)

#endif