#pragma once
#ifndef __HIT_RECORD_HPP__
#define __HIT_RECORD_HPP__

#include <optional>

#include "geometry/vec.hpp"

namespace SimplePathTracer
{
    using namespace NRenderer;
    using namespace std;
    
    /**
     * 光线与物体相交记录的基础结构
     * 记录光线与物体相交时的详细信息
     */
    struct HitRecordBase
    {
        float t;           // 光线参数t，表示相交点距离光线起点的距离
        Vec3 hitPoint;     // 相交点的世界坐标
        Vec3 normal;       // 相交点处的法向量
        Handle material;   // 相交物体的材质句柄
    };
    
    // 使用optional包装，表示可能没有相交
    using HitRecord = optional<HitRecordBase>;
    
    /**
     * 创建未相交记录
     * @return 表示未相交的HitRecord
     */
    inline
    HitRecord getMissRecord() {
        return nullopt;
    }

    /**
     * 创建相交记录
     * @param t 光线参数
     * @param hitPoint 相交点
     * @param normal 法向量
     * @param material 材质句柄
     * @return 包含相交信息的HitRecord
     */
    inline
    HitRecord getHitRecord(float t, const Vec3& hitPoint, const Vec3& normal, Handle material) {
        return make_optional<HitRecordBase>(t, hitPoint, normal, material);
    }
}

#endif