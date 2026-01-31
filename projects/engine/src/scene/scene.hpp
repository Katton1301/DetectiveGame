#pragma once
#include <scene/scene_interface.hpp>

class TScene : public IScene
{
public:
    TScene() = default;
    virtual ~TScene() = default;
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
    std::vector< TLight > const & Lights() const override {return m_lights;}


private:
    std::vector< TLight > m_lights;
};
