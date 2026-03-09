#pragma once
#include <model/model.hpp>
#include <memory>
#include <scene/scene_interface.hpp>
#include <common/scene_object_interface.hpp>

class TSceneModel : public TModel, public ISceneObject
{
public:
    TSceneModel();
    TSceneModel(std::string const & path);

    // Rule of 5 for proper map usage
    TSceneModel(const TSceneModel& other);
    TSceneModel& operator=(const TSceneModel& other);
    TSceneModel(TSceneModel&& other) noexcept;
    TSceneModel& operator=(TSceneModel&& other) noexcept;
    virtual ~TSceneModel() = default;
    void setModelMatrix( glm::mat4 const & _matrix ){m_modelMatrix = _matrix;}
    void setShader(HShader _shader){m_shader = _shader;}
    void setScene( std::shared_ptr<IScene> const & _sceneCptr ) { m_sceneCptr = _sceneCptr; }

    void Update() override;

    virtual void Draw() override;
    bool IsLoaded() const override { return TModel::IsLoaded(); }

protected:
    glm::mat4 const & ModelMatrix() const {return m_modelMatrix;}
    HShader Shader() const override {return m_shader;}
    std::shared_ptr<IScene> const & SceneCptr() const {return m_sceneCptr;}


private:
    glm::mat4 m_modelMatrix;
    HShader m_shader = nullptr;
    std::shared_ptr<IScene> m_sceneCptr;
};
