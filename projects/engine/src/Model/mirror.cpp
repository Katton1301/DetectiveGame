#include "mirror.hpp"
#include <iostream>
#include <shaders/shader.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

TMirror::TMirror() : m_width(1.0f), m_height(1.0f),
                     m_position(0.0f, 0.0f, 0.0f), m_rotation(0.0f, 0.0f, 0.0f),
                     m_scale(1.0f, 1.0f, 1.0f),
                     m_mirrorVAO(0), m_mirrorVBO(0), m_mirrorEBO(0),
                     m_reflectionFBO(0), m_reflectionTexture(0), m_reflectionDepthBuffer(0),
                     m_shader(nullptr), m_scene(nullptr),
                     m_initialized(false)
{
    m_camera = std::make_shared<TStaticCamera>(m_position, static_cast<GLfloat>(m_width) / m_height);
    initMirrorGeometry();
    setupMirrorPlane();
}

TMirror::TMirror(float width, float height) : m_width(width), m_height(height),
                                              m_position(0.0f, 0.0f, 0.0f), m_rotation(0.0f, 0.0f, 0.0f),
                                              m_scale(1.0f, 1.0f, 1.0f),
                                              m_mirrorVAO(0), m_mirrorVBO(0), m_mirrorEBO(0),
                                              m_reflectionFBO(0), m_reflectionTexture(0), m_reflectionDepthBuffer(0),
                                              m_shader(nullptr), m_scene(nullptr),
                                              m_initialized(false)
{
    m_camera = std::make_shared<TStaticCamera>(m_position, static_cast<GLfloat>(m_width) / m_height);
    initMirrorGeometry();
    setupMirrorPlane();
}

TMirror::~TMirror()
{
    // Clean up OpenGL resources
    if (m_mirrorVAO) glDeleteVertexArrays(1, &m_mirrorVAO);
    if (m_mirrorVBO) glDeleteBuffers(1, &m_mirrorVBO);
    if (m_mirrorEBO) glDeleteBuffers(1, &m_mirrorEBO);

    if (m_reflectionFBO) glDeleteFramebuffers(1, &m_reflectionFBO);
    if (m_reflectionTexture) glDeleteTextures(1, &m_reflectionTexture);
    if (m_reflectionDepthBuffer) glDeleteRenderbuffers(1, &m_reflectionDepthBuffer);
}

void TMirror::initMirrorGeometry()
{
    // Define a simple quad for the mirror plane
    // Vertex positions (x, y, z), texture coords (u, v)
    m_mirrorVertices = {
        // Positions         // Texture Coords
        -0.5f, -0.5f,  0.0f,  0.0f, 0.0f,  // Bottom-left
         0.5f, -0.5f,  0.0f,  1.0f, 0.0f,  // Bottom-right
         0.5f,  0.5f,  0.0f,  1.0f, 1.0f,  // Top-right
        -0.5f,  0.5f,  0.0f,  0.0f, 1.0f   // Top-left
    };

    m_mirrorIndices = {
        0, 1, 2,  // First triangle
        2, 3, 0   // Second triangle
    };
}

void TMirror::setupMirrorPlane()
{
    // Generate and bind VAO
    glGenVertexArrays(1, &m_mirrorVAO);
    glBindVertexArray(m_mirrorVAO);

    // Generate and bind VBO
    glGenBuffers(1, &m_mirrorVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_mirrorVBO);
    glBufferData(GL_ARRAY_BUFFER, m_mirrorVertices.size() * sizeof(float),
                 m_mirrorVertices.data(), GL_STATIC_DRAW);

    // Generate and bind EBO
    glGenBuffers(1, &m_mirrorEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_mirrorEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_mirrorIndices.size() * sizeof(uint32_t),
                 m_mirrorIndices.data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Unbind VAO
    glBindVertexArray(0);

    // Create reflection framebuffer
    glGenFramebuffers(1, &m_reflectionFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_reflectionFBO);

    // Create reflection texture
    glGenTextures(1, &m_reflectionTexture);
    glBindTexture(GL_TEXTURE_2D, m_reflectionTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 1024, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_reflectionTexture, 0);

    // Create depth buffer for reflection
    glGenRenderbuffers(1, &m_reflectionDepthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_reflectionDepthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 1024, 1024);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_reflectionDepthBuffer);

    // Check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    }

    // Unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    m_initialized = true;
}

void TMirror::setPosition(const glm::vec3& position)
{
    m_position = position;
}

void TMirror::setRotation(const glm::vec3& rotation)
{
    m_rotation = rotation;
}

void TMirror::setScale(const glm::vec3& scale)
{
    m_scale = scale;
}

glm::mat4 TMirror::getModelMatrix() const
{
    glm::mat4 model = glm::mat4(1.0f);

    // Apply transformations in order: scale -> rotate -> translate
    model = glm::translate(model, m_position);
    model = glm::rotate(model, glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, m_scale);

    return model;
}

glm::mat4 TMirror::calculateReflectionMatrix(const glm::vec3& mirrorNormal, const glm::vec3& mirrorPoint) const
{
    // Calculate reflection matrix for planar reflections
    // This is a simplified version - in practice, you'd want to compute the actual reflection matrix
    glm::mat4 reflectionMatrix(1.0f);

    // For a mirror facing along the Z-axis, the reflection matrix would flip the Z coordinate
    reflectionMatrix[2][2] = -1.0f;

    return reflectionMatrix;
}

