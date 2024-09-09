#include"shader.hpp"

TShader::TShader(const GLchar* vertexPath, const GLchar* fragmentPath)
{
    // 1. Получаем исходный код шейдера из filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // Удостоверимся, что ifstream объекты могут выкидывать исключения
    vShaderFile.exceptions(std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::badbit);
    try
    {
        // Открываем файлы
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        // Считываем данные в потоки
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // Закрываем файлы
        vShaderFile.close();
        fShaderFile.close();
        // Преобразовываем потоки в массив GLchar
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch(std::ifstream::failure & e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    const GLchar* vShaderCode = vertexCode.c_str();
    const GLchar* fShaderCode = fragmentCode.c_str();

    // 2. Сборка шейдеров
    GLuint vertex, fragment;
    GLint success;
    GLchar infoLog[512];

    // Вершинный шейдер
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    // Если есть ошибки - вывести их
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    };

    // Фрагментный шейдер
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    // Если есть ошибки - вывести их
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    };

    m_program = glCreateProgram();
    glAttachShader(m_program, vertex);
    glAttachShader(m_program, fragment);
    glLinkProgram(m_program);
    //Если есть ошибки - вывести их
    glGetProgramiv(m_program, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(m_program, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // Удаляем шейдеры, поскольку они уже в программу и нам больше не нужны.
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void TShader::addGeometryShader(const GLchar* geometryPath)
{
    std::string geometryCode;
    std::ifstream gShaderFile;
    gShaderFile.exceptions(std::ifstream::badbit);
    try
    {
        // Открываем файлы
        gShaderFile.open(geometryPath);
        std::stringstream gShaderStream;
        // Считываем данные в потоки
        gShaderStream << gShaderFile.rdbuf();
        // Закрываем файлы
        gShaderFile.close();
        // Преобразовываем потоки в массив GLchar
        geometryCode = gShaderStream.str();
    }
    catch(std::ifstream::failure & e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    const GLchar* gShaderCode = geometryCode.c_str();

    GLuint geometry;
    GLint success;
    GLchar infoLog[512];

    // Геометрический шейдер
    geometry = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geometry, 1, &gShaderCode, NULL);
    glCompileShader(geometry);
    // Если есть ошибки - вывести их
    glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(geometry, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;
    };

    glAttachShader(m_program, geometry);
    glLinkProgram(m_program);
    //Если есть ошибки - вывести их
    glGetProgramiv(m_program, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(m_program, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // Удаляем шейдеры, поскольку они уже в программу и нам больше не нужны.
    glDeleteShader(geometry);
}

void TShader::Use()
{
    glUseProgram(m_program);
}

void TShader::setVec2(const GLchar* vecName,  glm::vec2 const & _vec2)
{
    GLint uniformId = glGetUniformLocation(Program(), vecName);
    glUniform2f(uniformId, _vec2.x, _vec2.y);
}



void TShader::setVec3(const GLchar* vecName,  glm::vec3 const & _vec3)
{
    GLint uniformId = glGetUniformLocation(Program(), vecName);
    glUniform3f(uniformId, _vec3.x, _vec3.y, _vec3.z);
}

void TShader::setInt( GLchar const *vecName,  GLuint v)
{
    GLint uniformId = glGetUniformLocation(Program(), vecName);
    glUniform1i(uniformId, v);
}

void TShader::setFloat( GLchar const *vecName,  GLfloat v)
{
    GLint uniformId = glGetUniformLocation(Program(), vecName);
    glUniform1f(uniformId, v);
}

void TShader::setMat3( GLchar const *vecName, glm::mat3 const & _mat3 )
{
    GLint uniformId = glGetUniformLocation(Program(), vecName);
    glUniformMatrix3fv(uniformId, 1, GL_FALSE, glm::value_ptr(_mat3));
}

void TShader::setMat4( GLchar const *vecName, glm::mat4 const & _mat4 )
{
    GLint uniformId = glGetUniformLocation(Program(), vecName);
    glUniformMatrix4fv(uniformId, 1, GL_FALSE, glm::value_ptr(_mat4));
}



