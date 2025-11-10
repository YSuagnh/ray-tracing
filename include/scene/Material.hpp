// 材质系统定义
// 提供了灵活的材质属性管理系统，支持多种类型的材质属性
#pragma once

#ifndef __NR_MATERIAL_HPP__
#define __NR_MATERIAL_HPP__

#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "geometry/vec.hpp"
#include "Texture.hpp"

#include <optional>
#include <algorithm>

namespace NRenderer
{
    using namespace std;

    // 材质属性结构体
    // 定义了材质的各种属性类型和值的封装
    struct Property
    {
        // 属性值包装器类
        // 为不同类型的属性值提供统一的接口
        class Wrapper
        {
        private:
            template<typename T>
            struct Base { T value = {}; };
        public:
            struct IntType : public Base<int> {};         // 整数类型
            struct FloatType : public Base<float>{};      // 浮点数类型
            struct RGBType : public Base<RGB> {};         // RGB颜色类型
            struct RGBAType: public Base<RGBA> {};        // RGBA颜色类型
            struct Vec3Type : public Base<Vec3> {};       // 3D向量类型
            struct Vec4Type : public Base<Vec4> {};       // 4D向量类型
            struct TextureIdType : public Base<Handle> {}; // 纹理ID类型
        };

        // 属性类型枚举
        enum class Type
        {
            INT = 0x0,      // 整数类型
            FLOAT,          // 浮点数类型
            RGB,            // RGB颜色
            RGBA,           // RGBA颜色
            VEC3,           // 3D向量
            VEC4,           // 4D向量
            TEXTURE_ID      // 纹理ID
        };

    #define Types Wrapper::IntType, Wrapper::FloatType, Wrapper::RGBType, Wrapper::RGBAType, Wrapper::Vec3Type, Wrapper::Vec4Type, Wrapper::TextureIdType
        using ValueType = variant<Types>;  // 使用variant存储不同类型的值
    #undef Types

        string key;         // 属性名称
        Type type;         // 属性类型
        ValueType valueWrapper;  // 属性值

        // 各种类型属性的构造函数
        Property(const string& key, Wrapper::IntType intWrapper)
            : key               (key)
            , type              (Type::INT)
            , valueWrapper      (intWrapper)
        {}
        Property(const string& key, Wrapper::FloatType floatWrapper)
            : key               (key)
            , type              (Type::FLOAT)
            , valueWrapper      (floatWrapper)
        {}
        Property(const string& key, Wrapper::RGBType rgbWrapper)
            : key               (key)
            , type              (Type::RGB)
            , valueWrapper      (rgbWrapper)
        {}
        Property(const string& key, Wrapper::RGBAType rgbaWrapper)
            : key               (key)
            , type              (Type::RGBA)
            , valueWrapper      (rgbaWrapper)
        {}
        Property(const string& key, Wrapper::Vec3Type vec3Wrapper)
            : key               (key)
            , type              (Type::VEC3)
            , valueWrapper      (vec3Wrapper)
        {}
        Property(const string& key, Wrapper::Vec4Type vec4Wrapper)
            : key               (key)
            , type              (Type::VEC4)
            , valueWrapper      (vec4Wrapper)
        {}
        Property(const string& key, Wrapper::TextureIdType texIdWrapper)
            : key               (key)
            , type              (Type::TEXTURE_ID)
            , valueWrapper      (texIdWrapper)
        {}
    };

    // 材质结构体
    // 管理一组材质属性，提供属性的添加、删除和查询功能
    struct Material
    {
        Material() = default;

    public:
        unsigned int type = 0;           // 材质类型
        vector<Property> properties;     // 材质属性列表

        // 检查是否存在指定键名的属性
        inline
        bool hasProperty(const string& key) {
            auto it = find_if(properties.begin(),
                properties.end(),
                [&key](const Property& p1) -> bool {
                    return p1.key == key;
                }
            );
            return it != properties.end();
        }

        // 获取指定键名的属性值
        // 返回optional类型，如果属性不存在则返回nullopt
        template<typename T>
        auto getProperty(const string& key) -> optional<T> {
            auto it = find_if(properties.begin(),
                properties.end(),
                [&key](const Property& p1) -> bool {
                    return p1.key == key;
                }
            );
            if (it == properties.end()) return nullopt;
            else {
                auto prop = *it;
                auto v = std::get<T>(prop.valueWrapper);
                return optional<T>(v);
            }
        }

        // 注册新的属性
        // 如果属性已存在则返回false
        template<typename T>
        bool registerProperty(const string& key, T value) {
            using PropType = Property::Type;
            bool fail = hasProperty(key);
            if (!fail) {
                properties.push_back(Property{key, value});
            }
            return !fail;
        }

        // 注册已构造的属性对象
        bool registerProperty(const Property& prop) {
            bool fail = hasProperty(prop.key);
            if (!fail) {
                properties.push_back(prop);
            }
            return !fail;
        }

        // 获取属性数量
        auto getPropertiesNum() const {
            return properties.size();
        }

        // 根据键名删除属性
        void removeProperty(const string& key) {
            auto it = find_if(properties.begin(),
                properties.end(),
                [&key](const Property& p1) -> bool {
                    return p1.key == key;
                }
            );
            if (it != properties.end()) {
                properties.erase(it);
            }
        }
        
        // 根据索引删除属性
        void removeProperty(Index i) {
            if (i < properties.size()) {
                properties.erase(properties.begin() + i);
            }
        }
    };
    using SharedMaterial = shared_ptr<Material>;  // 材质的智能指针类型
}

#endif