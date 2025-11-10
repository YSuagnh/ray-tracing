#include "server/Server.hpp"
#include "scene/Scene.hpp"
#include "component/RenderComponent.hpp"
#include "Camera.hpp"

#include "SimplePathTracer.hpp"

using namespace std;
using namespace NRenderer;

namespace SimplePathTracer
{
    /**
     * 简单路径追踪渲染器适配器
     * 实现RenderComponent接口，作为渲染器与系统的桥梁
     */
    class Adapter : public RenderComponent
    {
        /**
         * 执行渲染
         * @param spScene 场景共享指针
         */
        void render(SharedScene spScene) {
            // 创建路径追踪渲染器
            SimplePathTracerRenderer renderer{spScene};
            
            // 执行渲染
            auto renderResult = renderer.render();
            auto [ pixels, width, height ]  = renderResult;
            
            // 将结果设置到屏幕
            getServer().screen.set(pixels, width, height);
            
            // 释放渲染结果内存
            renderer.release(renderResult);
        }
    };
}

// 渲染器描述信息
const static string description = 
    "A Simple Path Tracer. "
    "Only some simple primitives and materials(Lambertian) are supported."
    "\nPlease use scene file : cornel_area_light.scn";

// 注册渲染器到系统
REGISTER_RENDERER(SimplePathTracer, description, SimplePathTracer::Adapter);