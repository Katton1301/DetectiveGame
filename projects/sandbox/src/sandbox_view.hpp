#pragma once
#include <windows/window_controller.hpp>
#include <tools/pbr_builder.hpp>
#include <model/skybox.hpp>
#include <textures/texture.hpp>
#include <model/scene_model.hpp>
#include <model/mirror.hpp>
#include <scene/scene.hpp>

class TSandbox
{
public:
    TSandbox();
    ~TSandbox() = default;
    void createWindow( uint32_t width, uint32_t height );
    void init();
    void start();
    std::shared_ptr<TSkyBox> & SkyBox() {return m_skyBox;}

private: //methods
    std::unique_ptr<TWindowController> const & cptrWindowController();
    std::shared_ptr<TScene> const & Scene( ) const { return m_scene; }
    PBRTextures const & getTexture(std::string const & _textureName ) const;
    std::shared_ptr<TSceneModel> & getModel(std::string const & _modelName );


private: //attributes
    std::unique_ptr<TWindowController> m_windowController = nullptr;
    std::shared_ptr<TSkyBox> m_skyBox;
    std::map< std::string, PBRTextures > m_texturesMap{};
    std::map< std::string, std::shared_ptr<TSceneModel> > m_modelsMap{};
    std::shared_ptr<TSphereVertices> m_sphereVertices;
    std::shared_ptr<TScene> m_scene{};
    std::unique_ptr<TMirror> m_mirror;
};
