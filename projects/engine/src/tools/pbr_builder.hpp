#pragma once
#include"shaders/pbr_shader.hpp"
#include"textures/cubemap.hpp"
#include<vector>

class TPBRBuilder
{
public:
    TPBRBuilder();
    ~TPBRBuilder() = default;
    PBRShader const & Shader() const;

    void setCubeVAO( uint32_t _cubeVAO );

    void initCuptureBuffer();
    uint32_t convertEtoC(uint32_t hdrTextureId);

    uint32_t CuptureFBO() const;
    uint32_t CuptureRBO() const;
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
    uint32_t m_cubeVAO;

};
