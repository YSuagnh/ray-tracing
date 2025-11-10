#include "server/Server.hpp"

#include "SimplePathTracer.hpp"

#include "VertexTransformer.hpp"
#include "intersections/intersections.hpp"

#include "glm/gtc/matrix_transform.hpp"

namespace SimplePathTracer
{
    /**
     * Gamma校正函数
     * 对颜色进行平方根校正，模拟人眼对亮度的感知
     * @param rgb 原始颜色
     * @return 校正后的颜色
     */
    RGB SimplePathTracerRenderer::gamma(const RGB& rgb) {
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
    void SimplePathTracerRenderer::renderTask(RGBA* pixels, int width, int height, int off, int step) {
        for(int i=off; i<height; i+=step) {  // 按步长处理行
            for (int j=0; j<width; j++) {    // 处理每行的像素
                Vec3 color{0, 0, 0};         // 初始化像素颜色
                
                // 多重采样抗锯齿
                for (int k=0; k < samples; k++) {
                    // 在像素内随机采样
                    auto r = defaultSamplerInstance<UniformInSquare>().sample2d();
                    float rx = r.x;
                    float ry = r.y;
                    float x = (float(j)+rx)/float(width);   // 归一化x坐标
                    float y = (float(i)+ry)/float(height);  // 归一化y坐标
                    
                    // 从相机发射光线
                    auto ray = camera.shoot(x, y);
                    color += trace(ray, 0);  // 路径追踪
                }
                color /= samples;  // 平均采样结果
                color = gamma(color);  // Gamma校正
                pixels[(height-i-1)*width+j] = {color, 1};  // 存储像素（翻转y坐标）
            }
        }
    }

    /**
     * 主渲染函数
     * 初始化着色器，执行多线程渲染，返回渲染结果
     * @return 渲染结果（像素数据、宽度、高度）
     */
    auto SimplePathTracerRenderer::render() -> RenderResult {
        // 初始化着色器程序
        shaderPrograms.clear();
        ShaderCreator shaderCreator{};
        for (auto& m : scene.materials) {
            shaderPrograms.push_back(shaderCreator.create(m, scene.textures));
        }

        // 分配像素缓冲区
        RGBA* pixels = new RGBA[width*height]{};

        // 将局部坐标转换成世界坐标
        VertexTransformer vertexTransformer{};
        vertexTransformer.exec(spScene);

        // 多线程渲染
        const auto taskNums = 8;  // 使用8个线程
        thread t[taskNums];
        for (int i=0; i < taskNums; i++) {
            t[i] = thread(&SimplePathTracerRenderer::renderTask,
                this, pixels, width, height, i, taskNums);
        }
        for(int i=0; i < taskNums; i++) {
            t[i].join();  // 等待所有线程完成
        }
        getServer().logger.log("Done...");
        return {pixels, width, height};
    }

    /**
     * 释放渲染结果内存
     * @param r 渲染结果
     */
    void SimplePathTracerRenderer::release(const RenderResult& r) {
        auto [p, w, h] = r;
        delete[] p;  // 释放像素缓冲区
    }

    /**
     * 查找光线与最近物体的相交
     * 遍历所有几何体，找到最近的相交点
     * @param r 光线
     * @return 最近的相交记录
     */
    HitRecord SimplePathTracerRenderer::closestHitObject(const Ray& r) {
        HitRecord closestHit = nullopt;
        float closest = FLOAT_INF;
        
        // 检查球体
        for (auto& s : scene.sphereBuffer) {
            auto hitRecord = Intersection::xSphere(r, s, 0.000001, closest);
            if (hitRecord && hitRecord->t < closest) {
                closest = hitRecord->t;
                closestHit = hitRecord;
            }
        }
        
        // 检查三角形
        for (auto& t : scene.triangleBuffer) {
            auto hitRecord = Intersection::xTriangle(r, t, 0.000001, closest);
            if (hitRecord && hitRecord->t < closest) {
                closest = hitRecord->t;
                closestHit = hitRecord;
            }
        }
        
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
    tuple<float, Vec3> SimplePathTracerRenderer::closestHitLight(const Ray& r) {
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
    RGB SimplePathTracerRenderer::trace(const Ray& r, int currDepth) {
        // 达到最大递归深度，返回环境光
        if (currDepth == depth) return scene.ambient.constant;
        
        // 查找最近的物体和光源相交
        auto hitObject = closestHitObject(r);
        auto [ t, emitted ] = closestHitLight(r);
        
        // 如果光线击中物体
        if (hitObject && hitObject->t < t) {
            auto mtlHandle = hitObject->material;
            // 使用材质着色器计算散射
            auto scattered = shaderPrograms[mtlHandle.index()]->shade(r, hitObject->hitPoint, hitObject->normal);
            auto scatteredRay = scattered.ray;
            auto attenuation = scattered.attenuation;
            auto emitted = scattered.emitted;
            
            // 递归追踪散射光线
            auto next = trace(scatteredRay, currDepth+1);
            float n_dot_in = glm::dot(hitObject->normal, scatteredRay.direction);
            float pdf = scattered.pdf;
            
            /**
             * 路径追踪渲染方程实现：
             * emitted      - Le(p, w_0) 自发光
             * next         - Li(p, w_i) 入射光
             * n_dot_in     - cos<n, w_i> 余弦项
             * attenuation  - BRDF 双向反射分布函数
             * pdf          - p(w) 概率密度函数
             **/
            return emitted + attenuation * next * n_dot_in / pdf;
        }
        // 如果光线击中光源
        else if (t != FLOAT_INF) {
            return emitted;
        }
        // 光线未击中任何物体
        else {
            return Vec3{0};
        }
    }
}