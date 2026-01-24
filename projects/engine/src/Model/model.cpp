#include "model.hpp"
#include "textures/texture.hpp"
#include <algorithm>
#include <cctype>

GLuint TextureFromFile(const char* filename, std::string path)
{
    TTextureBuilder textureBuilder;
    textureBuilder.setWrapS(GL_REPEAT);
    textureBuilder.setWrapT(GL_REPEAT);
    textureBuilder.setMinFilter(GL_NEAREST);
    textureBuilder.setMagFilter(GL_NEAREST);
    GLuint texture = textureBuilder.MakeTexture(path, filename);
    return texture;
}


TModel::TModel(const char *path)
{
    Load(path);
}

TModel::TModel(std::string const & path)
{
    Load(path);
}

// Copy Constructor
TModel::TModel(const TModel& other)
    : m_meshes(other.m_meshes)
    , m_directory(other.m_directory)
    , m_textures_loaded(other.m_textures_loaded)
    , m_scene(nullptr)  // Can't copy Assimp scene
    , m_importer()
    , m_isLoaded(other.m_isLoaded)
    , m_embeddedTextures()
{
    // Note: We can't copy the Assimp scene, so we need to reload if the other model was loaded
    if (other.m_isLoaded && !other.m_directory.empty())
    {
        // This is a shallow copy - the meshes and textures are copied
        // but the Assimp scene needs to be reloaded if needed
        // For embedded textures, we'd need to deep copy them
        for (auto* texData : other.m_embeddedTextures)
        {
            // Deep copy embedded textures if needed
            // This is a simplified approach
        }
    }
}

// Copy Assignment Operator
TModel& TModel::operator=(const TModel& other)
{
    if (this != &other)
    {
        // Clean up existing resources
        for (auto* texData : m_embeddedTextures)
        {
            delete[] texData;
        }
        m_embeddedTextures.clear();

        // Copy data
        m_meshes = other.m_meshes;
        m_directory = other.m_directory;
        m_textures_loaded = other.m_textures_loaded;
        m_scene = nullptr;  // Can't copy Assimp scene
        m_isLoaded = other.m_isLoaded;

        // Copy embedded textures
        for (auto* texData : other.m_embeddedTextures)
        {
            // Deep copy if needed
        }
    }
    return *this;
}

// Move Constructor
TModel::TModel(TModel&& other) noexcept
    : m_meshes(std::move(other.m_meshes))
    , m_directory(std::move(other.m_directory))
    , m_textures_loaded(std::move(other.m_textures_loaded))
    , m_scene(other.m_scene)
    , m_importer()  // Assimp::Importer cannot be moved, create new one
    , m_isLoaded(other.m_isLoaded)
    , m_embeddedTextures(std::move(other.m_embeddedTextures))
{
    other.m_scene = nullptr;
    other.m_isLoaded = false;
}

// Move Assignment Operator
TModel& TModel::operator=(TModel&& other) noexcept
{
    if (this != &other)
    {
        // Clean up existing resources
        for (auto* texData : m_embeddedTextures)
        {
            delete[] texData;
        }
        m_embeddedTextures.clear();

        // Move data
        m_meshes = std::move(other.m_meshes);
        m_directory = std::move(other.m_directory);
        m_textures_loaded = std::move(other.m_textures_loaded);
        m_scene = other.m_scene;
        // Assimp::Importer cannot be moved, it will be default constructed
        m_isLoaded = other.m_isLoaded;
        m_embeddedTextures = std::move(other.m_embeddedTextures);

        // Reset moved-from object
        other.m_scene = nullptr;
        other.m_isLoaded = false;
    }
    return *this;
}

// Destructor
TModel::~TModel()
{
    // Clean up embedded textures
    for (auto* texData : m_embeddedTextures)
    {
        delete[] texData;
    }
    m_embeddedTextures.clear();
}

bool TModel::Load(const std::string& path)
{
    // Clear previous data
    m_meshes.clear();
    m_textures_loaded.clear();
    m_embeddedTextures.clear();
    m_isLoaded = false;

    std::string extension = getFileExtension(path);
    unsigned int flags = getAssimpFlags(extension);

    m_scene = m_importer.ReadFile(path.c_str(), flags);

    if(!m_scene || m_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !m_scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << m_importer.GetErrorString() << std::endl;
        return false;
    }

    m_directory = path.substr(0, path.find_last_of('/'));

    // Process FBX specific features
    if (extension == "fbx")
    {
        processFBXScene(m_scene);
    }

    processNode(m_scene->mRootNode, m_scene);
    m_isLoaded = true;
    return true;
}

bool TModel::loadModel(const std::string& path)
{
    return Load(path);
}

void TModel::processNode(aiNode *node, const aiScene *scene)
{
    // Process all meshes in the node (if any)
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        m_meshes.push_back(processMesh(mesh, scene));
    }
    // Process children nodes recursively
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

