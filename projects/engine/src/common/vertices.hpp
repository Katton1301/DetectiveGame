#pragma once
#include"vertices_interface.hpp"

#ifdef _WIN32
#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h>
#endif // _WIN32

#include<vector>

class TVertices : public IVertices
{
public:
    TVertices();
    ~TVertices() = default;
    float& operator[](uint32_t index) override;
    float at(uint32_t index) const override;
    uint32_t VAO() const override;
protected:
    uint32_t VBO() const;

protected:
    std::vector<float> m_vertices{};
    uint32_t m_vao = 0;
    uint32_t m_vbo = 0;
};

class TQuadVertices : public TVertices
{
public:
    TQuadVertices();
    ~TQuadVertices() = default;
private:
    void Init() override;
};

class TCubeVertices : public TVertices
{
public:
    TCubeVertices();
    ~TCubeVertices() = default;
private:
    void Init() override;
};


