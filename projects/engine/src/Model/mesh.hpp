#pragma once
#include <string>
#include <vector>
#include "../shaders/shader.hpp"
// GL Includes
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>


struct TVertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct STexture {
    uint32_t id;
    std::string type;
    aiString  path;
};

class TMesh
{
    public:
        /*  Mesh Data  */
        /*  Functions  */
        TMesh(std::vector<TVertex> vertices, std::vector<uint32_t> indices, std::vector<STexture> textures);
        void Draw(TShader & shader);
        unsigned int getVAO() const
        {
            return VAO;
        }
        std::vector<uint32_t> const & Indeces() const
        {
            return m_indices;
        }
    private: //methods
        /*  Render data  */
        unsigned int VAO, VBO, EBO;
        /*  Functions    */
        void setupMesh();
    private: //attributes
        std::vector<TVertex> m_vertices{};
        std::vector<uint32_t> m_indices{};
        std::vector<STexture> m_textures{};
};
