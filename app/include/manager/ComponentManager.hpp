#pragma once
#ifndef __NR_COMPONENT_MANAGER_HPP__
#define __NR_COMPONENT_MANAGER_HPP__

// 组件管理器头文件
// 定义了管理渲染组件的功能类

#include <Windows.h>
#include <vector>
#include <chrono>
#include <thread>

#include "component/RenderComponent.hpp"
#include "server/Server.hpp"

namespace NRenderer
{
    using namespace std;

    // 组件管理器类
    // 负责管理和执行渲染组件
    class DLL_EXPORT ComponentManager
    {
    public:
        // 组件状态枚举
        enum class State
        {
            IDLING,     // 空闲状态
            READY,      // 就绪状态
            RUNNING,    // 运行状态
            FINISH      // 完成状态
        };
    private:
        State state;                    // 当前状态
        vector<HMODULE> loadedDlls;     // 已加载的DLL模块列表
        ComponentInfo activeComponent;   // 当前活动组件信息
        chrono::system_clock::time_point lastStartTime;  // 上次开始时间
        chrono::system_clock::time_point lastEndTime;    // 上次结束时间
        thread t;                       // 执行线程

    public:
        // 构造函数
        ComponentManager();
        // 析构函数
        ~ComponentManager();

        // 初始化组件管理器
        // dllPath: DLL文件路径
        void init(const string& dllPath);
        
        // 获取当前活动组件信息
        // 返回: 活动组件的信息
        ComponentInfo getActiveComponentInfo() const;
        
        // 执行组件
        // Interface: 组件接口类型
        // componentInfo: 组件信息
        // args: 组件执行参数
        template<typename Interface, typename ...Args>
        void exec(const ComponentInfo& componentInfo, Args... args) {
            auto component = getServer().componentFactory.createComponent<Interface>(componentInfo.type, componentInfo.name);
            activeComponent = componentInfo;
            this->state = State::READY;
            try {
                t = thread(&Interface::exec,
                    component, 
                    [this]() -> void {
                        this->state = State::RUNNING;
                        this->lastStartTime = chrono::system_clock::now();
                    },
                    [this]() -> void {
                        this->state = State::FINISH;
                        this->lastEndTime = chrono::system_clock::now();
                    },
                    std::forward<Args>(args)...);
                    t.detach();
            }
            catch(const exception& e) {
                cerr<<"Unexpected termination"<<endl;
                cerr<<e.what()<<endl;
            }
        }
    
        // 完成组件执行
        void finish();

        // 获取当前状态
        // 返回: 当前组件状态
        State getState() const;

        // 获取上次执行时间
        // 返回: 上次执行的持续时间
        chrono::duration<double> getLastExecTime() const;
    };
} // namespace NRenderer


#endif