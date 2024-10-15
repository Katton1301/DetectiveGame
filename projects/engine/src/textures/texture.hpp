#pragma once
#ifdef _WIN32
#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h>
#endif // _WIN32

#include<string>
#include<vector>

struct PBRTextures
{
    uint32_t albedo;
    uint32_t normal;
    uint32_t metallic;
    uint32_t roughness;
    uint32_t ao;
};

class TTextureBuilder
{
public:
    TTextureBuilder();
    TTextureBuilder( GLint wrap_s, GLint wrap_t, GLint min_filter, GLint mag_filter );

    void setWrapS(GLint _wrap_s);

    void setWrapT(GLint _wrap_t);

    void setMinFilter(GLint _min_filter);

    void setMagFilter(GLint _mag_filter);

    void setMipMapGeneration(bool _mipmapGeneration);

    uint32_t MakeTexture(std::string const & path, std::string const & filename) const;
    PBRTextures MakePBRTextures(std::string const & path) const;
    PBRTextures MakePBRTextures(
            std::string const & path,
            std::string const & albedo,
            std::string const & normal,
            std::string const & metallic,
            std::string const & roughness,
            std::string const & ao) const;
protected:
    GLint m_wrap_s = 0;
    GLint m_wrap_t = 0;
    GLint m_min_filter = 0;
    GLint m_mag_filter = 0;
    bool m_mipmapGeneration = true;
};
