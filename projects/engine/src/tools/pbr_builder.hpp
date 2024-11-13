#pragma once
#include"shaders/pbr_shader.hpp"
#include"textures/texture.hpp"
#include"common/vertices.hpp"
#include<vector>

class TPBRBuilder
{
public:
    TPBRBuilder();
    ~TPBRBuilder() = default;
    PBRShader const & Shader() const;

    void setCubeVAO( uint32_t _cubeVAO );

    void initCuptureBuffer();
    void convertEtoC(uint32_t hdrTextureId);
    void setProjection( glm::mat4 const & _projection );
    void setMinBufferSize(uint32_t minBufferSize );
    void setFrameBufferSize(uint32_t frameBufferSize );
    void initMainShadersEnvs( glm::mat4 const & _view, glm::vec3 const & _camPos );

    void drawSphere( PBRTextures const& _textures, TSphereVertices const & _sphereVertices, glm::mat4 const & _model );
    void renderSkybox(glm::mat4 const & _view) const;
    void renderEtoC(glm::mat4 const & _view, uint32_t _hdrTextureId) const;

    uint32_t CuptureFBO() const;
    uint32_t CuptureRBO() const;
    uint32_t EnvCubemap() const;
    uint32_t IrradianceMap() const;
    uint32_t PrefilterMap() const;
    uint32_t BRDFLUTTexture() const;
    glm::mat4 const & CaptureProjection() const;
    std::vector<glm::mat4> const & CaptureViews() const;

protected:
    void create();
protected:
    PBRShader m_shader{};
    uint32_t m_cuptureFBO = 0;
    uint32_t m_cuptureRBO = 0;
    glm::mat4 m_captureProjection{};
    std::vector<glm::mat4> m_captureViews{};
    uint32_t m_frameBufferSize = 512;
    uint32_t m_minBufferSize = 32;
    uint32_t m_cubeVAO;
    uint32_t m_envCubemap;
    uint32_t m_irradianceMap;
    uint32_t m_prefilterMap;
    uint32_t m_brdfLUTTexture;

};
