#pragma once
#include<windows/window_controller.hpp>
#include "tools/pbr_builder.hpp"
#include "textures/texture.hpp"

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
    std::vector<glm::vec3> const & LightsPostions() const;
    std::vector<glm::vec3> const & LightsColors() const;
    PBRTextures const & getTexture(std::string const & _textureName ) const;




private: //attributes
    std::unique_ptr<TWindowController> m_windowController = nullptr;
    std::shared_ptr<TPBRBuilder> m_pbrBuilder;
    std::map< std::string, PBRTextures > m_texturesMap{};
    std::shared_ptr<TSphereVertices> m_sphereVertices;
    std::vector<glm::vec3> m_lightPositions{};
    std::vector<glm::vec3> m_lightColors{};
};
