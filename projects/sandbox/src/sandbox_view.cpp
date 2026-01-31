#include "sandbox_view.hpp"

#include <camera/camera_factory.hpp>
#include "common/vertices.hpp"
#include "model/model.hpp"
#include <shaders/shader_operations.hpp>
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
    GLfloat windowRatio = static_cast<GLfloat>(cptrWindowController()->Width()) / cptrWindowController()->Height();
    m_windowController->setCamera(createCamera(TCameraType::DEFAULT_CAMERA, glm::vec3(0.0f, 0.0f, 3.0f), windowRatio));
    m_windowController->init();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL); // set depth function to less than AND equal for skybox depth trick.
    // enable seamless cubemap sampling for lower mip levels in the pre-filter map.
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    // lights
    // ------
    m_scene = std::make_shared<TScene>();
    m_scene->addLight(TLight(glm::vec3(-10.0f,  10.0f, 10.0f),glm::vec3(300.0f, 300.0f, 300.0f)));
    m_scene->addLight(TLight(glm::vec3( 10.0f,  10.0f, 10.0f),glm::vec3(300.0f, 300.0f, 300.0f)));
    m_scene->addLight(TLight(glm::vec3(-10.0f, -10.0f, 10.0f),glm::vec3(300.0f, 300.0f, 300.0f)));
    m_scene->addLight(TLight(glm::vec3( 10.0f, -10.0f, 10.0f),glm::vec3(300.0f, 300.0f, 300.0f)));

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

    m_pbrBuilder->initCuptureBuffer();
    m_pbrBuilder->setCubeVAO(cubeVertices.VAO());
    m_pbrBuilder->convertEtoC(hdrTexture);
    m_pbrBuilder->setProjection(*cptrWindowController()->Camera()->ProjectionCptr());

    TSceneModel cyborg = TSceneModel("./models/cyborg/Cyborg.fbx");

    // Set up transformation matrices for cyborg
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f)); // Position the cyborg
    model = glm::scale(model, glm::vec3(0.1f)); // Scale it down
    cyborg.setModelMatrix(model);
    cyborg.setPojectionMatrixCptr(cptrWindowController()->Camera()->ProjectionCptr());
    cyborg.setShader(createSimpleModelShader());
    cyborg.setScene(m_scene);
    m_modelsMap.emplace("Man", cyborg);

    m_windowController->setDrawFunc(
        [this]()
        {
            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 view = cptrWindowController()->Camera()->GetViewMatrix();
            m_pbrBuilder->initMainShadersEnvs( view, cptrWindowController()->Camera()->Position());

            m_pbrBuilder->Shader().MainShader()->Use();
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

            // Draw the cyborg model
            TSceneModel& cyborgModel = getModel("Man");
            cyborgModel.Update(view, cptrWindowController()->Camera()->Position());

            if (cyborgModel.IsLoaded())
            {
                cyborgModel.Draw();
            }
            else
            {
                std::cout << "Warning: Cyborg model not loaded properly" << std::endl;
            }

            // render light source (simply re-render sphere at light positions)
            // this looks a bit off as we use the same shader, but it'll make their positions obvious and
            // keeps the codeprint small.
            m_pbrBuilder->Shader().MainShader()->Use();
            for (uint32_t i = 0; i < Scene()->Lights().size(); ++i)
            {
                m_scene->moveLight(i, glm::vec3(sin(glfwGetTime()), 0.0, 0.0));
                std::string str = "lightPositions[" + std::to_string(i) + "]";
                m_pbrBuilder->Shader().MainShader()->setVec3(str.c_str(), Scene()->Lights().at(i).Position());
                str = "lightColors[" + std::to_string(i) + "]";
                m_pbrBuilder->Shader().MainShader()->setVec3(str.c_str(), Scene()->Lights().at(i).Color());

                model = glm::mat4(1.0f);
                model = glm::translate(model, Scene()->Lights().at(i).Position());
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

TSceneModel & TSandbox::getModel(std::string const & _modelName )
{
    return m_modelsMap.at(_modelName);
}

void TSandbox::start()
{
    m_windowController->play();
}

std::unique_ptr<TWindowController> const & TSandbox::cptrWindowController()
{
    return m_windowController;
}
