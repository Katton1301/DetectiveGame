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

class TShader
{
public:
    // Конструктор считывает и собирает шейдер
    TShader(const GLchar* vertexPath, const GLchar* fragmentPath);
    // Использование программы
    void Use();

    void addGeometryShader(const GLchar* geometryPath);

    // Идентификатор программы
    void setVec3(const GLchar* vecName,  GLfloat x, GLfloat y, GLfloat z);

    void setVec2(const GLchar* vecName,  GLfloat x, GLfloat y);

    void setFloat(const GLchar* vecName,  GLfloat v);

    void setInt(const GLchar* vecName,  GLuint v);

    GLuint Program() const
    {
        return m_program;
    }
private:
    GLuint m_program;
};
