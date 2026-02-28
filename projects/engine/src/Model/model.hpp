#pragma once
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/DefaultLogger.hpp>
#include <string>
#include <vector>
#include <map>
#include <model/mesh.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <shaders/shader.hpp>
#include <textures/texture.hpp>

struct TBoneInfo {
    glm::mat4 offset_matrix;
    glm::mat4 final_transform;
    std::string name;
    int parent_id; // -1 if root

    TBoneInfo() : offset_matrix(1.0f), final_transform(1.0f), parent_id(-1) {}
};

// Structure to hold individual subject/node data
struct TSubject {
    std::vector<TMesh> meshes;
    std::string name;
    glm::mat4 transform;
    std::vector<STexture> textures; // Per-subject textures

    TSubject() : name(""), transform(1.0f) {}

    // Method to add a texture to this subject
    void AddTexture(const STexture& texture) {
        textures.push_back(texture);
    }

    // Method to clear all textures for this subject
    void ClearTextures() {
        textures.clear();
    }

    // Method to get subject textures
    const std::vector<STexture>& GetTextures() const {
        return textures;
    }
};

class TModel {
public:
    TModel() = default;
    explicit TModel(const std::string& path);

    /*  Copy Semantics (Rule of 5)   */
    TModel(TModel const & other);
    TModel& operator=(const TModel& other);
    TModel(TModel&& other) noexcept;
    TModel& operator=(TModel&& other) noexcept;
    virtual ~TModel() = default;

    void LoadModel(const std::string& path);
    void Draw(TShader& shader);
    void DrawSubject(const std::string& subjectName, TShader& shader);

    // Getters
    const std::vector<STexture>& GetTextures() const { return textures_loaded; }
    const std::vector<TBoneInfo>& GetBones() const { return bones; }
    virtual bool IsLoaded() const;

    // Subject/object management
    const std::vector<TSubject>& GetSubjects() const { return subjects; }
    const TSubject* GetSubject(const std::string& name) const;
    std::vector<std::string> GetSubjectNames() const;

    bool AssignTextureToCharacter(const std::string& subjectName, const STexture& texture);
    bool AssignTexturesToCharacter(const std::string& subjectName, const std::vector<STexture>& textures);
    bool ReplaceCharacterTextures(const std::string& subjectName, const std::vector<STexture>& textures);

    // Bone/Skeleton methods
    void SetupBones(TShader& shader);

    // Animation-related
    bool HasAnimations() const { return scene && scene->mNumAnimations > 0; }
    uint32_t GetAnimationCount() const { return scene ? scene->mNumAnimations : 0; }
    const aiAnimation* GetAnimation(uint32_t index) const;

private:
    // Core data
    std::string directory;
    std::vector<STexture> textures_loaded;

    // Subject objects/nodes data
    std::vector<TSubject> subjects;

    // Bone and animation data
    std::vector<TBoneInfo> bones;
    std::map<std::string, uint32_t> bone_mapping; // name to index
    std::map<std::string, uint32_t> bone_id_mapping; // node name to bone index

    // Assimp data
    const aiScene* scene = nullptr;
    Assimp::Importer importer;
    bool m_isLoaded = false;

    // Private methods
    void processNode(aiNode* node, const glm::mat4& parentTransform = glm::mat4(1.0f));
    TMesh processMesh(aiMesh* mesh);
    void processBones(aiMesh* mesh);
    void processNodeHierarchy(aiNode* node, const glm::mat4& parent_transform = glm::mat4(1.0f));
    void updateBoneTransforms();

    std::vector<STexture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName);
    uint32_t TextureFromFile(const char* path);
    std::string getFileExtension(const std::string& path) const;
    uint32_t getAssimpFlags(const std::string& extension) const;
};
