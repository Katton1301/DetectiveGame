#include"./window_controller.hpp"
#include<iostream>



static std::array<bool,1024l> keybord_kyes;
static GLfloat lastX = 400;
static GLfloat lastY = 300;

static std::map< GLFWwindow*, std::shared_ptr< ICamera >> & CameraContainer()
{
    static std::map< GLFWwindow*, std::shared_ptr< ICamera >> cameraContainer{};
    return cameraContainer;
}

static GLfloat deltaTime = 0.0f;	// Время, прошедшее между последним и текущим кадром
static GLfloat lastFrame = 0.0f;  	// Время вывода последнего кадра

void RegisterCamera(GLFWwindow* window, std::shared_ptr< ICamera > & _camera)
{
    if(!CameraContainer().contains(window))
    {
        CameraContainer().emplace(window, _camera);
    }
    else
    {
        CameraContainer().at(window) = _camera;
    }

}

TWindowController::TWindowController(uint32_t _width, uint32_t _height, std::string const & windowName)
{
    m_window = glfwCreateWindow(_width, _height, windowName.c_str(), nullptr, nullptr);

    if (ptrWindow() == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(ptrWindow());
}

void TWindowController::key_callback(GLFWwindow* window, int key, [[maybe_unused]]int scancode, int action, [[maybe_unused]]int mode)
{
    if(action == GLFW_PRESS)
      keybord_kyes[key] = true;
    else if(action == GLFW_RELEASE)
      keybord_kyes[key] = false;

    // Когда пользователь нажимает ESC, мы устанавливаем свойство WindowShouldClose в true,
    // и приложение после этого закроется
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

void TWindowController::mouse_callback([[maybe_unused]]GLFWwindow* window, double xpos, double ypos)
{
    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

    lastX = xpos;
    lastY = ypos;

    CameraContainer().at(window)->ProcessMouseMovement(xoffset, yoffset);
}

void TWindowController::scroll_callback([[maybe_unused]]GLFWwindow* window, [[maybe_unused]]double xoffset, double yoffset)
{
    CameraContainer().at(window)->ProcessMouseScroll(yoffset);
}

void TWindowController::init()
{
    int width, height;
    glfwGetFramebufferSize(ptrWindow(), &width, &height);
    glfwSetKeyCallback(ptrWindow(), key_callback);

    glViewport(0, 0, width, height);
    glfwSetInputMode(ptrWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetCursorPosCallback(ptrWindow(), mouse_callback);
    glfwSetScrollCallback(ptrWindow(), scroll_callback);
}

void TWindowController::play()
{
    // then before rendering, configure the viewport to the original framebuffer's screen dimensions
    int scrWidth, scrHeight;
    glfwGetFramebufferSize(ptrWindow(), &scrWidth, &scrHeight);
    glViewport(0, 0, scrWidth, scrHeight);

    double posX,posY;
    glfwGetCursorPos(ptrWindow(), &posX, &posY);
    lastX = posX;
    lastY = posY;

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    while(!glfwWindowShouldClose(ptrWindow()))
    {
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Проверяем события и вызываем функции обратного вызова.
        // Camera controls
        if(keybord_kyes[GLFW_KEY_W])
            Camera()->ProcessKeyboard(FORWARD, deltaTime);
        if(keybord_kyes[GLFW_KEY_S])
            Camera()->ProcessKeyboard(BACKWARD, deltaTime);
        if(keybord_kyes[GLFW_KEY_A])
            Camera()->ProcessKeyboard(LEFT, deltaTime);
        if(keybord_kyes[GLFW_KEY_D])
            Camera()->ProcessKeyboard(RIGHT, deltaTime);
        // Команды отрисовки здесь

        // move light position over time
        //lightPos.z = static_cast<float>(sin(glfwGetTime() * 0.5) * 3.0);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if(m_drawFunc)
        {
            m_drawFunc();
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(ptrWindow());
        glfwPollEvents();
    }
    glfwTerminate();
}

void TWindowController::setDrawFunc( std::function< void() > const & _drawFunc )
{
    m_drawFunc = _drawFunc;
}

void TWindowController::setCamera( std::shared_ptr< ICamera > && _camera )
{
    m_camera = _camera;
    RegisterCamera(ptrWindow(), m_camera);
}

std::shared_ptr< ICamera > const & TWindowController::Camera()
{
    return m_camera;
}


std::shared_ptr< ICamera > & TWindowController::ptrCamera()
{
    return m_camera;
}


GLFWwindow* TWindowController::ptrWindow()
{
    return m_window;
}
