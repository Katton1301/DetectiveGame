#pragma once
#include <string>
#include <vector>
#include "../shaders/shader.hpp"
// GL Includes
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


struct TVertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    // Bone influences for skeletal animation
    glm::ivec4 BoneIDs;
    glm::vec4 Weights;
    // Constructor with default values
    TVertex() : Position(0.0f), Normal(0.0f), TexCoords(0.0f),
              BoneIDs(-1), Weights(0.0f) {}
};

struct STexture {
    uint32_t id;
    std::string type;
    std::string path;
};

class TMesh
{
    public:
        /*  Mesh Data  */
        /*  Functions  */
        TMesh(std::vector<TVertex> vertices, std::vector<uint32_t> indices, std::vector<STexture> textures);
        void Draw(TShader & shader);

        void AddTexture(STexture const & texture)
        {
            m_textures.push_back(texture);
        }

        void ClearTextures()
        {
            m_textures.clear();
        }

        const std::vector<STexture>& GetTextures() const {
            return m_textures;
        }

        uint32_t getVAO() const
        {
            return VAO;
        }
        std::vector<uint32_t> const & Indeces() const
        {
            return m_indices;
        }
    private: //methods
        /*  Render data  */
        uint32_t VAO, VBO, EBO;
        /*  Functions    */
        void setupMesh();
    private: //attributes
        std::vector<TVertex> m_vertices{};
        std::vector<uint32_t> m_indices{};
        std::vector<STexture> m_textures{};
};
