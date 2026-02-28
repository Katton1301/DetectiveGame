#pragma once
#include <memory>
// GL Includes
#include <GL/glew.h>
#include <glm/glm.hpp>

enum TCameraMovement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

struct ICamera
{
    virtual ~ICamera() = default;
    virtual glm::mat4 const & ViewMatrix() = 0;
    virtual void HoldView() = 0;
    virtual void ProcessKeyboard(TCameraMovement direction, GLfloat deltaTime) = 0;
    virtual void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true) = 0;
    virtual void ProcessMouseScroll(GLfloat yoffset) = 0;
    virtual void setWindowRatio(GLfloat _windowRatio) = 0;
    virtual void setVisualRangeMin(GLfloat _min) = 0;
    virtual void setVisualRangeMax(GLfloat _max) = 0;
    virtual GLfloat Zoom() const = 0;
    virtual std::shared_ptr<glm::mat4> const & ProjectionCptr() const = 0;

    virtual glm::vec3 const & Position() const = 0;

    virtual glm::vec3 const & Front() const = 0;
};
