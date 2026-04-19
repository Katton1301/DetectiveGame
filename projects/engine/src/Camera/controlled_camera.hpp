#pragma once

#include <vector>

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <camera/camera_interface.hpp>


class TControlledCamera : public ICamera
{
public:
    TControlledCamera(
        glm::vec3 position,
        GLfloat windowRatio,
        glm::vec3 up,
        GLfloat yaw,
        GLfloat pitch,
        GLfloat visual_range_min,
        GLfloat visual_range_max
    );
    TControlledCamera(
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
    TControlledCamera(glm::vec3 position, GLfloat windowRatio);
    TControlledCamera();
    glm::mat4 const & ViewMatrix() override;
    void HoldView();
    void ProcessKeyboard(TCameraMovement direction, GLfloat deltaTime);
    void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true);
    void ProcessMouseScroll(GLfloat yoffset);
    void setWindowRatio(GLfloat _windowRatio);
    void setVisualRangeMin(GLfloat _min);
    void setVisualRangeMax(GLfloat _max);

    GLfloat WindowRatio( ) const override
    {
        return m_windowRatio;
    }

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

    glm::mat4 m_holdedView;

    // Projection for camera
    std::shared_ptr<glm::mat4> m_projectionPtr = nullptr;

};
