#pragma once
#include <common/object_interface.hpp>
#include <shaders/shader.hpp>

class ISceneObject : public IObject
{
public:
    virtual ~ISceneObject() = default;

protected:
    virtual HShader Shader() const = 0;
};
