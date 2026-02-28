#include"pbr_builder.hpp"
#include"textures/cubemap.hpp"
#include"common/vertices.hpp"

TPBRBuilder::TPBRBuilder()
{
    create();
}

void TPBRBuilder::create()
{
    // pbr: set up projection and view matrices for capturing data onto the 6 cubemap face directions
    // ----------------------------------------------------------------------------------------------
    m_captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
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

void TPBRBuilder::convertEtoC( uint32_t hdrTextureId )
{
    TCubeMapBuilder defaultCubemapBuilder;
    defaultCubemapBuilder.setMinFilter(GL_LINEAR);
    defaultCubemapBuilder.setMipMapGeneration(false);
    m_envCubemap = defaultCubemapBuilder.MakeCubemap(m_frameBufferSize, m_frameBufferSize);
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
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, EnvCubemap(), 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(m_cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
    glBindTexture(GL_TEXTURE_CUBE_MAP, EnvCubemap());
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // pbr: create an irradiance cubemap, and re-scale capture FBO to irradiance scale.
    // --------------------------------------------------------------------------------
    defaultCubemapBuilder.setMinFilter(GL_LINEAR);
    m_irradianceMap = defaultCubemapBuilder.MakeCubemap(m_minBufferSize, m_minBufferSize);

    glBindFramebuffer(GL_FRAMEBUFFER, CuptureFBO());
    glBindRenderbuffer(GL_RENDERBUFFER, CuptureRBO());
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_minBufferSize, m_minBufferSize);

    // pbr: solve diffuse integral by convolution to create an irradiance (cube)map.
    // -----------------------------------------------------------------------------
    Shader().IrradianceShader()->Use();
    Shader().IrradianceShader()->setInt("environmentMap", 0);
    Shader().IrradianceShader()->setMat4("projection", CaptureProjection());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, EnvCubemap());

    glViewport(0, 0, m_minBufferSize, m_minBufferSize); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, CuptureFBO());
    for (uint32_t i = 0; i < 6; ++i)
    {
        Shader().IrradianceShader()->setMat4("view", CaptureViews()[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, IrradianceMap(), 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(m_cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // pbr: create a pre-filter cubemap, and re-scale capture FBO to pre-filter scale.
    // --------------------------------------------------------------------------------
    defaultCubemapBuilder.setMipMapGeneration(true);
    defaultCubemapBuilder.setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
    m_prefilterMap = defaultCubemapBuilder.MakeCubemap(128,128);

    // pbr: run a quasi monte-carlo simulation on the environment lighting to create a prefilter (cube)map.
    // ----------------------------------------------------------------------------------------------------
    Shader().PrefilterShader()->Use();
    Shader().PrefilterShader()->setInt("environmentMap", 0);
    Shader().PrefilterShader()->setMat4("projection", CaptureProjection());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, EnvCubemap());

    glBindFramebuffer(GL_FRAMEBUFFER, CuptureFBO());
    uint32_t maxMipLevels = 5;
    for (uint32_t mip = 0; mip < maxMipLevels; ++mip)
    {
        // reisze framebuffer according to mip-level size.
        uint32_t mipWidth = static_cast<uint32_t>(128 * std::pow(0.5, mip));
        uint32_t mipHeight = static_cast<uint32_t>(128 * std::pow(0.5, mip));
        glBindRenderbuffer(GL_RENDERBUFFER, CuptureRBO());
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(maxMipLevels - 1);
        Shader().PrefilterShader()->setFloat("roughness", roughness);
        for (uint32_t i = 0; i < 6; ++i)
        {
            Shader().PrefilterShader()->setMat4("view", CaptureViews()[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, PrefilterMap(), mip);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glBindVertexArray(m_cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    TQuadVertices quadVertices;
    // pbr: generate a 2D LUT from the BRDF equations used.
    // ----------------------------------------------------
    glGenTextures(1, &m_brdfLUTTexture);

    // pre-allocate enough memory for the LUT texture.
    glBindTexture(GL_TEXTURE_2D, BRDFLUTTexture());
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, m_frameBufferSize, m_frameBufferSize, 0, GL_RG, GL_FLOAT, 0);
    // be sure to set wrapping mode to GL_CLAMP_TO_EDGE
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
    glBindFramebuffer(GL_FRAMEBUFFER, CuptureFBO());
    glBindRenderbuffer(GL_RENDERBUFFER, CuptureRBO());
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_frameBufferSize, m_frameBufferSize);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, BRDFLUTTexture(), 0);

    glViewport(0, 0, m_frameBufferSize, m_frameBufferSize);
    Shader().BRDFShader()->Use();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindVertexArray(quadVertices.VAO());
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TPBRBuilder::setMinBufferSize(uint32_t minBufferSize )
{
    m_minBufferSize = minBufferSize;
}

void TPBRBuilder::setFrameBufferSize(uint32_t frameBufferSize )
{
    m_frameBufferSize = frameBufferSize;
}

void TPBRBuilder::updateMainShadersCameraMatrix( std::shared_ptr<ICamera> const & _cameraCptr )
{
    // initialize static shader uniforms before rendering
    // --------------------------------------------------
    Shader().BackShader()->Use();
    Shader().BackShader()->setInt("environmentMap", 0);
    Shader().BackShader()->setMat4("projection", *_cameraCptr->ProjectionCptr());
    Shader().MainShader()->Use();
    Shader().MainShader()->setMat4("projection", *_cameraCptr->ProjectionCptr());
    Shader().MainShader()->setMat4("view", _cameraCptr->ViewMatrix());
    Shader().MainShader()->setVec3("camPos", _cameraCptr->Position() );

    // bind pre-computed IBL data
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, IrradianceMap());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, PrefilterMap());
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, BRDFLUTTexture());
}

void TPBRBuilder::drawSphere( PBRTextures const& _textures, TSphereVertices const & _sphereVertices, glm::mat4 const & _model )
{
    // rusted iron
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, _textures.albedo);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, _textures.normal);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, _textures.metallic);
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, _textures.roughness);
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, _textures.ao);

    Shader().MainShader()->setMat4("model", _model);
    glm::mat3 normalMatrix  = glm::transpose(glm::inverse(glm::mat3(_model)));
    Shader().MainShader()->setMat3("normalMatrix", normalMatrix);
    glBindVertexArray(_sphereVertices.VAO());
    glDrawElements(GL_TRIANGLE_STRIP, _sphereVertices.Indices().size(), GL_UNSIGNED_INT, 0);
}

