#ifndef __NR_MATERIAL_TEMPLATES_HPP__
#define __NR_MATERIAL_TEMPLATES_HPP__

// 材质模板头文件
// 定义了预设材质模板的管理类

#include <unordered_map>

#include "scene/Material.hpp"
#include "common/macros.hpp"

namespace NRenderer
{
    // 材质模板类
    // 管理和提供预设的材质模板
    class DLL_EXPORT MaterialTemplates
    {
    public:
        // 模板结构体
        // 定义单个材质模板的属性
        struct Template
        {
            unsigned int index;      // 模板索引
            string key;             // 模板名称
            vector<Property> props;  // 材质属性列表
        };

        static unordered_map<int, Template> templates;  // 材质模板映射表

    private:
        // 模板初始化结构体
        // 负责创建和配置预设材质模板
        struct InitTemplates
        {
            // 添加单个属性到模板
            // p: 目标模板
            // arg: 要添加的属性
            template<typename T>
            void PushProp(Template& p, T&& arg) {
                p.props.push_back(arg);
            }

            // 添加多个属性到模板
            // p: 目标模板
            // arg1: 第一个属性
            // args: 其余属性
            template<typename T, typename ...Args>
            void PushProp(Template& p, T&& arg1, Args&& ...args) {
                p.props.push_back(arg1);
                PushProp(p, args...);
            }

            // 添加新的材质模板
            // templates: 模板映射表
            // index: 模板索引
            // name: 模板名称
            // args: 模板属性列表
            template<typename ...Args>
            void Add(unordered_map<int, Template>& templates, unsigned int index, const string& name, Args&& ...args) {
                templates[index] = {
                    index, name, {}
                };
                PushProp(templates[index], args...);
            }

            // 构造函数
            // 初始化预设材质模板
            InitTemplates(unordered_map<int, Template>& templates) {
                using PW = Property::Wrapper;
                // Lambertian漫反射材质
                Add(templates, 0, string("Lambertian"),
                    Property("diffuseColor", PW::RGBType{}),      // 漫反射颜色
                    Property("diffuseMap", PW::TextureIdType{})   // 漫反射贴图
                );
                // Phong材质
                Add(templates, 1, string("Phong"),
                    Property("diffuseColor", PW::RGBType{}),      // 漫反射颜色
                    Property("diffuseMap", PW::TextureIdType{}),  // 漫反射贴图
                    Property("specularColor", PW::RGBType{}),     // 高光颜色
                    Property("specularEx", PW::FloatType{})       // 高光指数
                );
                // 电介质材质
                Add(templates, 2, string("Dielectric"),
                    Property("ior", PW::FloatType{float(1.0)}),   // 折射率
                    Property("absorbed", PW::RGBType{RGB{1, 1, 1}})  // 吸收率
                );
                // 导体材质
                Add(templates, 3, string("Conductor"),
                    Property("reflect", PW::RGBType{RGB{1, 1, 1}})  // 反射率
                );
                // 塑料材质
                Add(templates, 4, string("Plastic"),
                    Property("diffuseColor", PW::RGBType{}),      // 漫反射颜色
                    Property("specularColor", PW::RGBType{}),     // 高光颜色
                    Property("refractIndex", PW::FloatType{})     // 折射率
                );
                
            }
        };
        static InitTemplates initTemplates;  // 模板初始化器实例
    };
}

#endif