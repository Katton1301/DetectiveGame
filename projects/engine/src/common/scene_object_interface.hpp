#pragma once
#include <common/object_interface.hpp>
#include <shaders/shader.hpp>
#include <camera/camera_interface.hpp>
#include <memory>

class ISceneObject : public IObject
{
public:
    virtual ~ISceneObject() = default;

    virtual void Update() override = 0;
    virtual void Update(std::shared_ptr<ICamera> _camera) = 0;

protected:
    virtual HShader Shader() const = 0;
};
