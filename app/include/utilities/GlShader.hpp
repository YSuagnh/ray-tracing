#pragma once
#ifndef __NR_GL_SHADER_HPP__
#define __NR_GL_SHADER_HPP__

// OpenGL着色器工具类头文件
// 提供了OpenGL着色器程序的创建和管理功能

#include <iostream>
#include "geometry/vec.hpp"
#include "glad/glad.h"

namespace NRenderer
{
    using namespace std;

    // OpenGL着色器工具类
    // 封装了着色器程序的创建、编译、链接和使用
    class GlShader
    {
    public:
        // OpenGL着色器程序ID类型
        using GlShaderId = unsigned int;
    private:
        GlShaderId shaderId;  // 着色器程序ID

        // 检查着色器编译错误
        // shader: 着色器ID
        // type: 着色器类型（"VERTEX"、"FRAGMENT"或"PROGRAM"）
        inline
        void checkShaderCompileError(unsigned int shader, string type) {
            GLint success;
            GLchar infoLog[1024];
            if (type != "PROGRAM")
            {
                glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
                if (!success)
                {
                    glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                    std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
                            << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
                }
            }
            else
            {
                glGetProgramiv(shader, GL_LINK_STATUS, &success);
                if (!success)
                {
                    glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                    std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
                            << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
                }
            }
        }
    public:
        // 构造函数
        // vShaderSource: 顶点着色器源代码
        // fShaderSource: 片段着色器源代码
        GlShader(const char* vShaderSource, const char* fShaderSource) {
            // 创建并编译顶点着色器
            unsigned int vShader, fShader;
            vShader = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vShader, 1, &vShaderSource, NULL);
            glCompileShader(vShader);
            checkShaderCompileError(vShader, "VERTEX");

            // 创建并编译片段着色器
            fShader = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fShader, 1, &fShaderSource, NULL);
            glCompileShader(fShader);
            checkShaderCompileError(fShader, "FRAGMENT");

            // 创建着色器程序并链接着色器
            shaderId = glCreateProgram();
            glAttachShader(shaderId, vShader);
            glAttachShader(shaderId, fShader);
            glLinkProgram(shaderId);
            // 删除着色器对象
            glDeleteShader(vShader);
            glDeleteShader(fShader);
        }

        // 使用着色器程序
        void use() {
            glUseProgram(shaderId);
        }
    
        // 设置4x4矩阵uniform变量
        // name: uniform变量名
        // m: 矩阵值
        void setMat4x4(const char* name, const Mat4x4& m) {
            auto loc = glGetUniformLocation(shaderId, name);
            glUniformMatrix4fv(loc, 1, GL_FALSE, &m[0][0]);
        }

        // 设置3维向量uniform变量
        // name: uniform变量名
        // v: 向量值
        void setVec3(const char* name, const Vec3& v) {
            auto loc = glGetUniformLocation(shaderId, name);
            glUniform3f(loc, v.x, v.y, v.z);
        }

        // 设置4维向量uniform变量
        // name: uniform变量名
        // v: 向量值
        void setVec4(const char* name, const Vec4& v) {
            auto loc = glGetUniformLocation(shaderId, name);
            glUniform4f(loc, v.x, v.y, v.z, v.w);
        }
    
        // 设置整数uniform变量
        // name: uniform变量名
        // i: 整数值
        void setInt(const char* name, int i) {
            auto loc = glGetUniformLocation(shaderId, name);
            glUniform1i(loc, i);
        }

        // 设置浮点数uniform变量
        // name: uniform变量名
        // f: 浮点数值
        void setFloat(const char* name, float f) {
            auto loc = glGetUniformLocation(shaderId, name);
            glUniform1f(loc,  f);
        }
    };
}

#endif