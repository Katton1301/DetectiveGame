#include "./texture.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include<iostream>

TTextureBuilder::TTextureBuilder()
    : TTextureBuilder(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR)
{

}

TTextureBuilder::TTextureBuilder( GLint wrap_s, GLint wrap_t, GLint min_filter, GLint mag_filter)
    : m_wrap_s(wrap_s)
    , m_wrap_t(wrap_t)
    , m_min_filter(min_filter)
    , m_mag_filter(mag_filter)
{
}

void TTextureBuilder::setWrapS(GLint _wrap_s)
{
    m_wrap_s = _wrap_s;
}

void TTextureBuilder::setWrapT(GLint _wrap_t)
{
    m_wrap_t = _wrap_t;
}

void TTextureBuilder::setMinFilter(GLint _min_filter)
{
    m_min_filter = _min_filter;
}

void TTextureBuilder::setMagFilter(GLint _mag_filter)
{
    m_mag_filter = _mag_filter;
}

void TTextureBuilder::setMipMapGeneration(bool _mipmapGeneration)
{
    m_mipmapGeneration = _mipmapGeneration;
}

uint32_t TTextureBuilder::MakeTexture(std::string const & path, std::string const & filename) const
{

    uint32_t id = 0;
    stbi_set_flip_vertically_on_load(true);
    int width, height;
    int nrComponents;
    float *data = stbi_loadf((path + "/" + filename).c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format = 3;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_FLOAT, data); // note how we specify the texture's data value to be float
        if(m_mipmapGeneration)
        {
            glGenerateMipmap(GL_TEXTURE_2D);
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_wrap_s);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_wrap_t);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_min_filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_mag_filter);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Failed to load HDR image." << std::endl;
    }
    return id;
}

PBRTextures TTextureBuilder::MakePBRTextures(std::string const & path) const
{
    PBRTextures pbr;
    pbr.albedo = MakeTexture(path, "albedo.png");
    pbr.normal = MakeTexture(path, "normal.png");
    pbr.metallic = MakeTexture(path, "metallic.png");
    pbr.roughness = MakeTexture(path, "roughness.png");
    pbr.ao = MakeTexture(path, "ao.png");
    return pbr;
}

PBRTextures TTextureBuilder::MakePBRTextures(
        std::string const & path,
        std::string const & albedo,
        std::string const & normal,
        std::string const & metallic,
        std::string const & roughness,
        std::string const & ao) const
{
    PBRTextures pbr;
    pbr.albedo = MakeTexture(path, albedo);
    pbr.normal = MakeTexture(path, normal);
    pbr.metallic = MakeTexture(path, metallic);
    pbr.roughness = MakeTexture(path, roughness);
    pbr.ao = MakeTexture(path, ao);
    return pbr;
}
