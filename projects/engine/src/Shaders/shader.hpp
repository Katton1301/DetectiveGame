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
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class TShader
{
public:
    // Конструктор считывает и собирает шейдер
    TShader(const GLchar* vertexPath, const GLchar* fragmentPath);
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
