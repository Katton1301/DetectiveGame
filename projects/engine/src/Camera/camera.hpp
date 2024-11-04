#pragma once

#include <vector>

#include <glm/gtc/matrix_transform.hpp>
#include <camera/camera_interface.hpp>

class TCamera : public ICamera
{
public:
    TCamera(glm::vec3 position, glm::vec3 up, GLfloat yaw , GLfloat pitch);
    TCamera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch);
    TCamera(glm::vec3 position);
    TCamera();
    glm::mat4 GetViewMatrix() override;
    void ProcessKeyboard(TCameraMovement direction, GLfloat deltaTime) override;
    void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true) override;
    void ProcessMouseScroll(GLfloat yoffset) override;

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

private:
    void updateCameraVectors();

private:    // Camera Attributes
    glm::vec3 m_position;
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_worldUp;
    // Eular Angles
    GLfloat m_yaw;
    GLfloat m_pitch;
    // Camera options
    GLfloat m_movementSpeed;
    GLfloat m_mouseSensitivity;
    GLfloat m_zoom;

};
