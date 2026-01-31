#pragma once
#include <common/lights.hpp>
#include <vector>

struct IScene
{
    virtual ~IScene() = default;

    virtual std::vector< TLight > const & Lights() const = 0;
};
