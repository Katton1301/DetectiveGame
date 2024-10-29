#include"tools/pbr_builder.hpp"
#include "camera/camera.hpp"
#include "common/vertices.hpp"
#include<GLFW/glfw3.h>
//#include "Model/model.hpp"
#include <filesystem>
#include "textures/texture.hpp"
#include "textures/cubemap.hpp"
#include <iostream>
#include <map>
#include <array>
#include <random>

uint32_t screenWidth = 1024;
uint32_t screenHeight = 768;
GLfloat lastX = 400, lastY = 300;
uint32_t renderBufferSize = 512;
uint32_t minBufferSize = 32;

TCamera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool keys[1024];

GLfloat deltaTime = 0.0f;	// Время, прошедшее между последним и текущим кадром
GLfloat lastFrame = 0.0f;  	// Время вывода последнего кадра

void key_callback(GLFWwindow* window, int key, [[maybe_unused]]int scancode, int action, [[maybe_unused]]int mode)
{
    if(action == GLFW_PRESS)
      keys[key] = true;
    else if(action == GLFW_RELEASE)
      keys[key] = false;

    // Когда пользователь нажимает ESC, мы устанавливаем свойство WindowShouldClose в true,
    // и приложение после этого закроется
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

void mouse_callback([[maybe_unused]]GLFWwindow* window, double xpos, double ypos)
{
    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback([[maybe_unused]]GLFWwindow* window, [[maybe_unused]]double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

void do_movement()
{
    // Camera controls
    if(keys[GLFW_KEY_W])
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if(keys[GLFW_KEY_S])
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if(keys[GLFW_KEY_A])
        camera.ProcessKeyboard(LEFT, deltaTime);
    if(keys[GLFW_KEY_D])
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

int main()
{
    //Инициализация GLFW
    glfwInit();
    //Настройка GLFW
    //Задается минимальная требуемая версия OpenGL.
    //Мажорная
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    //Минорная
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //Установка профайла для которого создается контекст
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //Выключение возможности изменения размера окна
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glfwSetKeyCallback(window, key_callback);

    glViewport(0, 0, width, height);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL); // set depth function to less than AND equal for skybox depth trick.
    // enable seamless cubemap sampling for lower mip levels in the pre-filter map.
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    TTextureBuilder defaultTextureBuilder;
    // load PBR material textures
    // --------------------------
    // rusted iron
    auto ironMap = defaultTextureBuilder.MakePBRTextures("./texture/pbr/rusted_iron");
    // gold
    auto goldMap = defaultTextureBuilder.MakePBRTextures("./texture/pbr/gold_nugget");
    // grass
    auto grassMap = defaultTextureBuilder.MakePBRTextures("./texture/pbr/grass");
    // white marble
    auto marbleMap = defaultTextureBuilder.MakePBRTextures("./texture/pbr/white_marble");
    // wood
    auto woodMap = defaultTextureBuilder.MakePBRTextures("./texture/pbr/wood");

    // lights
    // ------
    glm::vec3 lightPositions[] = {
        glm::vec3(-10.0f,  10.0f, 10.0f),
        glm::vec3( 10.0f,  10.0f, 10.0f),
        glm::vec3(-10.0f, -10.0f, 10.0f),
        glm::vec3( 10.0f, -10.0f, 10.0f),
    };
    glm::vec3 lightColors[] = {
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f)
    };

    TSphereVertices sphereVertices(64);
    TCubeVertices cubeVertices;
    TPBRBuilder pbrBuilder;

    // pbr: load the HDR environment map
    // ---------------------------------
    defaultTextureBuilder.setMinFilter(GL_LINEAR);
    defaultTextureBuilder.setMipMapGeneration(false);
    uint32_t hdrTexture = defaultTextureBuilder.MakeTexture("./texture", "newport_loft.hdr");

    pbrBuilder.initCuptureBuffer();
    pbrBuilder.setCubeVAO(cubeVertices.VAO());
    pbrBuilder.convertEtoC(hdrTexture);
    pbrBuilder.setProjection(glm::perspective(camera.Zoom(), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f));

    // then before rendering, configure the viewport to the original framebuffer's screen dimensions
    int scrWidth, scrHeight;
    glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
    glViewport(0, 0, scrWidth, scrHeight);

    double posX,posY;
    glfwGetCursorPos(window, &posX, &posY);
    lastX = posX;
    lastY = posY;

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    while(!glfwWindowShouldClose(window))
    {
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Проверяем события и вызываем функции обратного вызова.
        do_movement();
        // Команды отрисовки здесь

        // move light position over time
        //lightPos.z = static_cast<float>(sin(glfwGetTime() * 0.5) * 3.0);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        pbrBuilder.initMainShadersEnvs( view, camera.Position());

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-5.0, 0.0, 2.0));
        pbrBuilder.drawSphere(ironMap, sphereVertices, model);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-3.0, 0.0, 2.0));
        pbrBuilder.drawSphere(goldMap, sphereVertices, model);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.0, 0.0, 2.0));
        pbrBuilder.drawSphere(grassMap, sphereVertices, model);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(1.0, 0.0, 2.0));
        pbrBuilder.drawSphere(marbleMap, sphereVertices, model);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(3.0, 0.0, 2.0));
        pbrBuilder.drawSphere(woodMap, sphereVertices, model);

        // render light source (simply re-render sphere at light positions)
        // this looks a bit off as we use the same shader, but it'll make their positions obvious and 
        // keeps the codeprint small.
        glm::mat3 normalMatrix;
        for (uint32_t i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
        {
            glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
            //newPos = lightPositions[i];
            std::string str = "lightPositions[" + std::to_string(i) + "]";
            pbrBuilder.Shader().MainShader()->setVec3(str.c_str(), newPos);
            str = "lightColors[" + std::to_string(i) + "]";
            pbrBuilder.Shader().MainShader()->setVec3(str.c_str(), lightColors[i]);

            model = glm::mat4(1.0f);
            model = glm::translate(model, newPos);
            model = glm::scale(model, glm::vec3(0.5f));
            pbrBuilder.drawSphere(woodMap, sphereVertices, model);
        }

        pbrBuilder.renderSkybox(view);
        //pbrBuilder.renderEtoC(view);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
