#include "server/Server.hpp"

#include "RayTracer.hpp"

#include "VertexTransformer.hpp"
#include "intersections/intersections.hpp"

#include "glm/gtc/matrix_transform.hpp"

namespace RayTracer
{
    /**
     * Gamma校正函数
     * 对颜色进行平方格校正，模拟人眼对亮度的感知
     * @param rgb 原始颜色
     * @return 校正后的颜色
     */
    RGB RayTracerRenderer::gamma(const RGB& rgb) {
        return glm::sqrt(rgb);
    }

    /**
     * 渲染任务函数（多线程）
     * 处理指定范围内的像素行，进行路径追踪计算
     * @param pixels 像素缓冲区
     * @param width 图像宽度
     * @param height 图像高度
     * @param off 起始行偏移
     * @param step 行步长（用于多线程分配）
     */
    void RayTracerRenderer::renderTask(RGBA* pixels, int width, int height, int off, int step) {
        for (int i = off; i < height; i += step) {
            for (int j = 0; j < width; j++) {
                Vec3 color{ 0, 0, 0 };

                // 多重采样抗锯齿
                for (int k = 0; k < samples; k++) {
                    // 在像素内随机采样
                    auto r = defaultSamplerInstance<UniformInSquare>().sample2d();
                    float x = (float(j) + r.x) / float(width);   // 归一化x坐标
                    float y = (float(i) + r.y) / float(height);  // 归一化y坐标

                    // 从相机发射光线
                    auto ray = camera.shoot(x, y);
                    color += trace(ray, 0);  // 路径追踪
                }
                color /= samples;  // 平均采样结果
                color = gamma(color);  // Gamma校正
                pixels[(height - i - 1) * width + j] = { color, 1 };  // 存储像素（翻转y坐标）
            }
			std::cerr << "Thread " << off << " finished line " << i << std::endl;
        }
    }

    /**
     * 主渲染函数
     * 初始化着色器，执行多线程渲染，返回渲染结果
     * @return 渲染结果（像素数据、宽度、高度）
     */
    auto RayTracerRenderer::render() -> RenderResult {
        std::cerr << "Render started." << std::endl;
        
        // 将局部坐标转换成世界坐标 - 必须在构建KDTree和光子映射之前完成
        VertexTransformer vertexTransformer{};
        vertexTransformer.exec(spScene);

        kdtree = make_shared<KDT::KDTree>(scene);

        shaderPrograms.clear();
        ShaderCreator shaderCreator{};
        for (auto& m : scene.materials) {
            auto shader = shaderCreator.create(m, scene.textures);
            shaderPrograms.push_back(shader);
        }

		photonMapping = make_shared<PhotonMapping>(spScene, &shaderPrograms, kdtree);
        // build photon map once per render (optional)
        if (photonMapping) {
            // default photon count can be overridden by scene/materials later
            photonMapping->build();
        }
        std::cerr << "photo map build\n";
        // 初始化着色器程序

        // 分配像素缓冲区
        RGBA* pixels = new RGBA[width * height]{};



        // 多线程渲染
        const auto taskNums = 8;  // 使用8个线程
        thread t[taskNums];
        for (int i = 0; i < taskNums; i++) {
            t[i] = thread(&RayTracerRenderer::renderTask,
                this, pixels, width, height, i, taskNums);
        }
        for (int i = 0; i < taskNums; i++) {
            t[i].join();  // 等待所有线程完成
        }
        getServer().logger.log("Done...");
        return { pixels, width, height };
    }

    /**
     * 释放渲染结果内存
     * @param r 渲染结果
     */
    void RayTracerRenderer::release(const RenderResult& r) {
        auto [p, w, h] = r;
        delete[] p;  // 释放像素缓冲区
    }

    /**
     * 查找光线与最近物体的相交
     * 遍历所有几何体，找到最近的相交点
     * @param r 光线
     * @return 最近的相交记录
     */
    HitRecord RayTracerRenderer::closestHitObject(const Ray& r) {
        HitRecord closestHit = nullopt;
        float closest = FLOAT_INF;

		auto hitRecordKDT = kdtree->intersect(r, 0.000001, closest);
		if (hitRecordKDT) return hitRecordKDT;

        // 检查平面
        for (auto& p : scene.planeBuffer) {
            auto hitRecord = Intersection::xPlane(r, p, 0.000001, closest);
            if (hitRecord && hitRecord->t < closest) {
                closest = hitRecord->t;
                closestHit = hitRecord;
            }
        }
        return closestHit;
    }

