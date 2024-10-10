#pragma once
#include"texture.hpp"

class TCubeMapBuilder : public TTextureBuilder
{
public:
    TCubeMapBuilder();
    ~TCubeMapBuilder() = default;

    uint32_t MakeCubemap(uint32_t width, uint32_t height) const;

    void setInternalFormat( uint32_t _internalFormat );
    void setFormat( uint32_t _format );
    void setType( uint32_t _type );
    void setWrapR( uint32_t _wrap_r );

private:
    uint32_t m_wrap_r = GL_CLAMP_TO_EDGE;
    uint32_t m_width = 0;
    uint32_t m_height = 0;
    uint32_t m_internalFormat = GL_RGB16F;
    uint32_t m_format = GL_RGB;
    uint32_t m_type = GL_FLOAT;
};
