// 渲染组件适配器实现
// 提供了光线投射渲染器与渲染系统的接口适配
#include "server/Server.hpp"
#include "component/RenderComponent.hpp"
#include "RayCastRenderer.hpp"

using namespace std;
using namespace NRenderer;

namespace RayCast
{
    // 适配器类
    // 继承自渲染组件，实现渲染接口
    class Adapter : public RenderComponent
    {
    public:
        // 渲染场景
        // spScene: 场景指针
        void render(SharedScene spScene) {
            // 创建光线投射渲染器
            RayCastRenderer rayCast{spScene};
            // 执行渲染
            auto result = rayCast.render();
            // 获取渲染结果并设置到屏幕
            auto [ pixels, width, height ] = result;
            getServer().screen.set(pixels, width, height);
            // 释放渲染结果
            rayCast.release(result);
        }
    };
}

// 渲染器描述信息
const static string description = 
    "Ray Cast Renderer.\n"
    "Supported:\n"
    " - Lambertian and Phong\n"
    " - One Point Light\n"
    " - Triangle, Sphere, Plane\n"
    " - Simple Pinhole Camera\n\n"
    "Please use ray_cast.scn"
    ;

// 注册渲染器
REGISTER_RENDERER(RayCast, description, RayCast::Adapter);
