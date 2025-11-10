#include "VertexTransformer.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace SimplePathTracer
{
    /**
     * 执行顶点变换
     * 将场景中所有几何体的顶点从局部坐标转换到世界坐标
     * @param spScene 场景共享指针
     */
    void VertexTransformer::exec(SharedScene spScene) {
        auto& scene = *spScene;
        
        // 遍历所有场景节点
        for (auto& node : scene.nodes) {
            Mat4x4 t{1};  // 初始化变换矩阵为单位矩阵
            auto& model = spScene->models[node.model];
            
            // 应用平移变换
            t = glm::translate(t, model.translation);
            
            // 根据几何体类型应用变换
            if (node.type == Node::Type::TRIANGLE) {
                // 变换三角形的三个顶点
                for (int i=0; i<3; i++) {
                    auto& v = scene.triangleBuffer[node.entity].v[i];
                    v = t*Vec4{v, 1};  // 齐次坐标变换
                }
            }
            else if (node.type == Node::Type::SPHERE) {
                // 变换球心位置
                auto& v = scene.sphereBuffer[node.entity].position;
                v = t*Vec4{v, 1};  // 齐次坐标变换
            }
            else if (node.type == Node::Type::PLANE) {
                // 变换平面位置
                auto& v = scene.planeBuffer[node.entity].position;
                v = t*Vec4{v, 1};  // 齐次坐标变换
            }
        }
    }
}