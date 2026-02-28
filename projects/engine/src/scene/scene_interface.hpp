#pragma once
#include <common/lights.hpp>
#include <camera/camera.hpp>
#include <vector>

struct IScene
{
    virtual ~IScene() = default;

    virtual std::vector< TLight > const & Lights() const = 0;
    virtual std::shared_ptr< ICamera > const & Camera() const = 0;
};
