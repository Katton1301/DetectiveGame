#pragma once
#include<inttypes.h>

struct IWindow
{
    virtual ~IWindow() = default;
    virtual uint32_t Width() const = 0;
    virtual uint32_t Height() const = 0;
    virtual void init() = 0;
    virtual void play() = 0;
};
