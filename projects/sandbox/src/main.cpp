#include"shaders/pbr_shader.hpp"
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

uint32_t screenWidth = 800;
uint32_t screenHeight = 600;
GLfloat lastX = 400, lastY = 300;

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

    PBRShader pbrShader;
    pbrShader.BackShader()->Use();
    pbrShader.BackShader()->setInt("environmentMap", 0);

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

    TCubeVertices cubeVertices;
    TQuadVertices quadVertices;
    TSphereVertices sphereVertices(64);

    // pbr: setup framebuffer
    // ----------------------
    uint32_t captureFBO;
    uint32_t captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    // pbr: load the HDR environment map
    // ---------------------------------
    defaultTextureBuilder.setMinFilter(GL_LINEAR);
    defaultTextureBuilder.setMipMapGeneration(false);
    uint32_t hdrTexture = defaultTextureBuilder.MakeTexture("./texture", "newport_loft.hdr");

    // pbr: setup cubemap to render to and attach to framebuffer
    // ---------------------------------------------------------
    TCubeMapBuilder defaultCubemapBuilder;

    defaultCubemapBuilder.setMipMapGeneration(false);
    uint32_t envCubemap = defaultCubemapBuilder.MakeCubemap(512, 512);

    // pbr: set up projection and view matrices for capturing data onto the 6 cubemap face directions
    // ----------------------------------------------------------------------------------------------
    glm::mat4 captureProjection = glm::perspective(90.0f, 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] =
    {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    // pbr: convert HDR equirectangular environment map to cubemap equivalent
    // ----------------------------------------------------------------------
    pbrShader.EtCShader()->Use();
    pbrShader.EtCShader()->setInt("equirectangularMap", 0);
    pbrShader.EtCShader()->setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);

    glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (uint32_t i = 0; i < 6; ++i)
    {
        pbrShader.EtCShader()->setMat4("view",captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(cubeVertices.VAO());
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // pbr: create an irradiance cubemap, and re-scale capture FBO to irradiance scale.
    // --------------------------------------------------------------------------------

    defaultCubemapBuilder.setMipMapGeneration(false);
    defaultCubemapBuilder.setMinFilter(GL_LINEAR);
    uint32_t irradianceMap = defaultCubemapBuilder.MakeCubemap(32, 32);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

    // pbr: solve diffuse integral by convolution to create an irradiance (cube)map.
    // -----------------------------------------------------------------------------
    pbrShader.IrradianceShader()->Use();
    pbrShader.IrradianceShader()->setInt("environmentMap", 0);
    pbrShader.IrradianceShader()->setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

    glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (uint32_t i = 0; i < 6; ++i)
    {
        pbrShader.IrradianceShader()->setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(cubeVertices.VAO());
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // pbr: create a pre-filter cubemap, and re-scale capture FBO to pre-filter scale.
    // --------------------------------------------------------------------------------

    defaultCubemapBuilder.setMipMapGeneration(true);
    uint32_t prefilterMap = defaultCubemapBuilder.MakeCubemap(128,128);

    // pbr: run a quasi monte-carlo simulation on the environment lighting to create a prefilter (cube)map.
    // ----------------------------------------------------------------------------------------------------
    pbrShader.PrefilterShader()->Use();
    pbrShader.PrefilterShader()->setInt("environmentMap", 0);
    pbrShader.PrefilterShader()->setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    uint32_t maxMipLevels = 5;
    for (uint32_t mip = 0; mip < maxMipLevels; ++mip)
    {
        // reisze framebuffer according to mip-level size.
        uint32_t mipWidth = static_cast<uint32_t>(128 * std::pow(0.5, mip));
        uint32_t mipHeight = static_cast<uint32_t>(128 * std::pow(0.5, mip));
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(maxMipLevels - 1);
        pbrShader.PrefilterShader()->setFloat("roughness", roughness);
        for (uint32_t i = 0; i < 6; ++i)
        {
            pbrShader.PrefilterShader()->setMat4("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glBindVertexArray(cubeVertices.VAO());
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // pbr: generate a 2D LUT from the BRDF equations used.
    // ----------------------------------------------------
    uint32_t brdfLUTTexture;
    glGenTextures(1, &brdfLUTTexture);

    // pre-allocate enough memory for the LUT texture.
    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
    // be sure to set wrapping mode to GL_CLAMP_TO_EDGE
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

    glViewport(0, 0, 512, 512);
    pbrShader.BRDFShader()->Use();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindVertexArray(quadVertices.VAO());
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // initialize static shader uniforms before rendering
    // --------------------------------------------------
    glm::mat4 projection = glm::perspective(camera.Zoom(), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
    pbrShader.MainShader()->Use();
    pbrShader.MainShader()->setMat4("projection", projection);
    pbrShader.BackShader()->Use();
    pbrShader.BackShader()->setMat4("projection", projection);

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

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);
        pbrShader.MainShader()->Use();
        pbrShader.MainShader()->setMat4("view", view);
        pbrShader.MainShader()->setVec3("camPos", camera.Position());

        // bind pre-computed IBL data
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);

        // rusted iron
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, ironMap.albedo);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, ironMap.normal);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, ironMap.metallic);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, ironMap.roughness);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, ironMap.ao);


        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-5.0, 0.0, 2.0));
        pbrShader.MainShader()->setMat4("model", model);
        glm::mat3 normalMatrix  = glm::transpose(glm::inverse(glm::mat3(model)));
        pbrShader.MainShader()->setMat3("normalMatrix", normalMatrix);
        glBindVertexArray(sphereVertices.VAO());
        glDrawElements(GL_TRIANGLE_STRIP, sphereVertices.Indices().size(), GL_UNSIGNED_INT, 0);

        // gold
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, goldMap.albedo);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, goldMap.normal);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, goldMap.metallic);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, goldMap.roughness);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, goldMap.ao);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-3.0, 0.0, 2.0));
        pbrShader.MainShader()->setMat4("model", model);
        normalMatrix  = glm::transpose(glm::inverse(glm::mat3(model)));
        pbrShader.MainShader()->setMat3("normalMatrix", normalMatrix);
        glBindVertexArray(sphereVertices.VAO());
        glDrawElements(GL_TRIANGLE_STRIP, sphereVertices.Indices().size(), GL_UNSIGNED_INT, 0);

        // grass
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, grassMap.albedo);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, grassMap.normal);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, grassMap.metallic);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, grassMap.roughness);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, grassMap.ao);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.0, 0.0, 2.0));
        pbrShader.MainShader()->setMat4("model", model);
        normalMatrix  = glm::transpose(glm::inverse(glm::mat3(model)));
        pbrShader.MainShader()->setMat3("normalMatrix", normalMatrix);
        glBindVertexArray(sphereVertices.VAO());
        glDrawElements(GL_TRIANGLE_STRIP, sphereVertices.Indices().size(), GL_UNSIGNED_INT, 0);

        // marble
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, marbleMap.albedo);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, marbleMap.normal);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, marbleMap.metallic);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, marbleMap.roughness);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, marbleMap.ao);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(1.0, 0.0, 2.0));
        pbrShader.MainShader()->setMat4("model", model);
        normalMatrix  = glm::transpose(glm::inverse(glm::mat3(model)));
        pbrShader.MainShader()->setMat3("normalMatrix", normalMatrix);
        glBindVertexArray(sphereVertices.VAO());
        glDrawElements(GL_TRIANGLE_STRIP, sphereVertices.Indices().size(), GL_UNSIGNED_INT, 0);

        // wood
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, woodMap.albedo);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, woodMap.normal);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, woodMap.metallic);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, woodMap.roughness);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, woodMap.ao);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(3.0, 0.0, 2.0));
        pbrShader.MainShader()->setMat4("model", model);
        normalMatrix  = glm::transpose(glm::inverse(glm::mat3(model)));
        pbrShader.MainShader()->setMat3("normalMatrix", normalMatrix);
        glBindVertexArray(sphereVertices.VAO());
        glDrawElements(GL_TRIANGLE_STRIP, sphereVertices.Indices().size(), GL_UNSIGNED_INT, 0);

        // render light source (simply re-render sphere at light positions)
        // this looks a bit off as we use the same shader, but it'll make their positions obvious and 
        // keeps the codeprint small.
        for (uint32_t i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
        {
            glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
            newPos = lightPositions[i];
            std::string str = "lightPositions[" + std::to_string(i) + "]";
            pbrShader.MainShader()->setVec3(str.c_str(), newPos);
            str = "lightColors[" + std::to_string(i) + "]";
            pbrShader.MainShader()->setVec3(str.c_str(), lightColors[i]);

            model = glm::mat4(1.0f);
            model = glm::translate(model, newPos);
            model = glm::scale(model, glm::vec3(0.5f));
            pbrShader.MainShader()->setMat4("model", model);
            normalMatrix  = glm::transpose(glm::inverse(glm::mat3(model)));
            pbrShader.MainShader()->setMat3("normalMatrix", normalMatrix);
            glBindVertexArray(sphereVertices.VAO());
            glDrawElements(GL_TRIANGLE_STRIP, sphereVertices.Indices().size(), GL_UNSIGNED_INT, 0);
        }

        // render skybox (render as last to prevent overdraw)
        pbrShader.BackShader()->Use();
        pbrShader.BackShader()->setMat4("view", view);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
        //glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap); // display irradiance map
        //glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap); // display prefilter map

        glBindVertexArray(cubeVertices.VAO());
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        /*equirectangularToCubemapShader->Use();
        equirectangularToCubemapShader->setMat4("view", view);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, hdrTexture);
        glBindVertexArray(cubeVertices.VAO());
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);*/

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
