#include <model/scene_model.hpp>



TSceneModel::TSceneModel()
{

}

TSceneModel::TSceneModel(std::string const & path) : TModel(path)
{
}

void TSceneModel::Update(glm::mat4 const & _view, glm::vec3 const & _cameraPos)
{
    glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(ModelMatrix())));

    Shader()->Use();
    Shader()->setMat4("projection", ProjectionMatrix());
    Shader()->setMat4("view", _view);
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
     Shader()->setVec3("viewPos", _cameraPos);


    // Set material properties
    Shader()->setVec3("material_ambient", glm::vec3(0.2f, 0.2f, 0.2f));
    Shader()->setVec3("material_diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
    Shader()->setVec3("material_specular", glm::vec3(1.0f, 1.0f, 1.0f));
    Shader()->setFloat("material_shininess", 32.0f);
}

void TSceneModel::Draw( )
{
    Shader()->Use();
    TModel::Draw(*Shader());
}
