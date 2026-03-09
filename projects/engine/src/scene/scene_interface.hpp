#pragma once
#include <common/lights.hpp>
#include <camera/camera.hpp>
#include <model/skybox.hpp>
#include <common/scene_object_interface.hpp>
#include <vector>

struct IScene
{
    virtual ~IScene() = default;

    virtual std::vector< TLight > const & Lights() const = 0;
    virtual std::shared_ptr< ICamera > const & Camera() const = 0;
    virtual std::shared_ptr< TSkyBox > const & SkyBox() const = 0;
    virtual std::vector<std::shared_ptr< ISceneObject >> const & SceneObjects() const = 0;
};
