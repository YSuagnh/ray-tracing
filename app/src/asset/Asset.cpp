#include "asset/Asset.hpp"

// 资产管理实现文件
// 实现了场景资产的OpenGL预览功能

namespace NRenderer
{
    // 为每个节点生成OpenGL预览缓冲区
    // 包括顶点数组对象(VAO)、顶点缓冲对象(VBO)和索引缓冲对象(EBO)
    void Asset::genPreviewGlBuffersPerNode(NodeItem& node) {
        // 清理已存在的缓冲区
        if (node.glVAO != 0) {
            glDeleteVertexArrays(1, &node.glVAO);
            node.glVAO = 0;
        }
        if (node.glVBO != 0) {
            glDeleteBuffers(1, &node.glVBO);
            node.glVBO = 0;
        }
        if (node.glEBO != 0) {
            glDeleteBuffers(1, &node.glEBO);
            node.glEBO = 0;
        }

        // 创建新的顶点数组对象
        glGenVertexArrays(1, &node.glVAO);
        glBindVertexArray(node.glVAO);
        auto& np = *node.node;
        using T = Node::Type;

        // 根据节点类型生成不同的缓冲数据
        if (np.type == T::TRIANGLE) {  // 三角形节点
            auto& t = *triangles[np.entity];
            glGenBuffers(1, &node.glVBO);

            // 设置顶点数据
            glBindBuffer(GL_ARRAY_BUFFER, node.glVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3)*3, &t.v[0], GL_DYNAMIC_DRAW);
            
            // 设置顶点属性
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), (void *)0);
        
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        else if (np.type == T::SPHERE) {  // 球体节点
            auto&s = *spheres[np.entity];
            node.externalDrawData = SharedGlDrawData{new GlDrawData{}};
            auto& bf = node.externalDrawData->positions;
            
            // 生成球体的线框数据
            const int count = 60;
            // XY平面圆
            for (int i=0; i<=count; i++) {
                Vec3 p = (s.position + Vec3{s.radius*cos(float(i)/float(count)*6.28) , s.radius*sin(float(i)/float(count)*6.28) , 0});
                bf.push_back(p);
            }
            // XZ平面圆
            for (int i=0; i<=count; i++) {
                Vec3 p = (s.position + Vec3{s.radius*cos(float(i)/float(count)*6.28) , 0, s.radius*sin(float(i)/float(count)*6.28)});
                bf.push_back(p);
            }
            // XZ平面部分圆
            for (int i=0; i<=count/4; i++) {
                Vec3 p = (s.position + Vec3{s.radius*cos(float(i)/float(count)*6.28) , 0, s.radius*sin(float(i)/float(count)*6.28)});
                bf.push_back(p);
            }
            // YZ平面圆
            for (int i=0; i<=count; i++) {
                Vec3 p = (s.position + Vec3{0, s.radius*sin(float(i)/float(count)*6.28) , s.radius*cos(float(i)/float(count)*6.28)});
                bf.push_back(p);
            }

            // 设置顶点数据
            glGenBuffers(1, &node.glVBO);
            glBindBuffer(GL_ARRAY_BUFFER, node.glVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3)*bf.size(), &bf[0], GL_DYNAMIC_DRAW);
            
            // 设置顶点属性
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), (void *)0);
        
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        else if (np.type == T::PLANE) {  // 平面节点
            auto&p = *planes[np.entity];
            node.externalDrawData = make_shared<GlDrawData>();
            auto& bf = node.externalDrawData->positions;
            
            // 生成平面的四个顶点
            bf.push_back(p.position);
            bf.push_back(p.position + p.u);
            bf.push_back(p.position + p.u + p.v);
            bf.push_back(p.position + p.v);

            // 设置顶点数据
            glGenBuffers(1, &node.glVBO);
            glBindBuffer(GL_ARRAY_BUFFER, node.glVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3)*bf.size(), &bf[0], GL_DYNAMIC_DRAW);
            
            // 设置顶点属性
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), (void *)0);
        
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        else if (np.type == T::MESH) {  // 网格节点
            auto& m = *meshes[np.entity];
            
            // 创建顶点和索引缓冲区
            glGenBuffers(1, &node.glVBO);
            glGenBuffers(1, &node.glEBO);

            // 设置顶点数据
            glBindBuffer(GL_ARRAY_BUFFER, node.glVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3)*m.positions.size(), &m.positions[0], GL_DYNAMIC_DRAW);

            // 设置索引数据
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, node.glEBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* m.positionIndices.size(), &m.positionIndices[0], GL_DYNAMIC_DRAW);

            // 设置顶点属性
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), (void *)0);
        }

        glBindVertexArray(0);
    }

    // 为每个光源生成OpenGL预览缓冲区
    void Asset::genPreviewGlBuffersPerLight(LightItem& light) {
        // 清理已存在的缓冲区
        if (light.glVAO != 0) {
            glDeleteVertexArrays(1, &light.glVAO);
            light.glVAO = 0;
        }
        if (light.glVBO != 0) {
            glDeleteBuffers(1, &light.glVBO);
            light.glVBO = 0;
        }
        if (light.glEBO != 0) {
            glDeleteBuffers(1, &light.glEBO);
            light.glEBO = 0;
        }

        auto& lp = *light.light;
        using T = Light::Type;
        light.externalDrawData = make_shared<GlDrawData>();
        auto& ps = light.externalDrawData->positions;
        
        // 创建顶点数组对象
        glGenVertexArrays(1, &light.glVAO);
        glBindVertexArray(light.glVAO);

        // 根据光源类型生成不同的缓冲数据
        if (lp.type == T::POINT) {  // 点光源
            const float l = 1.f/20.f;
            const int count = 8;
            // 生成点光源的线框表示
            for (int i=0; i < count; i++) {
                Vec3 p{l*cos(float(i)/float(count)*2*3.1415926f), l*sin(float(i)/float(count)*2*3.1415926f), 0};
                ps.push_back({0, 0, 0});
                ps.push_back(p);
            }

            // 设置顶点数据
            glGenBuffers(1, &light.glVBO);
            glBindBuffer(GL_ARRAY_BUFFER, light.glVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3)*ps.size(), &ps[0], GL_STATIC_DRAW);
            
            // 设置顶点属性
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), (void *)0);
        
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        else if (lp.type == T::AREA) {  // 面光源
            ps.assign(4, {});
            auto& a = *areaLights[lp.entity];
            // 生成面光源的四个顶点
            ps[0] = {0, 0, 0};
            ps[1] = ps[0] + a.u;
            ps[2] = ps[1] + a.v;
            ps[3] = ps[0] + a.v;

            // 设置顶点数据
            glGenBuffers(1, &light.glVBO);
            glBindBuffer(GL_ARRAY_BUFFER, light.glVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3)*ps.size(), &ps[0], GL_DYNAMIC_DRAW);
            
            // 设置顶点属性
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), (void *)0);
        
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        else if (lp.type == T::DIRECTIONAL) {  // 平行光源
            // 待实现
        }
        else if (lp.type == T::SPOT) {  // 聚光灯
            // 待实现
        }
        glBindVertexArray(0);
    }

    // 更新节点的OpenGL绘制数据
    void Asset::updateNodeGlDrawData(NodeItem& node) {
        auto& np = *node.node;
        using T = Node::Type;
        
        // 根据节点类型更新不同的数据
        if (np.type == T::SPHERE) {  // 更新球体数据
            node.externalDrawData = make_shared<GlDrawData>();
            auto&s = *spheres[np.entity];
            node.externalDrawData = SharedGlDrawData{new GlDrawData{}};
            auto& bf = node.externalDrawData->positions;
            
            // 重新生成球体的线框数据
            const int count = 60;
            for (int i=0; i<=count; i++) {
                Vec3 p = (s.position + Vec3{s.radius*cos(float(i)/float(count)*6.28) , s.radius*sin(float(i)/float(count)*6.28) , 0});
                bf.push_back(p);
            }
            for (int i=0; i<=count; i++) {
                Vec3 p = (s.position + Vec3{s.radius*cos(float(i)/float(count)*6.28) , 0, s.radius*sin(float(i)/float(count)*6.28)});
                bf.push_back(p);
            }
            for (int i=0; i<=count/4; i++) {
                Vec3 p = (s.position + Vec3{s.radius*cos(float(i)/float(count)*6.28) , 0, s.radius*sin(float(i)/float(count)*6.28)});
                bf.push_back(p);
            }
            for (int i=0; i<=count; i++) {
                Vec3 p = (s.position + Vec3{0, s.radius*sin(float(i)/float(count)*6.28) , s.radius*cos(float(i)/float(count)*6.28)});
                bf.push_back(p);
            }

            // 更新缓冲区数据
            glBindBuffer(GL_ARRAY_BUFFER, node.glVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vec3)*bf.size(), &bf[0]);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        else if (np.type == T::PLANE) {  // 更新平面数据
            auto&p = *planes[np.entity];
            node.externalDrawData = make_shared<GlDrawData>();
            auto& bf = node.externalDrawData->positions;
            
            // 重新生成平面的顶点数据
            bf.push_back(p.position);
            bf.push_back(p.position + p.u);
            bf.push_back(p.position + p.u + p.v);
            bf.push_back(p.position + p.v);

            // 更新缓冲区数据
            glBindBuffer(GL_ARRAY_BUFFER, node.glVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vec3)*bf.size(), &bf[0]);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        else if (np.type == T::TRIANGLE) {  // 更新三角形数据
            auto& t = *triangles[np.entity];
            // 更新缓冲区数据
            glBindBuffer(GL_ARRAY_BUFFER, node.glVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vec3)*3, &t.v[0]);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
    }

    // 更新光源的OpenGL绘制数据
    void Asset::updateLightGlDrawData(LightItem& light) {
        auto& lp = *light.light;
        using T = Light::Type;
        auto& ps = light.externalDrawData->positions;
        
        // 根据光源类型更新不同的数据
        if (lp.type == T::POINT) {  // 点光源
            // 点光源不需要更新
        }
        else if (lp.type == T::AREA) {  // 面光源
            light.externalDrawData = make_shared<GlDrawData>();
            ps.assign(4, {});
            auto& a = *areaLights[lp.entity];
            
            // 重新生成面光源的顶点数据
            ps[0] = {0, 0, 0};
            ps[1] = ps[0] + a.u;
            ps[2] = ps[1] + a.v;
            ps[3] = ps[0] + a.v;

            // 更新缓冲区数据
            glBindBuffer(GL_ARRAY_BUFFER, light.glVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vec3)*ps.size(), &ps[0]);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        else if (lp.type == T::DIRECTIONAL) {  // 平行光源
            // 待实现
        }
        else if (lp.type == T::SPOT) {  // 聚光灯
            // 待实现
        }
    }
}