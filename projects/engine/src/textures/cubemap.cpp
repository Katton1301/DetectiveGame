#include"cubemap.hpp"
TCubeMapBuilder::TCubeMapBuilder()
    : TTextureBuilder()
{
}

uint32_t TCubeMapBuilder::MakeCubemap(uint32_t width, uint32_t height) const
{
    uint32_t id = 0;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);
    for (unsigned int i = 0; i < 6; ++i)
    {
        // обратите внимание, что каждая грань использует
        // 16битный формат с плавающей точкой
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, m_internalFormat, width, height, 0, m_format, m_type, nullptr);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, m_wrap_s);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, m_wrap_t);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, m_wrap_r);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, m_min_filter);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, m_mag_filter);
    if(m_mipmapGeneration)
    {
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    }

    return id;
}

void  TCubeMapBuilder::setInternalFormat( uint32_t _internalFormat )
{
    m_internalFormat = _internalFormat;
}

void  TCubeMapBuilder::setFormat( uint32_t _format )
{
    m_format = _format;
}

void  TCubeMapBuilder::setType( uint32_t _type )
{
    m_type = _type;
}

void TCubeMapBuilder::setWrapR( uint32_t _wrap_r )
{
    m_wrap_r = _wrap_r;
}
