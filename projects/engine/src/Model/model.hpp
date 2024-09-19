#pragma once
#include "mesh.hpp"
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class TModel
{
    public:
        /*  Методы   */
        TModel(const char *path)
        {
            loadModel(path);
        }
        void Draw(TShader & shader);
        std::vector<TMesh> & Meshes( )
        {
            return m_meshes;
        }
        std::vector<TTexture> & TextureLoaded( )
        {
            return m_textures_loaded;
        }
    private: //method

        /*  Методы   */
        void loadModel(std::string path);
        void processNode(aiNode *node, const aiScene *scene);
        TMesh processMesh(aiMesh *mesh, const aiScene *scene);
        std::vector<TTexture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);

    private:
        /*  Данные модели  */
        std::vector<TMesh> m_meshes{};
        std::string m_directory{};
        std::vector<TTexture> m_textures_loaded{};
};
