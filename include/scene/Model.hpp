// 模型系统定义
// 定义了场景中的各种几何实体和模型结构
#pragma once
#ifndef __NR_MODEL_HPP__
#define __NR_MODEL_HPP__

#include <string>
#include <vector>

#include "geometry/vec.hpp"

#include "Material.hpp"
#include "common/macros.hpp"

namespace NRenderer
{
    using namespace std;

    // 实体基类
    // 所有几何实体的基类，包含材质信息
    struct Entity {
        Handle material;    // 材质句柄
    };
    SHARE(Entity);

    // 球体实体
    // 定义了一个球体的位置、方向和半径
    struct Sphere : public Entity
    {
        Vec3 direction = {0, 0, 1};  // 球体方向
        Vec3 position = {0, 0, 0};   // 球心位置
        float radius = { 0 };        // 球体半径
    };
    SHARE(Sphere);
    
    // 三角形实体
    // 定义了一个三角形的三个顶点和法线
    struct Triangle : public Entity
    {
        union {
            struct {
                Vec3 v1;    // 第一个顶点
                Vec3 v2;    // 第二个顶点
                Vec3 v3;    // 第三个顶点
            };
            Vec3 v[3];     // 顶点数组形式
        };
        Vec3 normal;       // 三角形法线

        // 默认构造函数
        Triangle()
            : v1            ()
            , v2            ()
            , v3            ()
            , normal         (0, 0, 1)
        {}
    };
    SHARE(Triangle);

    // 平面实体
    // 定义了一个平面的位置、法线和两个基向量
    struct Plane : public Entity
    {
        Vec3 normal = {0, 0, 1};  // 平面法线
        Vec3 position = {};       // 平面上的一点
        Vec3 u = {};             // 平面第一个基向量
        Vec3 v = {};             // 平面第二个基向量
    };
    SHARE(Plane);

    // 网格实体
    // 定义了一个由多个三角形组成的网格
    struct Mesh : public Entity
    {
        vector<Vec3> normals;         // 法线列表
        vector<Vec3> positions;       // 顶点位置列表
        vector<Vec2> uvs;             // UV坐标列表
        vector<Index> normalIndices;   // 法线索引
        vector<Index> positionIndices; // 顶点索引
        vector<Index> uvIndices;       // UV索引

        // 检查是否有法线数据
        bool hasNormal() const {
            return normals.size() != 0;
        }

        // 检查是否有UV坐标数据
        bool hasUv() const {
            return uvs.size() != 0;
        }
    };
    SHARE(Mesh);

    // 场景节点
    // 表示场景图中的一个节点，可以是不同类型的几何实体
    struct Node
    {
        // 节点类型枚举
        enum class Type
        {
            SPHERE = 0x0,    // 球体
            TRIANGLE = 0X1,   // 三角形
            PLANE = 0X2,      // 平面
            MESH = 0X3        // 网格
        };
        Type type = Type::SPHERE;  // 节点类型
        Index entity;              // 实体索引
        Index model;               // 所属模型索引
    };
    SHARE(Node);

    // 模型结构
    // 定义了一个完整的3D模型，包含多个节点和变换信息
    struct Model {
        vector<Index> nodes;           // 节点列表
        Vec3 translation = {0, 0, 0};  // 平移向量
        Vec3 scale = {1, 1, 1};       // 缩放向量
    };
    SHARE(Model);
}

#endif