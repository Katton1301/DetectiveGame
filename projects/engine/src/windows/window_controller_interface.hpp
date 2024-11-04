#pragma once

struct IWindow
{
    virtual ~IWindow() = default;
    virtual void init() = 0;
    virtual void play() = 0;
};
