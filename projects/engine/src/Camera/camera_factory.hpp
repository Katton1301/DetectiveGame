#pragma once
#include <camera/camera.hpp>
#include <memory>

enum class TCameraType : uint32_t
{
    DEFAULT_CAMERA = 0
};

std::shared_ptr< ICamera > createCamera( TCameraType _cameraType, glm::vec3 const & _position, GLfloat _windowRatio )
{
    std::shared_ptr< ICamera > camera = nullptr;
    switch (_cameraType) {
    case TCameraType::DEFAULT_CAMERA:
        camera = std::make_shared<TCamera>(_position, _windowRatio);
        break;
    default:
        camera = std::make_shared<TCamera>(_position, _windowRatio);
        break;
    }
    return camera;
}
