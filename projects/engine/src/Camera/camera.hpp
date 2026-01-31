#pragma once

#include <vector>

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <camera/camera_interface.hpp>


class TCamera : public ICamera
{
public:
    TCamera(
        glm::vec3 position,
        GLfloat windowRatio,
        glm::vec3 up,
        GLfloat yaw,
        GLfloat pitch,
        GLfloat visual_range_min,
        GLfloat visual_range_max
    );
    TCamera(
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
    TCamera(glm::vec3 position, GLfloat windowRatio);
    TCamera();
    glm::mat4 GetViewMatrix() override;
    void ProcessKeyboard(TCameraMovement direction, GLfloat deltaTime) override;
    void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true) override;
    void ProcessMouseScroll(GLfloat yoffset) override;
    void setWindowRatio(GLfloat _windowRatio) override;
    void setVisualRangeMin(GLfloat _min) override;
    void setVisualRangeMax(GLfloat _max) override;

    GLfloat Zoom() const override
    {
        return m_zoom;
    }

    glm::vec3 const & Position() const override
    {
        return m_position;
    }

    glm::vec3 const & Front() const override
    {
        return m_front;
    }

    std::shared_ptr<glm::mat4> const & ProjectionCptr() const
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

    // Projection for camera
    std::shared_ptr<glm::mat4> m_projectionPtr = nullptr;

};
