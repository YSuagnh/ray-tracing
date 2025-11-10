#pragma once
#ifndef __HIT_RECORD_HPP__
#define __HIT_RECORD_HPP__

// 相交记录头文件
// 定义了光线与物体相交的记录结构

#include <optional>

#include "geometry/vec.hpp"

namespace RayCast
{
    using namespace NRenderer;
    using namespace std;

    // 相交记录基础结构
    // 存储光线与物体相交的详细信息
    struct HitRecordBase
    {
        float t;         // 相交距离
        Vec3 hitPoint;   // 相交点
        Vec3 normal;     // 相交点法线
        Handle material; // 相交物体的材质
    };

    // 相交记录类型
    // 使用optional包装，可以表示是否发生相交
    using HitRecord = optional<HitRecordBase>;

    // 获取未相交记录
    // 返回一个表示未相交的记录
    inline
    HitRecord getMissRecord() {
        return nullopt;
    }

    // 创建相交记录
    // t: 相交距离
    // hitPoint: 相交点
    // normal: 相交点法线
    // material: 相交物体的材质
    // 返回一个包含相交信息的记录
    inline
    HitRecord getHitRecord(float t, const Vec3& hitPoint, const Vec3& normal, Handle material) {
        return make_optional<HitRecordBase>(t, hitPoint, normal, material);
    }
}

#endif