void TPBRBuilder::renderSkybox(glm::mat4 const & _view) const
{
    // render skybox (render as last to prevent overdraw)
    Shader().BackShader()->Use();
    Shader().BackShader()->setMat4("view", _view);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, EnvCubemap());
    //glBindTexture(GL_TEXTURE_CUBE_MAP, IrradianceMap()); // display irradiance map
    //glBindTexture(GL_TEXTURE_CUBE_MAP, PrefilterMap()); // display prefilter map

    glBindVertexArray(m_cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void TPBRBuilder::renderEtoC(glm::mat4 const & _view, uint32_t _hdrTextureId) const
{
    Shader().EtCShader()->Use();
    Shader().EtCShader()->setMat4("view", _view);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _hdrTextureId);
    glBindVertexArray(m_cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
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

uint32_t TPBRBuilder::EnvCubemap() const
{
    return m_envCubemap;
}

uint32_t TPBRBuilder::IrradianceMap() const
{
    return m_irradianceMap;
}

uint32_t TPBRBuilder::PrefilterMap() const
{
    return m_irradianceMap;
}

uint32_t TPBRBuilder::BRDFLUTTexture() const
{
    return m_brdfLUTTexture;
}

glm::mat4 const & TPBRBuilder::CaptureProjection() const
{
    return m_captureProjection;
}
std::vector<glm::mat4> const & TPBRBuilder::CaptureViews() const
{
    return m_captureViews;
}