glm::vec3 TMirror::getNormal() const
{
    // Assuming the mirror faces the positive Z direction initially
    // Transform the normal based on the mirror's rotation
    glm::vec3 normal = glm::vec3(0.0f, 0.0f, 1.0f);

    // Apply rotation to the normal
    glm::mat4 rotationMatrix = glm::mat4(1.0f);
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    normal = glm::vec3(rotationMatrix * glm::vec4(normal, 0.0f));
    return glm::normalize(normal);
}

void TMirror::setClipPlane( glm::vec4 const & clipPlane)
{
    m_clipPlane = clipPlane;
}

glm::vec4 const & TMirror::ClipPlane() const
{
    return m_clipPlane;
}

glm::mat4 TMirror::createReflectionMatrix(const glm::vec3& normal, float d)
{
    float a = normal.x, b = normal.y, c = normal.z;

    return glm::mat4(
        1.0f - 2.0f * a * a, -2.0f * a * b,        -2.0f * a * c,        0.0f,
        -2.0f * a * b,        1.0f - 2.0f * b * b, -2.0f * b * c,        0.0f,
        -2.0f * a * c,        -2.0f * b * c,        1.0f - 2.0f * c * c, 0.0f,
        -2.0f * a * d,        -2.0f * b * d,        -2.0f * c * d,        1.0f
        );
}

void TMirror::renderReflection()
{
    if (!m_initialized || !m_scene || !m_scene->Camera()) return;

    // Calculate reflection parameters
    glm::vec3 mirrorNormal = glm::normalize(getNormal());
    glm::vec3 mirrorPoint = m_position;
    float mirrorD = -glm::dot(mirrorNormal, mirrorPoint);

    // Get original camera parameters
    auto camera = m_scene->Camera();
    glm::vec3 originalPos = camera->Position();
    glm::vec3 originalFront = camera->Front();
    glm::vec3 originalUp = camera->Up();

    float distToPlane = glm::dot(originalPos, mirrorNormal) + mirrorD;
    glm::vec3 reflectedCamPos = originalPos - 2.0f * distToPlane * mirrorNormal;
    glm::vec3 reflectedFront = glm::reflect(originalFront, mirrorNormal);

    // Calculate reflected up vector
    glm::vec3 reflectedUp = glm::reflect(originalUp, mirrorNormal);

    // Calculate the look-at point from reflected position and reflected front
    glm::vec3 reflectedLookAt = reflectedCamPos + reflectedFront;

    // Store original OpenGL state
    GLint originalFBO;
    GLint viewport[4];
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &originalFBO);
    glGetIntegerv(GL_VIEWPORT, viewport);

    // Bind reflection framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, m_reflectionFBO);
    glViewport(0, 0, 1024, 1024);

    // Clear buffers
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Enable clipping plane to avoid rendering objects behind the mirror
    glEnable(GL_CLIP_DISTANCE0);

    // Create reflected view matrix directly from reflected camera parameters
    glm::mat4 view = glm::lookAt(reflectedCamPos, reflectedLookAt, reflectedUp);
    glFrontFace(GL_CW);
    m_camera->setWindowRatio(camera->WindowRatio());
    m_camera->setViewMatrix(view);
    m_camera->setPosition(reflectedCamPos);

    glm::vec4 worldClipPlane(mirrorNormal, mirrorD);
    setClipPlane(worldClipPlane);
    // Render all objects in the scene (except the mirror itself)
    for (const auto& obj : SceneCptr()->SceneObjects())
    {
        obj->Update(m_camera);
        obj->Draw();
    }
    SceneCptr()->SkyBox()->Update();
    SceneCptr()->SkyBox()->renderSkybox(view);
    SceneCptr()->SkyBox()->Draw();

    glFrontFace(GL_CCW);

    // Disable clipping plane
    glDisable(GL_CLIP_DISTANCE0);

    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
    // Restore original OpenGL state
    glBindFramebuffer(GL_FRAMEBUFFER, originalFBO);
}

void TMirror::Draw()
{
    if (!m_initialized) return;

    // Use the mirror shader
    if (Shader())
    {
        Shader()->Use();

        // Set the model matrix
        glm::mat4 model = getModelMatrix();
        // Scale the model to the desired width and height
        model = glm::scale(model, glm::vec3(m_width, m_height, 1.0f));

        Shader()->setMat4("model", model);

        glm::vec3 normal = glm::transpose(glm::inverse(glm::mat3(model))) * getNormal();
        Shader()->setVec3("normalVec", normal);
        Shader()->setVec4("clipPlane", ClipPlane());

        // Set view position
        if (SceneCptr() && SceneCptr()->Camera())
        {
            Shader()->setMat4("projection", *SceneCptr()->Camera()->ProjectionCptr());
            Shader()->setMat4("view", SceneCptr()->Camera()->ViewMatrix());
            Shader()->setVec3("viewPos", SceneCptr()->Camera()->Position());
        }

        // Bind the reflection texture to texture unit 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_reflectionTexture);

        // Set mirror material properties
        Shader()->setFloat("metallic", 1.0f);
        Shader()->setFloat("roughness", 0.02f);
        Shader()->setVec3("albedo", glm::vec3(1.0f, 1.0f, 1.0f));

        // Set light positions and colors
        if (SceneCptr()) {
            for (uint32_t i = 0; i < SceneCptr()->Lights().size(); ++i)
            {
                std::string str = "lightPositions[" + std::to_string(i) + "]";
                Shader()->setVec3(str.c_str(), SceneCptr()->Lights().at(i).Position());
                str = "lightColors[" + std::to_string(i) + "]";
                Shader()->setVec3(str.c_str(), SceneCptr()->Lights().at(i).Color());
            }
        }
    }

    // Draw the mirror plane
    glBindVertexArray(m_mirrorVAO);
    glDrawElements(GL_TRIANGLES, m_mirrorIndices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
