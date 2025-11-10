#include "manager/Manager.hpp"
#include "server/Server.hpp"

// 管理器实现文件
// 负责管理渲染设置、资产和组件

namespace NRenderer
{
    // 构造函数
    // 初始化所有子管理器并加载组件
    Manager::Manager()
        : renderSettingsManager             ()  // 渲染设置管理器
        , assetManager                      ()  // 资产管理器
        , componentManager                  ()  // 组件管理器
    {
        componentManager.init(".\\components\\*.dll");  // 初始化组件管理器，加载DLL插件
   }
}