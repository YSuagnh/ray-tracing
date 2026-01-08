#include "component/RenderComponent.hpp"
#include "server/Server.hpp"
#include "scene/Scene.hpp"
#include "RayTracer.hpp"

namespace RayTracer
{
    using namespace std;
    using namespace NRenderer;

    // ¼Ì³ÐRenderComponent, ¸´Ð´render½Ó¿Ú
    class Adapter : public RenderComponent
    {
        void render(SharedScene spScene) {
			RayTracerRenderer renderer{ spScene };
			auto renderResult = renderer.render();
			auto [pixels, width, height] = renderResult;
			getServer().screen.set(pixels, width, height);
			renderer.release(renderResult);
        }
    };
}

// REGISTER_RENDERER(Name, Description, Class)
REGISTER_RENDERER(RayTracer, "my own ray tracer", RayTracer::Adapter);