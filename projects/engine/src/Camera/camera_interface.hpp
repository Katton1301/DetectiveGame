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
    virtual GLfloat Zoom() const = 0;
    virtual std::shared_ptr<glm::mat4> const & ProjectionCptr() const = 0;
    virtual GLfloat WindowRatio( ) const = 0;

    virtual glm::vec3 const & Position() const = 0;

    virtual glm::vec3 const & Front() const = 0;
    virtual glm::vec3 const & Up() const = 0;
};
