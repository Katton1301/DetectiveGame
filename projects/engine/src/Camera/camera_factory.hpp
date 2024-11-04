#pragma once
#include <camera/camera.hpp>
#include <memory>

enum class TCameraType : uint32_t
{
    DEFAULT_CAMERA = 0
};

std::shared_ptr< ICamera > createCamera( TCameraType _cameraType, glm::vec3 const & _position )
{
    std::shared_ptr< ICamera > camera = nullptr;
    switch (_cameraType) {
    case TCameraType::DEFAULT_CAMERA:
        camera = std::make_shared<TCamera>(_position);
        break;
    default:
        camera = std::make_shared<TCamera>(_position);
        break;
    }
    return camera;
}
