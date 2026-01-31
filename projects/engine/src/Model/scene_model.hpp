#pragma once
#include <model/model.hpp>
#include <memory>
#include <scene/scene_interface.hpp>

class TSceneModel : private TModel
{
public:
    TSceneModel();
    TSceneModel(std::string const & path);
    virtual ~TSceneModel() = default;
    void setPojectionMatrixCptr( std::shared_ptr<glm::mat4> _matrixCptr ){ m_projectionMatrixCptr = _matrixCptr;}
    void setModelMatrix( glm::mat4 const & _matrix ){m_modelMatrix = _matrix;}
    void setShader(HShader _shader){m_shader = _shader;}
    void setScene( std::shared_ptr<IScene> const & _sceneCptr ) { m_sceneCptr = _sceneCptr; }

    void Update(glm::mat4 const & _view, glm::vec3 const & _cameraPos);

    void Draw();
    bool IsLoaded() const override { return TModel::IsLoaded(); }

private:
    glm::mat4 const & ModelMatrix() const {return m_modelMatrix;}
    HShader Shader() const {return m_shader;}
    std::shared_ptr<IScene> const & SceneCptr() const {return m_sceneCptr;}
    glm::mat4 const & ProjectionMatrix() const {return *m_projectionMatrixCptr;}


private:
    glm::mat4 m_modelMatrix;
    std::shared_ptr<const glm::mat4> m_projectionMatrixCptr = nullptr;
    HShader m_shader = nullptr;
    std::shared_ptr<IScene> m_sceneCptr;
};
