#pragma once
// GL Includes
#include <GL/glew.h>
#include <glm/glm.hpp>

class TLight
{
public:
    TLight()
        : m_position(glm::vec3(0.0f, 0.0f, 0.0f))
        , m_color(glm::vec3(0.0f, 0.0f, 0.f))
    {}
    TLight( glm::vec3 const & _position, glm::vec3 const & _color )
        : m_position(_position)
        , m_color(_color)
    {}
    virtual ~TLight() = default;
    void setPosition( glm::vec3 const & _position ){ m_position = _position; }
    void setColor( glm::vec3 const & _color ){ m_color = _color;}

    glm::vec3 & Position( ) { return m_position; }
    glm::vec3 const & Position( ) const { return m_position; }
    glm::vec3 & Color( ) { return m_color; }
    glm::vec3 const & Color( ) const { return m_color; }


private:
    glm::vec3 m_position{};
    glm::vec3 m_color{};
};
