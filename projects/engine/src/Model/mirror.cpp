#include "mirror.hpp"
#include <iostream>
#include <shaders/shader.hpp>

TMirror::TMirror() : m_width(1.0f), m_height(1.0f),
                     m_position(0.0f, 0.0f, 0.0f), m_rotation(0.0f, 0.0f, 0.0f),
                     m_scale(1.0f, 1.0f, 1.0f),
                     m_mirrorVAO(0), m_mirrorVBO(0), m_mirrorEBO(0),
                     m_reflectionFBO(0), m_reflectionTexture(0), m_reflectionDepthBuffer(0),
                     m_shader(nullptr), m_scene(nullptr),
                     m_initialized(false)
{
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

void TMirror::setupReflectionRendering()
{
    // Calculate reflected camera position
    glm::vec3 mirrorNormal = getNormal();
    glm::vec3 mirrorCenter = m_position;

    // Calculate reflected camera position
    glm::vec3 camPos = m_scene->Camera()->Position();
    glm::vec3 reflectedCamPos = camPos - 2.0f * glm::dot(camPos - mirrorCenter, mirrorNormal) * mirrorNormal;

    // Calculate reflected view matrix
    glm::vec3 originalTarget = m_scene->Camera()->Position() + m_scene->Camera()->Front();
    glm::vec3 reflectedTarget = originalTarget - 2.0f * glm::dot(originalTarget - mirrorCenter, mirrorNormal) * mirrorNormal;

    // For the up vector, we'll use the world up vector (0, 1, 0) and reflect it
    glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 reflectedUp = worldUp - 2.0f * glm::dot(worldUp, mirrorNormal) * mirrorNormal;

    // Store the original viewport
    int viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    // Bind reflection framebuffer for rendering the reflected scene
    glBindFramebuffer(GL_FRAMEBUFFER, m_reflectionFBO);
    glViewport(0, 0, 1024, 1024);

    // Clear the reflection buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // In a real implementation, we would need to update the camera with the reflected view
    // For now, we'll just set up the matrices for rendering
}

void TMirror::renderReflection()
{
    if (!m_initialized || !m_scene || !m_scene->Camera()) return;


    // Calculate reflection parameters
    glm::vec3 mirrorNormal = getNormal();
    glm::vec3 mirrorCenter = m_position;

    // Get original camera parameters
    auto camera = m_scene->Camera();
    glm::vec3 originalPos = camera->Position();
    glm::vec3 originalFront = camera->Front();
    glm::vec3 originalUp = camera->Up();

    // Calculate reflected camera position
    glm::vec3 toMirror = mirrorCenter - originalPos;
    float distanceToMirror = glm::dot(toMirror, mirrorNormal);
    glm::vec3 reflectedCamPos = originalPos + 2.0f * distanceToMirror * mirrorNormal;

    // Calculate reflected camera direction
    glm::vec3 originalTarget = originalPos + originalFront;
    glm::vec3 toTarget = originalTarget - mirrorCenter;
    glm::vec3 reflectedTarget = originalTarget - 2.0f * glm::dot(toTarget, mirrorNormal) * mirrorNormal;
    glm::vec3 reflectedFront = glm::normalize(reflectedTarget - reflectedCamPos);

    // Calculate reflected up vector
    glm::vec3 reflectedUp = originalUp - 2.0f * glm::dot(originalUp, mirrorNormal) * mirrorNormal;
    reflectedUp = glm::normalize(reflectedUp);

    // Calculate reflection matrix for clipping plane
    float d = -glm::dot(mirrorNormal, mirrorCenter);
    glm::vec4 clipPlane = glm::vec4(mirrorNormal, d);


    // Store original OpenGL state
    GLint originalFBO;
    GLint viewport[4];
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &originalFBO);
    glGetIntegerv(GL_VIEWPORT, viewport);

    // Bind reflection framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, m_reflectionFBO);
    glViewport(0, 0, 1024, 1024);

    // Clear buffers
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Disable depth clamping if enabled
    glDisable(GL_DEPTH_CLAMP);
    // Enable clipping plane to avoid rendering objects behind the mirror
    glEnable(GL_CLIP_DISTANCE0);

    // Get projection matrix
    glm::mat4 projection = *camera->ProjectionCptr();

    // Create reflected view matrix
    glm::mat4 view = glm::lookAt(reflectedCamPos, reflectedCamPos + reflectedFront, reflectedUp);

    // Render all objects in the scene (except the mirror itself)
    auto objects = SceneCptr()->SceneObjects();

    //m_camera->setViewMatrixForce(view);

    for (const auto& obj : objects)
    {
        /*
        // Skip rendering the mirror itself
        if (obj.get() == this) continue;
        //To do: make mirror camera and give object mirror camera
        // Get object's shader
        auto shader = obj->Shader();
        if (!shader) continue;

        shader->Use();

        // Set basic matrices
        shader->setMat4("projection", projection);
        shader->setMat4("view", view);

        // Calculate and set model matrix
        glm::mat4 model = obj->getModelMatrix();
        shader->setMat4("model", model);

        // Calculate and set normal matrix
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
        shader->setMat3("normalMatrix", normalMatrix);

        // Set clip plane uniform (if shader supports it)
        try {
            shader->setVec4("clipPlane", clipPlane);
        } catch (...) {
            // Shader might not have clipPlane uniform
        }

        // Set view position (reflected camera position)
        shader->setVec3("viewPos", reflectedCamPos);

        // Set lights
        auto lights = SceneCptr()->Lights();
        for (uint32_t i = 0; i < lights.size(); ++i)
        {
            std::string posStr = "lightPositions[" + std::to_string(i) + "]";
            std::string colorStr = "lightColors[" + std::to_string(i) + "]";

            // For lights, we need to reflect their positions too for accurate lighting
            glm::vec3 lightPos = lights[i].Position();
            glm::vec3 reflectedLightPos = lightPos - 2.0f * glm::dot(lightPos - mirrorCenter, mirrorNormal) * mirrorNormal;

            shader->setVec3(posStr.c_str(), reflectedLightPos);
            shader->setVec3(colorStr.c_str(), lights[i].Color());
        }
        */
        obj->Update();

        // Draw the object
        obj->Draw();
    }

    SceneCptr()->SkyBox()->Update();
    SceneCptr()->SkyBox()->renderSkybox(view);
    SceneCptr()->SkyBox()->Draw();


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

        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
        Shader()->setMat3("normalMatrix", normalMatrix);

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
