#pragma once
#ifndef __SHADER_CREATOR_HPP__
#define __SHADER_CREATOR_HPP__

#include "Shader.hpp"
#include "Phong.hpp"
#include "PBR.hpp"
#include "Lambertian.hpp"

namespace RayTracer
{
    /**
     * 着色器创建器
     * 根据材质类型创建相应的着色器实例
     */
    class ShaderCreator
    {
    public:
        ShaderCreator() = default;
        
        /**
         * 根据材质类型创建着色器
         * @param material 材质对象
         * @param t 纹理缓冲区
         * @return 着色器共享指针
         */
        SharedShader create(Material& material, vector<Texture>& t) {
            SharedShader shader{nullptr};
            switch (material.type)
            {
            case 0:
                shader = make_shared<Phong>(material, t);
                break;
			case 1:
                shader = make_shared<Phong>(material, t);
				break;
            default:
                shader = make_shared<Phong>(material, t);
                break;
            }
            return shader;
        }
    };
}

#endif