TMesh TModel::processMesh(aiMesh *mesh, const aiScene *scene)
{
    std::vector<TVertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<STexture> textures;

    for(unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        TVertex vertex;
        // Process vertex positions, normals and texture coordinates
        glm::vec3 vector;
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;

        // Handle normals
        if (mesh->HasNormals())
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
        }
        else
        {
            vertex.Normal = glm::vec3(0.0f, 1.0f, 0.0f); // Default normal
        }

        // Handle texture coordinates
        if(mesh->mTextureCoords[0] && mesh->mNumUVComponents[0] >= 2)
        {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
        }
        else
        {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }
        vertices.push_back(vertex);
    }

    // Process indices
    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    // Process material
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

        // Load different texture types
        std::vector<STexture> diffuseMaps = loadMaterialTextures(material,
                                            aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        std::vector<STexture> specularMaps = loadMaterialTextures(material,
                                            aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

        std::vector<STexture> normalMaps = loadMaterialTextures(material,
                                            aiTextureType_NORMALS, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

        std::vector<STexture> heightMaps = loadMaterialTextures(material,
                                            aiTextureType_HEIGHT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
    }

    return TMesh(vertices, indices, textures);
}

std::vector<STexture> TModel::loadMaterialTextures(aiMaterial *mat, aiTextureType type, const std::string& typeName)
{
    std::vector<STexture> textures;
    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = false;
        for(unsigned int j = 0; j < m_textures_loaded.size(); j++)
        {
            if(std::strcmp(m_textures_loaded[j].path.C_Str(), str.C_Str()) == 0)
            {
                textures.push_back(m_textures_loaded[j]);
                skip = true;
                break;
            }
        }
        if(!skip)
        {   // Load texture if not already loaded
            STexture texture;
            texture.id = TextureFromFile(str.C_Str(), m_directory);
            texture.type = typeName;
            texture.path = str;
            textures.push_back(texture);
            // Add texture to loaded textures list
            m_textures_loaded.push_back(texture);
        }
    }
    return textures;
}

// FBX Specific Methods
void TModel::processFBXScene(const aiScene* scene)
{
    // Process embedded textures
    processEmbeddedTextures(scene);

    // TODO: Process animations, bones, etc.
}

void TModel::processEmbeddedTextures(const aiScene* scene)
{
    // Handle embedded textures in FBX files
    for (unsigned int i = 0; i < scene->mNumTextures; ++i)
    {
        const aiTexture* texture = scene->mTextures[i];
        if (texture->mHeight == 0) // Compressed texture
        {
            // Store compressed texture data
            unsigned char* data = new unsigned char[texture->mWidth];
            memcpy(data, texture->pcData, texture->mWidth);
            m_embeddedTextures.push_back(data);
        }
    }
}

std::vector<STexture> TModel::loadEmbeddedTexture(aiMaterial* mat, aiTextureType type, const std::string& typeName)
{
    std::vector<STexture> textures;
    // Implementation for loading embedded textures
    // This would handle FBX embedded texture data
    return textures;
}

// Helper Methods
std::string TModel::getFileExtension(const std::string& path) const
{
    size_t dotPos = path.find_last_of('.');
    if (dotPos != std::string::npos)
    {
        std::string ext = path.substr(dotPos + 1);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        return ext;
    }
    return "";
}

unsigned int TModel::getAssimpFlags(const std::string& extension) const
{
    unsigned int flags = aiProcess_Triangulate | aiProcess_FlipUVs;

    // Add FBX-specific processing flags
    if (extension == "fbx")
    {
        flags |= aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace;
    }

    return flags;
}

// Getter Methods
std::vector<TMesh>& TModel::Meshes()
{
    return m_meshes;
}

const std::vector<TMesh>& TModel::Meshes() const
{
    return m_meshes;
}

std::vector<STexture>& TModel::TextureLoaded()
{
    return m_textures_loaded;
}

const std::vector<STexture>& TModel::TextureLoaded() const
{
    return m_textures_loaded;
}

const std::string& TModel::GetDirectory() const
{
    return m_directory;
}

bool TModel::IsLoaded() const
{
    return m_isLoaded;
}

// FBX Specific Getters
bool TModel::HasAnimations() const
{
    return m_scene && m_scene->mNumAnimations > 0;
}

unsigned int TModel::GetAnimationCount() const
{
    return m_scene ? m_scene->mNumAnimations : 0;
}

const aiAnimation* TModel::GetAnimation(unsigned int index) const
{
    if (m_scene && index < m_scene->mNumAnimations)
    {
        return m_scene->mAnimations[index];
    }
    return nullptr;
}

bool TModel::HasEmbeddedTextures() const
{
    return m_scene && m_scene->mNumTextures > 0;
}

unsigned int TModel::GetMaterialCount() const
{
    return m_scene ? m_scene->mNumMaterials : 0;
}

// Utility Function
bool IsModelFormatSupported(const std::string& path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path.c_str(), 0);
    return scene != nullptr;
}

void TModel::Draw(TShader & shader)
{
    if (!m_isLoaded) return;

    for(unsigned int i = 0; i < m_meshes.size(); i++)
    {
        m_meshes[i].Draw(shader);
    }
}
