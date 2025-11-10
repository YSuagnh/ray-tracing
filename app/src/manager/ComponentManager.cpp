#include "manager/ComponentManager.hpp"
#include "io.h"

// 组件管理器实现文件
// 负责管理渲染组件的加载、执行和状态

namespace NRenderer
{
    // 构造函数
    // 初始化组件管理器的状态和成员变量
    ComponentManager::ComponentManager()
        : state             (State::IDLING)         // 初始状态为空闲
        , loadedDlls        ()                      // 已加载的DLL列表
        , activeComponent   ()                      // 当前活动组件
        , lastStartTime     ()                      // 上次执行开始时间
        , lastEndTime       ()                      // 上次执行结束时间
        , t                 ()                      // 执行线程
    {}

    // 初始化组件管理器
    // path: DLL文件的搜索路径
    void ComponentManager::init(const string& path) {
        auto& f = getServer().componentFactory;
        _finddata_t findData;
        
        // 搜索并加载所有DLL文件
        auto handle = _findfirst(path.c_str(), &findData);
        if (handle == -1) return;
        do {
            string name{findData.name};
            string filePath = (path.substr(0 ,path.size() - 6) + "\\" + name);
            
            // 加载DLL文件
            auto h = ::LoadLibrary((LPCSTR)filePath.c_str());
            if (h != NULL) {
                loadedDlls.push_back(h);  // 保存DLL句柄
            }
        } while (_findnext(handle, &findData) == 0);
        _findclose(handle);
    }

    // 完成组件执行
    // 将状态设置为空闲
    void ComponentManager::finish() {
        state = State::IDLING;
    }

    // 获取当前活动组件的信息
    // 返回值: 活动组件的信息
    ComponentInfo ComponentManager::getActiveComponentInfo() const {
        return activeComponent;
    }

    // 获取当前组件管理器的状态
    // 返回值: 当前状态
    ComponentManager::State ComponentManager::getState() const {
        return state;
    }

    // 获取上次执行的时间
    // 返回值: 执行持续时间
    chrono::duration<double> ComponentManager::getLastExecTime() const {
        return lastEndTime - lastStartTime;
    }

    // 析构函数
    // 释放所有加载的DLL
    ComponentManager::~ComponentManager()
    {
        for (auto& h : loadedDlls) {
            ::FreeLibrary(h);  // 释放DLL
        }
    }

} // namespace NRenderer
