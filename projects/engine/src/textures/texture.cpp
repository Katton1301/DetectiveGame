#include "./texture.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include<iostream>

TTextureBuilder::TTextureBuilder(
        std::string const & path,
        GLint wrap_s/* = GL_CLAMP_TO_EDGE*/,
        GLint wrap_t/* = GL_CLAMP_TO_EDGE*/,
        GLint min_filter/* = GL_LINEAR_MIPMAP_LINEAR*/,
        GLint mag_filter/* = GL_LINEAR*/)
    : m_path(path)
    , m_wrap_s(wrap_s)
    , m_wrap_t(wrap_t)
    , m_min_filter(min_filter)
    , m_mag_filter(mag_filter)
{
}

void TTextureBuilder::changeWrapS(GLint _wrap_s)
{
    m_wrap_s = _wrap_s;
}

void TTextureBuilder::changeWrapT(GLint _wrap_t)
{
    m_wrap_t = _wrap_t;
}

void TTextureBuilder::changeMinFilter(GLint _min_filter)
{
    m_min_filter = _min_filter;
}

void TTextureBuilder::changeMagFilter(GLint _mag_filter)
{
    m_mag_filter = _mag_filter;
}

void TTextureBuilder::changeMipMapGeneration(bool _mipmapGeneration)
{
    m_mipmapGeneration = _mipmapGeneration;
}

uint32_t TTextureBuilder::MakeTexture(std::string const & filename)
{
    stbi_set_flip_vertically_on_load(true);
    int width, height;
    int nrComponents;
    unsigned char *data = stbi_load((m_path + filename).c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format = 3;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glGenTextures(1, &m_id);
        glBindTexture(GL_TEXTURE_2D, m_id);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data); // note how we specify the texture's data value to be float
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
    return m_id;
}

GLint TTextureBuilder::LastBuiltID() const
{
    return m_id;
}

