#pragma once
#include <model/model.hpp>
#include <tools/pbr_builder.hpp>
#include <textures/texture.hpp>
#include <common/vertices.hpp>
#include <camera/static_camera.hpp>
#include <scene/scene_interface.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class TMirror
{
public:
    TMirror();
    TMirror(float width, float height);
    virtual ~TMirror();

    // Set mirror position and orientation
    void setPosition(const glm::vec3& position);
    void setRotation(const glm::vec3& rotation);
    void setScale(const glm::vec3& scale);

    // Get mirror transform matrix
    glm::mat4 getModelMatrix() const;

    // Render the mirror with reflection
    void renderReflection();

    // Draw the mirror plane
    void Draw();

    // Get mirror dimensions
    float getWidth() const { return m_width; }
    float getHeight() const { return m_height; }


    // Setter methods
    void setShader(HShader shader) { m_shader = shader; }
    void setScene(std::shared_ptr<IScene> const & scene) { m_scene = scene; }

    // Getter methods
    HShader Shader() const { return m_shader; }
    std::shared_ptr<IScene> const & SceneCptr() const { return m_scene; }


private:
    glm::mat4 createReflectionMatrix(const glm::vec3& normal, float d);
    void initMirrorGeometry();
    void setupMirrorPlane();
    glm::mat4 calculateReflectionMatrix(const glm::vec3& mirrorNormal, const glm::vec3& mirrorPoint) const;
    glm::vec3 getNormal() const;
    void setClipPlane( glm::vec4 const & clipPlane);

    glm::vec4 const & ClipPlane() const;

private:
    float m_width;
    float m_height;
    glm::vec3 m_position;
    glm::vec3 m_rotation;  // Euler angles
    glm::vec3 m_scale;

    glm::vec4 m_clipPlane;

    // Mirror plane geometry
    std::vector<float> m_mirrorVertices;
    std::vector<uint32_t> m_mirrorIndices;
    uint32_t m_mirrorVAO;
    uint32_t m_mirrorVBO;
    uint32_t m_mirrorEBO;

    // Reflection buffer
    uint32_t m_reflectionFBO;
    uint32_t m_reflectionTexture;
    uint32_t m_reflectionDepthBuffer;

    std::shared_ptr<TStaticCamera> m_camera;

    // Shader and scene references
    HShader m_shader;
    std::shared_ptr<IScene> m_scene;
    // Flag to check if mirror is initialized
    bool m_initialized;
};
