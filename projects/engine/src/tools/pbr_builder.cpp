#include"pbr_builder.hpp"
#include"common/vertices.hpp"

void DrawSphere( PBRTextures const& _textures, TSphereVertices const & _sphereVertices, glm::mat4 const & _model )
{
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, _textures.albedo);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, _textures.normal);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, _textures.metallic);
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, _textures.roughness);
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, _textures.ao);

    TShaderCollection::Instance().MainShader()->setMat4("model", _model);
    glm::mat3 normalMatrix  = glm::transpose(glm::inverse(glm::mat3(_model)));
    TShaderCollection::Instance().MainShader()->setMat3("normalMatrix", normalMatrix);
    glBindVertexArray(_sphereVertices.VAO());
    glDrawElements(GL_TRIANGLE_STRIP, _sphereVertices.Indices().size(), GL_UNSIGNED_INT, 0);
}
