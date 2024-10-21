#include"pbr_builder.hpp"

TPBRBuilder::TPBRBuilder()
{
    create();
}

void TPBRBuilder::create()
{
    // pbr: set up projection and view matrices for capturing data onto the 6 cubemap face directions
    // ----------------------------------------------------------------------------------------------
    m_captureProjection = glm::perspective(90.0f, 1.0f, 0.1f, 10.0f);
    m_captureViews =
    {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };
}

void TPBRBuilder::setCubeVAO( uint32_t _cubeVAO )
{
    m_cubeVAO = _cubeVAO;
}

void TPBRBuilder::initCuptureBuffer()
{
    // pbr: setup framebuffer
    // ----------------------
    uint32_t captureRBO;
    glGenFramebuffers(1, &m_cuptureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, m_cuptureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_frameBufferSize, m_frameBufferSize);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
}

uint32_t TPBRBuilder::convertEtoC( uint32_t hdrTextureId )
{
    TCubeMapBuilder defaultCubemapBuilder;

    defaultCubemapBuilder.setMipMapGeneration(false);
    uint32_t envCubemap = defaultCubemapBuilder.MakeCubemap(m_frameBufferSize, m_frameBufferSize);
    // pbr: convert HDR equirectangular environment map to cubemap equivalent
    // ----------------------------------------------------------------------
    Shader().EtCShader()->Use();
    Shader().EtCShader()->setInt("equirectangularMap", 0);
    Shader().EtCShader()->setMat4("projection", CaptureProjection());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTextureId);

    glViewport(0, 0, m_frameBufferSize, m_frameBufferSize); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, CuptureFBO());
    for (uint32_t i = 0; i < 6; ++i)
    {
        Shader().EtCShader()->setMat4("view",m_captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(m_cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    return envCubemap;
}

PBRShader const & TPBRBuilder::Shader() const
{
    return m_shader;
}

uint32_t TPBRBuilder::CuptureFBO() const
{
    return m_cuptureFBO;
}

uint32_t TPBRBuilder::CuptureRBO() const
{
    return m_cuptureRBO;
}

glm::mat4 const & TPBRBuilder::CaptureProjection() const
{
    return m_captureProjection;
}
std::vector<glm::mat4> const & TPBRBuilder::CaptureViews() const
{
    return m_captureViews;
}
