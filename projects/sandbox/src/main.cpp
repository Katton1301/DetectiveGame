#ifdef _WIN32
#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h>
#endif // _WIN32
#include "sandbox_view.hpp"
#include<iostream>



int main()
{
    TSandbox sandBox;
    sandBox.createWindow(1024,768);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
    }

    sandBox.init();
    sandBox.start();

    return 0;
}
