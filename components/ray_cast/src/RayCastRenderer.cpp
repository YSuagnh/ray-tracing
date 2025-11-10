// 光线投射渲染器实现
// 实现了基本的光线投射渲染算法，包括阴影计算
#include "RayCastRenderer.hpp"
#include "VertexTransformer.hpp"
#include "intersections/intersections.hpp"

namespace RayCast
{
    // 释放渲染结果
    // r: 渲染结果
    void RayCastRenderer::release(const RenderResult& r) {
        auto [p, w, h] = r;
        delete[] p;
    }

    // 伽马校正
    // rgb: 输入的RGB颜色
    // 返回校正后的颜色
    RGB RayCastRenderer::gamma(const RGB& rgb) {
        return glm::sqrt(rgb);
    }

    // 渲染场景
    // 返回渲染结果
    auto RayCastRenderer::render() -> RenderResult {
        auto width = scene.renderOption.width;
        auto height = scene.renderOption.height;
        // 创建像素缓冲区
        auto pixels = new RGBA[width*height];

        // 执行顶点变换
        VertexTransformer vertexTransformer{};
        vertexTransformer.exec(spScene);

        // 创建着色器程序
        ShaderCreator shaderCreator{};
        for (auto& mtl : scene.materials) {
            shaderPrograms.push_back(shaderCreator.create(mtl, scene.textures));
        }

        // 对每个像素进行光线追踪
        for (int i=0; i<height; i++) {
            for (int j=0; j < width; j++) {
                // 生成光线
                auto ray = camera.shoot(float(j)/float(width), float(i)/float(height));
                // 追踪光线获取颜色
                auto color = trace(ray);
                // 颜色后处理
                color = clamp(color);
                color = gamma(color);
                pixels[(height-i-1)*width+j] = {color, 1};
            }
        }

        return {pixels, width, height};
    }
    
    // 光线追踪
    // r: 输入光线
    // 返回该光线对应的颜色
    RGB RayCastRenderer::trace(const Ray& r) {
        // 如果场景中没有点光源，返回黑色
        if (scene.pointLightBuffer.size() < 1) return {0, 0, 0};
        // 获取第一个点光源
        auto& l = scene.pointLightBuffer[0];
        // 计算最近的相交物体
        auto closestHitObj = closestHit(r);
        
        if (closestHitObj) {
            auto& hitRec = *closestHitObj;
            // 计算光源方向
            auto out = glm::normalize(l.position - hitRec.hitPoint);
            // 检查光源是否在表面背面
            if (glm::dot(out, hitRec.normal) < 0) {
                return {0, 0, 0};
            }
            // 计算到光源的距离
            auto distance = glm::length(l.position - hitRec.hitPoint);
            // 生成阴影射线
            auto shadowRay = Ray{hitRec.hitPoint, out};
            auto shadowHit = closestHit(shadowRay);
            // 计算着色结果
            auto c = shaderPrograms[hitRec.material.index()]->shade(-r.direction, out, hitRec.normal);
            // 如果没有遮挡或遮挡物在光源后面，返回着色结果
            if ((!shadowHit) || (shadowHit && shadowHit->t > distance)) {
                return c * l.intensity;
            }
            // 否则在阴影中，返回黑色
            else {
                return Vec3{0};
            }
        }
        else {
            return {0, 0, 0};
        }
    }

    // 计算最近的相交物体
    // r: 输入光线
    // 返回最近的相交记录
    HitRecord RayCastRenderer::closestHit(const Ray& r) {
        HitRecord closestHit = nullopt;
        float closest = FLOAT_INF;
        // 检查与球体的相交
        for (auto& s : scene.sphereBuffer) {
            auto hitRecord = Intersection::xSphere(r, s, 0.01, closest);
            if (hitRecord && hitRecord->t < closest) {
                closest = hitRecord->t;
                closestHit = hitRecord;
            }
        }
        // 检查与三角形的相交
        for (auto& t : scene.triangleBuffer) {
            auto hitRecord = Intersection::xTriangle(r, t, 0.01, closest);
            if (hitRecord && hitRecord->t < closest) {
                closest = hitRecord->t;
                closestHit = hitRecord;
            }
        }
        // 检查与平面的相交
        for (auto& p : scene.planeBuffer) {
            auto hitRecord = Intersection::xPlane(r, p, 0.01, closest);
            if (hitRecord && hitRecord->t < closest) {
                closest = hitRecord->t;
                closestHit = hitRecord;
            }
        }
        return closestHit; 
    }
}