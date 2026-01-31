#pragma once
#include "mesh.hpp"
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <string>
#include <vector>
#include <iostream>

class TModel
{
    public:
        /*  Constructors   */
        TModel() = default;
        TModel(std::string const & path);

        /*  Copy Semantics (Rule of 5)   */
        TModel(const TModel& other);
        TModel& operator=(const TModel& other);
        TModel(TModel&& other) noexcept;
        TModel& operator=(TModel&& other) noexcept;
        virtual ~TModel();

        /*  Public Methods   */
        bool Load(const std::string& path);
        void Draw(TShader & shader);

        /*  Getters   */
        std::vector<TMesh>& Meshes();
        const std::vector<TMesh>& Meshes() const;
        std::vector<STexture>& TextureLoaded();
        const std::vector<STexture>& TextureLoaded() const;
        const std::string& GetDirectory() const;
        virtual bool IsLoaded() const;

        /*  FBX Specific Features   */
        bool HasAnimations() const;
        unsigned int GetAnimationCount() const;
        const aiAnimation* GetAnimation(unsigned int index) const;

        /*  Material Enhancements   */
        bool HasEmbeddedTextures() const;
        unsigned int GetMaterialCount() const;
    private: //methods

        /*  Loading Methods   */
        bool loadModel(const std::string& path);
        void processNode(aiNode *node, const aiScene *scene);
        TMesh processMesh(aiMesh *mesh, const aiScene *scene);
        std::vector<STexture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, const std::string& typeName);

        /*  FBX Specific Processing   */
        void processFBXScene(const aiScene* scene);
        void processEmbeddedTextures(const aiScene* scene);
        std::vector<STexture> loadEmbeddedTexture(aiMaterial* mat, aiTextureType type, const std::string& typeName);

        /*  Helper Methods   */
        std::string getFileExtension(const std::string& path) const;
        unsigned int getAssimpFlags(const std::string& extension) const;

    private:
        /*  Model Data  */
        std::vector<TMesh> m_meshes{};
        std::string m_directory{};
        std::vector<STexture> m_textures_loaded{};

        /*  FBX Specific Data  */
        const aiScene* m_scene = nullptr;
        Assimp::Importer m_importer;
        bool m_isLoaded = false;
        std::vector<unsigned char*> m_embeddedTextures{};
};

// Utility function to check if a file format is supported
bool IsModelFormatSupported(const std::string& path);
