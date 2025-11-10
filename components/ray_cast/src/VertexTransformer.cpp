// 顶点变换器实现
// 实现了场景中模型顶点的坐标变换功能
#include "VertexTransformer.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace RayCast
{
    // 执行顶点变换
    // spScene: 场景指针
    // 将场景中所有模型从局部坐标转换到世界坐标
    void VertexTransformer::exec(SharedScene spScene) {
        auto& scene = *spScene;
        // 遍历场景中的所有节点
        for (auto& node : scene.nodes) {
            // 创建变换矩阵
            Mat4x4 t{1};
            auto& model = spScene->models[node.model];
            // 应用平移变换
            t = glm::translate(t, model.translation);

            // 根据节点类型应用变换
            if (node.type == Node::Type::TRIANGLE) {
                // 变换三角形的三个顶点
                for (int i=0; i<3; i++) {
                    auto& v = scene.triangleBuffer[node.entity].v[i];
                    v = t*Vec4{v, 1};
                }
            }
            else if (node.type == Node::Type::SPHERE) {
                // 变换球体的中心位置
                auto& v = scene.sphereBuffer[node.entity].position;
                v = t*Vec4{v, 1};
            }
            else if (node.type == Node::Type::PLANE) {
                // 变换平面的位置
                auto& v = scene.planeBuffer[node.entity].position;
                v = t*Vec4{v, 1};
            }
        }
    }
}