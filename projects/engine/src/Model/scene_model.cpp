#include <model/scene_model.hpp>

TSceneModel::TSceneModel() : TModel()
{
}

TSceneModel::TSceneModel(std::string const & path) : TModel(path)
{
}

TSceneModel::TSceneModel(const TSceneModel& other) : TModel(static_cast<const TModel&>(other))
{
    m_modelMatrix = other.m_modelMatrix;
    m_shader = other.m_shader;
    m_sceneCptr = other.m_sceneCptr;
}

TSceneModel& TSceneModel::operator=(const TSceneModel& other)
{
    if (this != &other)
    {
        // Copy base class
        TModel::operator=(static_cast<const TModel&>(other));

        // Copy member variables
        m_modelMatrix = other.m_modelMatrix;
        m_shader = other.m_shader;
        m_sceneCptr = other.m_sceneCptr;
    }
    return *this;
}

TSceneModel::TSceneModel(TSceneModel&& other) noexcept : TModel(std::move(static_cast<TModel&>(other)))
{
    m_modelMatrix = std::move(other.m_modelMatrix);
    m_shader = std::move(other.m_shader);
    m_sceneCptr = std::move(other.m_sceneCptr);
}

TSceneModel& TSceneModel::operator=(TSceneModel&& other) noexcept
{
    if (this != &other)
    {
        // Move base class
        TModel::operator=(std::move(static_cast<TModel&>(other)));

        // Move member variables
        m_modelMatrix = std::move(other.m_modelMatrix);
        m_shader = std::move(other.m_shader);
        m_sceneCptr = std::move(other.m_sceneCptr);
    }
    return *this;
}

void TSceneModel::Update()
{
    glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(ModelMatrix())));

    Shader()->Use();
    Shader()->setMat4("projection", *SceneCptr()->Camera()->ProjectionCptr());
    Shader()->setMat4("view", SceneCptr()->Camera()->ViewMatrix());
    Shader()->setMat4("model", ModelMatrix());
    Shader()->setMat3("normalMatrix", normalMatrix);

    // Set lighting
    for (uint32_t i = 0; i < SceneCptr()->Lights().size(); ++i)
    {
        std::string str = "lightPositions[" + std::to_string(i) + "]";
        Shader()->setVec3(str.c_str(), SceneCptr()->Lights().at(i).Position());
        str = "lightColors[" + std::to_string(i) + "]";
        Shader()->setVec3(str.c_str(), SceneCptr()->Lights().at(i).Color());
    }
    Shader()->setVec3("viewPos", SceneCptr()->Camera()->Position());


    // Set material properties
    Shader()->setVec3("material_ambient", glm::vec3(0.2f, 0.2f, 0.2f));
    Shader()->setVec3("material_diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
    Shader()->setVec3("material_specular", glm::vec3(1.0f, 1.0f, 1.0f));
    Shader()->setFloat("material_shininess", 32.0f);

    SetupBones(*Shader());
}

void TSceneModel::Draw( )
{
    Shader()->Use();
    TModel::Draw(*Shader());
}
