#pragma once
#include <camera/static_camera.hpp>
#include <camera/controlled_camera.hpp>
#include <memory>

enum class TCameraType : uint32_t
{
    CONTROLLED_CAMERA = 0,
    STATIC_CAMERA,
};

std::shared_ptr< ICamera > createCamera( TCameraType _cameraType, glm::vec3 const & _position, GLfloat _windowRatio )
{
    std::shared_ptr< ICamera > camera = nullptr;
    switch (_cameraType) {
    case TCameraType::CONTROLLED_CAMERA:
        camera = std::make_shared<TControlledCamera>(_position, _windowRatio);
        break;
    case TCameraType::STATIC_CAMERA:
        camera = std::make_shared<TStaticCamera>(_position, _windowRatio);
        break;
    default:
        camera = std::make_shared<TStaticCamera>(_position, _windowRatio);
        break;
    }
    return camera;
}
