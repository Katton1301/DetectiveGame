#pragma once

#pragma once

#include <vector>

// GL Includes
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum TCameraMovement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

class TCamera
{
public:
    TCamera(glm::vec3 position, glm::vec3 up, GLfloat yaw , GLfloat pitch);
    TCamera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch);
    TCamera(glm::vec3 position);
    TCamera();
    glm::mat4 GetViewMatrix();
    void ProcessKeyboard(TCameraMovement direction, GLfloat deltaTime);
    void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true);
    void ProcessMouseScroll(GLfloat yoffset);

    GLfloat Zoom() const
    {
        return m_zoom;
    }

    glm::vec3 const & Position() const
    {
        return m_position;
    }

    glm::vec3 const & Front() const
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
