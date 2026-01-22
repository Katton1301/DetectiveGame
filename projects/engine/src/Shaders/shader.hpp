#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#ifdef _WIN32
#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h>
#endif // _WIN32
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

class TShader
{
public:
    // Конструктор считывает и собирает шейдер
    TShader( std::string const & vertexShaderText, std::string const & fragmentShaderText);
    TShader() = delete;
    TShader( TShader const & ) = delete;
    // Использование программы
    void Use();

    void addGeometryShader(const GLchar* geometryPath);

    // Идентификатор программы
    void setVec3(const GLchar* vecName,  glm::vec3 const & _vec3);

    void setVec2(const GLchar* vecName,  glm::vec2 const & _vec2);

    void setFloat(const GLchar* vecName,  GLfloat v);

    void setInt(const GLchar* vecName,  GLuint v);

    void setMat3( GLchar const *vecName, glm::mat3 const & _mat3 );

    void setMat4( GLchar const *vecName, glm::mat4 const & _mat4 );

    GLuint Program() const
    {
        return m_program;
    }

private:
    GLuint m_program;
};

using HShader = TShader*;
