#pragma once
#ifdef _WIN32
#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h>
#endif // _WIN32

#include<string>

class TTextureBuilder
{
public:
    TTextureBuilder() = delete;
    TTextureBuilder(std::string const & path,
             GLint wrap_s = GL_CLAMP_TO_EDGE,
             GLint wrap_t = GL_CLAMP_TO_EDGE,
             GLint min_filter = GL_LINEAR_MIPMAP_LINEAR,
             GLint mag_filter = GL_LINEAR);

    void changeWrapS(GLint _wrap_s);

    void changeWrapT(GLint _wrap_t);

    void changeMinFilter(GLint _min_filter);

    void changeMagFilter(GLint _mag_filter);

    void changeMipMapGeneration(bool _mipmapGeneration);

    uint32_t MakeTexture(std::string const & filename);

    GLint LastBuiltID() const;
private:
    std::string m_path{};
    GLint m_wrap_s = 0;
    GLint m_wrap_t = 0;
    GLint m_min_filter = 0;
    GLint m_mag_filter = 0;
    bool m_mipmapGeneration = true;
    uint32_t m_id = 0;
};
