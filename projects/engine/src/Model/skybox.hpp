#pragma once
#include <shaders/shader_collection.hpp>
#include <textures/texture.hpp>
#include <common/vertices.hpp>
#include <camera/camera.hpp>
#include <vector>


class TSkyBox
{
public:
    TSkyBox();
    virtual ~TSkyBox() = default;

    void setMinBufferSize(uint32_t minBufferSize );
    void setFrameBufferSize(uint32_t frameBufferSize );
    void initCuptureBuffer();
    void setupHDRTexture(uint32_t hdrTextureId);

    void Update( );

    void renderSkybox(glm::mat4 const & _view) const;
    void renderEtoC(glm::mat4 const & _view, uint32_t _hdrTextureId) const;

    void Draw( ) const;

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
