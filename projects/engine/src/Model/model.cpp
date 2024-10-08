#include "model.hpp"
#include "textures/texture.hpp"

GLuint TextureFromFile(const char* filename, std::string path)
{
    TTextureBuilder textureBuilder = TTextureBuilder(path);
    textureBuilder.changeWrapS(GL_REPEAT);
    textureBuilder.changeWrapT(GL_REPEAT);
    textureBuilder.changeMinFilter(GL_NEAREST);
    textureBuilder.changeMagFilter(GL_NEAREST);
    GLuint texture = textureBuilder.MakeTexture(filename);
    return texture;
}


void TModel::loadModel(std::string path)
{
    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        return;
    }
    m_directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

void TModel::processNode(aiNode *node, const aiScene *scene)
{
    // обработать все полигональные сетки в узле(если есть)
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        m_meshes.push_back(processMesh(mesh, scene));
    }
    // выполнить ту же обработку и для каждого потомка узла
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
        // обработка координат, нормалей и текстурных координат вершин
        glm::vec3 vector;
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;
        vector.x = mesh->mNormals[i].x;
        vector.y = mesh->mNormals[i].y;
        vector.z = mesh->mNormals[i].z;
        vertex.Normal = vector;
        if(mesh->mTextureCoords[0]) // сетка обладает набором текстурных координат?
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
    // орбаботка индексов
    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }
    // обработка материала
    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
    std::vector<STexture> diffuseMaps = loadMaterialTextures(material,
                                        aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    std::vector<STexture> specularMaps = loadMaterialTextures(material,
                                        aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

    return TMesh(vertices, indices, textures);
}

std::vector<STexture> TModel::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
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
        {   // если текстура не была загружена – сделаем это
            STexture texture;
            texture.id = TextureFromFile(str.C_Str(), m_directory);
            texture.type = typeName;
            texture.path = str;
            textures.push_back(texture);
// занесем текстуру в список уже загруженных
            m_textures_loaded.push_back(texture);
        }
    }
    return textures;
}

void TModel::Draw(TShader & shader)
{
    for(unsigned int i = 0; i < m_meshes.size(); i++)
    {
        m_meshes[i].Draw(shader);
    }
}
