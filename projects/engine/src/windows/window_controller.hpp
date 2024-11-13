#pragma once
#include<windows/window_controller_interface.hpp>
#include"camera/camera.hpp"
#include<GLFW/glfw3.h>
#include<inttypes.h>
#include<string>
#include<memory>
#include<functional>
#include<map>

class TWindowController : public IWindow
{
public:
    TWindowController(uint32_t width, uint32_t height, std::string const & windowName);
    ~TWindowController() = default;
    void init() override;
    void play() override;
    uint32_t Width() const override;
    uint32_t Height() const override;
    void setDrawFunc( std::function< void() > const & _drawFunc );

    void setCamera( std::shared_ptr< ICamera > && _camera );

    std::shared_ptr< ICamera > const & Camera();
    GLFWwindow* ptrWindow();

private:
    std::shared_ptr< ICamera > & ptrCamera();

    static void key_callback(GLFWwindow* window, int key, [[maybe_unused]]int scancode, int action, [[maybe_unused]]int mode);
    static void mouse_callback([[maybe_unused]]GLFWwindow* window, double xpos, double ypos);
    static void scroll_callback([[maybe_unused]]GLFWwindow* window, [[maybe_unused]]double xoffset, double yoffset);
private:
    std::shared_ptr< ICamera > m_camera = nullptr;
    std::function< void() > m_drawFunc{};
    GLFWwindow* m_window = nullptr;
    uint32_t m_width = 0;
    uint32_t m_height = 0;
};