    /**
     * 查找光线与最近光源的相交
     * 遍历所有区域光源，找到最近的相交点
     * @param r 光线
     * @return 相交距离和发光强度
     */
    tuple<float, Vec3> RayTracerRenderer::closestHitLight(const Ray& r) {
        Vec3 v = {};
        HitRecord closest = getHitRecord(FLOAT_INF, {}, {}, {});

        // 检查区域光源
        for (auto& a : scene.areaLightBuffer) {
            auto hitRecord = Intersection::xAreaLight(r, a, 0.000001, closest->t);
            if (hitRecord && closest->t > hitRecord->t) {
                closest = hitRecord;
                v = a.radiance;  // 记录发光强度
            }
        }
        return { closest->t, v };
    }

    /**
     * 路径追踪主函数
     * 实现蒙特卡洛路径追踪算法，递归计算光线颜色
     * @param r 光线
     * @param currDepth 当前递归深度
     * @return 光线颜色
     */
    RGB RayTracerRenderer::trace(const Ray& r, int currDepth, bool flag) {
        if (currDepth == depth) return scene.ambient.constant;

        auto hitObject = closestHitObject(r);
        auto [t, emitted] = closestHitLight(r);

        if (hitObject && hitObject->t < t) {
            auto mtlHandle = hitObject->material;
            auto shader = shaderPrograms[mtlHandle.index()];

            Vec3 pmIndirect(0.0f);

            int shaderType = 1;
            bool isDielectric = false;
            bool isDiffuse = false;
            {
                auto st = scene.materials[mtlHandle.index()].getProperty<Property::Wrapper::IntType>("ShaderType");
                if (st) shaderType = (*st).value;
                if (shaderType == 3) {
                    isDielectric = true;
                }
                auto id = scene.materials[mtlHandle.index()].getProperty<Property::Wrapper::IntType>("isDiffuse");
                if (id) isDiffuse = (*id).value;
            }

            // Photon gather once on the first diffuse hit
            if (flag && isDiffuse && photonMapping) {
                flag = false;
                float radius = 0.05f;
                auto radProp = scene.materials[mtlHandle.index()].getProperty<Property::Wrapper::FloatType>("photonRadius");
                if (radProp) radius = glm::max(1e-3f, (*radProp).value);

                std::vector<Photon> nearby;
                photonMapping->findInRadius(nearby, hitObject->hitPoint, radius);

                Vec3 flux(0.0f);
                int count = 0;
                for (auto& ph : nearby) {
                    if (!ph) continue;
                    // ph->ray.direction 存储的是入射光的反方向（即从hit point指向光源）
                    // 要检查入射光方向与法线的关系，需要取反
                    const Vec3 wi = -glm::normalize(ph->ray.direction);  // 实际入射方向
                    // 入射光应该从法线同侧射入（dot > 0 表示错误方向）
                    if (glm::dot(hitObject->normal, wi) >= 0.0f) continue;
                    flux += ph->power;
                    ++count;
                }

                if (count > 0) {
                    const float area = PI * radius * radius;
                    Vec3 irradiance = flux / glm::max(area, 1e-6f);

                    Vec3 kd(1.0f);
                    auto diffuseColor = scene.materials[mtlHandle.index()].getProperty<Property::Wrapper::RGBType>("diffuseColor");
                    if (diffuseColor) kd = (*diffuseColor).value;

                    pmIndirect = irradiance * (kd / PI);
                    pmIndirect = glm::clamp(pmIndirect, Vec3(0.0f), Vec3(50.0f));
                }
            }

            auto scattered = shader->shade(r, hitObject->hitPoint, hitObject->normal);

            // Dielectric is a delta BSDF (reflect/refract).
            if (isDielectric) {
                return scattered.emitted + scattered.attenuation * trace(scattered.ray, currDepth + 1, flag);
            }

            // For non-delta materials, use classic estimator.
            // Note: for specular lobes (Phong), directions can be valid even when numeric issues
            // make dot(n, wo) slightly negative; clamp but do NOT early-return black.
            const float pdf = glm::max(scattered.pdf, 1e-8f);
            const float cosTheta = glm::max(glm::dot(hitObject->normal, scattered.ray.direction), 0.0f);

            auto next = trace(scattered.ray, currDepth + 1, flag);
            Vec3 path = scattered.emitted + scattered.attenuation * next * (cosTheta / pdf);

            return pmIndirect + path;
        }
        else if (t != FLOAT_INF) {
            return emitted;
        }
        else {
            return Vec3{ 0.0f };
        }
    }
}