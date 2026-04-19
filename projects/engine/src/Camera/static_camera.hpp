#pragma once

#include <vector>

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <camera/camera_interface.hpp>


class TStaticCamera : public ICamera
{
public:
    TStaticCamera(
        glm::vec3 position,
        GLfloat windowRatio,
        glm::vec3 up,
        GLfloat yaw,
        GLfloat pitch,
        GLfloat visual_range_min,
        GLfloat visual_range_max
    );
    TStaticCamera(
        GLfloat posX,
        GLfloat posY,
        GLfloat posZ,
        GLfloat upX,
        GLfloat upY,
        GLfloat upZ,
        GLfloat windowRatio,
        GLfloat yaw,
        GLfloat pitch,
        GLfloat visual_range_min,
        GLfloat visual_range_max
    );
    TStaticCamera(glm::vec3 position, GLfloat windowRatio);
    TStaticCamera();
    glm::mat4 const & ViewMatrix() override;
    void setViewMatrix( glm::mat4 const & _viewMatrix )
    {
        m_viewMatrix = _viewMatrix;
    }
    void setWindowRatio(GLfloat _windowRatio);

    GLfloat Zoom() const override
    {
        return m_zoom;
    }

    GLfloat WindowRatio( ) const override
    {
        return m_windowRatio;
    }

    glm::vec3 const & Position() const override
    {
        return m_position;
    }
    void setPosition(glm::vec3 const & _position)
    {
        m_position = _position;
    }

    glm::vec3 const & Front() const override
    {
        return m_front;
    }

    glm::vec3 const & Up() const override
    {
        return m_up;
    }

    std::shared_ptr<glm::mat4> const & ProjectionCptr() const override
    {
        return m_projectionPtr;
    }

private:
    void updateCameraVectors();
    void recalcProjection();

private:    // Camera Attributes
    glm::vec3 m_position;
    GLfloat m_windowRatio;
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_worldUp;
    // Eular Angles
    GLfloat m_yaw;
    GLfloat m_pitch;
    // Camera options
    GLfloat m_visual_range_min;
    GLfloat m_visual_range_max;
    GLfloat m_movementSpeed;
    GLfloat m_mouseSensitivity;
    GLfloat m_zoom;

    glm::mat4 m_viewMatrix;

    // Projection for camera
    std::shared_ptr<glm::mat4> m_projectionPtr = nullptr;

};
