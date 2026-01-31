#pragma once
#include <windows/window_controller.hpp>
#include <tools/pbr_builder.hpp>
#include <textures/texture.hpp>
#include <model/scene_model.hpp>
#include <scene/scene.hpp>

class TSandbox
{
public:
    TSandbox();
    ~TSandbox() = default;
    void createWindow( uint32_t width, uint32_t height );
    void init();
    void start();
    std::shared_ptr<TPBRBuilder> & PBR() {return m_pbrBuilder;}

private: //methods
    std::unique_ptr<TWindowController> const & cptrWindowController();
    std::shared_ptr<TScene> const & Scene( ) const { return m_scene; }
    PBRTextures const & getTexture(std::string const & _textureName ) const;
    TSceneModel & getModel(std::string const & _modelName );


private: //attributes
    std::unique_ptr<TWindowController> m_windowController = nullptr;
    std::shared_ptr<TPBRBuilder> m_pbrBuilder;
    std::map< std::string, PBRTextures > m_texturesMap{};
    std::map< std::string, TSceneModel > m_modelsMap{};
    std::shared_ptr<TSphereVertices> m_sphereVertices;
    std::shared_ptr<TScene> m_scene{};
};
