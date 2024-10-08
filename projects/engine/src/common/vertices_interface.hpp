#pragma once
#include<stdint.h>

struct IVertices
{
    virtual ~IVertices() = default;
    virtual void Init() = 0;
    virtual float& operator[](uint32_t index) = 0;
    virtual float at(uint32_t index) const = 0;
    virtual uint32_t VAO() const = 0;
};
