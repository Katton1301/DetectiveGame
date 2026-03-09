#pragma once
#include <scene/scene_interface.hpp>

class TScene : public IScene
{
public:
    TScene() = default;
    virtual ~TScene() = default;
    void setCameraCptr( std::shared_ptr<ICamera> const & _cameraCptr )
    {
        m_cameraCptr = _cameraCptr;
    }
    void setSkyBoxCptr( std::shared_ptr<TSkyBox> const & _skyBoxCptr )
    {
        m_skyBoxCptr = _skyBoxCptr;
    }
    void addLight(  TLight const & _light )
    {
        m_lights.push_back(_light);
    }
    void addLight( TLight && _light )
    {
        m_lights.push_back(_light);
    }
    void moveLight(uint32_t lightId, glm::vec3 step )
    {
        assert(lightId < m_lights.size());
        m_lights[lightId].Position() += step;
    }

    void addSceneObject(std::shared_ptr<ISceneObject> const & _sceneObject)
    {
        m_sceneObjects.push_back(_sceneObject);
    }

    std::vector< TLight > const & Lights() const override { return m_lights; }
    std::shared_ptr<ICamera> const & Camera() const override { return m_cameraCptr; }
    std::shared_ptr<TSkyBox> const & SkyBox() const override { return m_skyBoxCptr; }
    std::vector<std::shared_ptr< ISceneObject >> const & SceneObjects() const override { return m_sceneObjects; }


private:
    std::vector< TLight > m_lights{};
    std::shared_ptr<ICamera> m_cameraCptr{};
    std::shared_ptr<TSkyBox> m_skyBoxCptr{};
    std::vector<std::shared_ptr< ISceneObject >> m_sceneObjects{};

};
