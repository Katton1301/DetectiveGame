#include "sandbox_view.hpp"

#include <camera/camera_factory.hpp>
#include "common/vertices.hpp"
#include "model/model.hpp"
#include <filesystem>
#include <iostream>
#include <map>
#include <array>
#include <random>

TSandbox::TSandbox()
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
}


void TSandbox::createWindow( uint32_t width, uint32_t height )
{
    m_windowController =  std::make_unique<TWindowController>(width, height, "SandBox");
}

void TSandbox::init()
{
    m_windowController->setCamera(createCamera(TCameraType::DEFAULT_CAMERA, glm::vec3(0.0f, 0.0f, 3.0f)));
    m_windowController->init();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL); // set depth function to less than AND equal for skybox depth trick.
    // enable seamless cubemap sampling for lower mip levels in the pre-filter map.
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    // lights
    // ------
    m_lightPositions = {
        glm::vec3(-10.0f,  10.0f, 10.0f),
        glm::vec3( 10.0f,  10.0f, 10.0f),
        glm::vec3(-10.0f, -10.0f, 10.0f),
        glm::vec3( 10.0f, -10.0f, 10.0f),
    };
    m_lightColors = {
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f)
    };

    TTextureBuilder defaultTextureBuilder;
    // load PBR material textures
    // --------------------------
    // rusted iron
    auto ironMap = defaultTextureBuilder.MakePBRTextures("./texture/pbr/rusted_iron");
    m_texturesMap.emplace("Iron", ironMap);
    // gold
    auto goldMap = defaultTextureBuilder.MakePBRTextures("./texture/pbr/gold_nugget");
    m_texturesMap.emplace("Gold", goldMap);
    // grass
    auto grassMap = defaultTextureBuilder.MakePBRTextures("./texture/pbr/grass");
    m_texturesMap.emplace("Grass", grassMap);
    // white marble
    auto marbleMap = defaultTextureBuilder.MakePBRTextures("./texture/pbr/white_marble");
    m_texturesMap.emplace("Marble", marbleMap);
    // wood
    auto woodMap = defaultTextureBuilder.MakePBRTextures("./texture/pbr/wood");
    m_texturesMap.emplace("Wood", woodMap);

    m_sphereVertices = std::make_shared<TSphereVertices>(64);
    TCubeVertices cubeVertices;
    m_pbrBuilder = std::make_shared<TPBRBuilder>();

    // pbr: load the HDR environment map
    // ---------------------------------
    defaultTextureBuilder.setMinFilter(GL_LINEAR);
    defaultTextureBuilder.setMipMapGeneration(false);
    uint32_t hdrTexture = defaultTextureBuilder.MakeTexture("./texture", "newport_loft.hdr");

    //TModel man("./models/man.obj");


    m_pbrBuilder->initCuptureBuffer();
    m_pbrBuilder->setCubeVAO(cubeVertices.VAO());
    m_pbrBuilder->convertEtoC(hdrTexture);
    m_pbrBuilder->setProjection(glm::perspective(cptrWindowController()->Camera()->Zoom(), (float)cptrWindowController()->Width() / (float)cptrWindowController()->Height(), 0.1f, 100.0f));
    m_windowController->setDrawFunc(
        [this]()
        {
            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 view = cptrWindowController()->Camera()->GetViewMatrix();
            m_pbrBuilder->initMainShadersEnvs( view, cptrWindowController()->Camera()->Position());

            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(-5.0, 0.0, 2.0));
            m_pbrBuilder->drawSphere(getTexture("Iron"), *m_sphereVertices, model);

            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(-3.0, 0.0, 2.0));
            m_pbrBuilder->drawSphere(getTexture("Gold"), *m_sphereVertices, model);

            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(-1.0, 0.0, 2.0));
            m_pbrBuilder->drawSphere(getTexture("Grass"), *m_sphereVertices, model);

            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(1.0, 0.0, 2.0));
            m_pbrBuilder->drawSphere(getTexture("Marble"), *m_sphereVertices, model);

            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(3.0, 0.0, 2.0));
            m_pbrBuilder->drawSphere(getTexture("Wood"), *m_sphereVertices, model);

            // render light source (simply re-render sphere at light positions)
            // this looks a bit off as we use the same shader, but it'll make their positions obvious and
            // keeps the codeprint small.
            glm::mat3 normalMatrix;
            for (uint32_t i = 0; i < LightsPostions().size(); ++i)
            {
                glm::vec3 newPos = LightsPostions()[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
                //newPos = lightPositions[i];
                std::string str = "lightPositions[" + std::to_string(i) + "]";
                m_pbrBuilder->Shader().MainShader()->setVec3(str.c_str(), newPos);
                str = "lightColors[" + std::to_string(i) + "]";
                m_pbrBuilder->Shader().MainShader()->setVec3(str.c_str(), LightsColors()[i]);

                model = glm::mat4(1.0f);
                model = glm::translate(model, newPos);
                model = glm::scale(model, glm::vec3(0.5f));
                m_pbrBuilder->drawSphere(getTexture("Wood"), *m_sphereVertices, model);
            }

            m_pbrBuilder->renderSkybox(view);
            //m_pbrBuilder->renderEtoC(view);
        }
    );

}

PBRTextures const & TSandbox::getTexture(std::string const & _textureName ) const
{
    return m_texturesMap.at(_textureName);
}

std::vector<glm::vec3> const & TSandbox::LightsPostions() const
{
    return m_lightPositions;
}

std::vector<glm::vec3> const & TSandbox::LightsColors() const
{
    return m_lightColors;
}

void TSandbox::start()
{
    m_windowController->play();
}

std::unique_ptr<TWindowController> const & TSandbox::cptrWindowController()
{
    return m_windowController;
}
