#include"tools/pbr_builder.hpp"
#include<windows/window_controller.hpp>
#include "common/vertices.hpp"
#include <camera/camera_factory.hpp>
//#include "Model/model.hpp"
#include <filesystem>
#include "textures/texture.hpp"
#include <iostream>
#include <map>
#include <array>
#include <random>

uint32_t screenWidth = 1024;
uint32_t screenHeight = 768;
uint32_t renderBufferSize = 512;
uint32_t minBufferSize = 32;
GLfloat lastX = 400, lastY = 300;
bool keys[1024];
GLfloat deltaTime = 0.0f;	// Время, прошедшее между последним и текущим кадром
GLfloat lastFrame = 0.0f;  	// Время вывода последнего кадра

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

    TWindowController windowController(screenWidth, screenHeight, "LearnOpenGL");
    windowController.setCamera(createCamera(TCameraType::DEFAULT_CAMERA, glm::vec3(0.0f, 0.0f, 3.0f)));

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    windowController.init();


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
    std::vector<glm::vec3> lightPositions = {
        glm::vec3(-10.0f,  10.0f, 10.0f),
        glm::vec3( 10.0f,  10.0f, 10.0f),
        glm::vec3(-10.0f, -10.0f, 10.0f),
        glm::vec3( 10.0f, -10.0f, 10.0f),
    };
    std::vector<glm::vec3> lightColors = {
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
    pbrBuilder.setProjection(glm::perspective(windowController.Camera()->Zoom(), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f));

    windowController.setDrawFunc(
        [&]()
        {
            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 view = windowController.Camera()->GetViewMatrix();
            pbrBuilder.initMainShadersEnvs( view, windowController.Camera()->Position());

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
            for (uint32_t i = 0; i < lightPositions.size(); ++i)
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
        }
    );

    windowController.play();

    return 0;
}
