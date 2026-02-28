#include <model/model.hpp>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <fstream>

// TModel Implementation
TModel::TModel(const std::string& path) {
    LoadModel(path);
}


// Copy Constructor
TModel::TModel(const TModel& other)
    : directory(other.directory)
    , textures_loaded(other.textures_loaded)
    , subjects(other.subjects)
    , bones(other.bones)
    , bone_mapping(other.bone_mapping)
    , bone_id_mapping(other.bone_id_mapping)
    , scene(nullptr)  // Can't copy Assimp scene
    , importer()
    , m_isLoaded(other.m_isLoaded)
{
    // Note: We can't copy the scene, so we need to reload if the other model was loaded
}

// Copy Assignment Operator
TModel& TModel::operator=(const TModel& other)
{
    if (this != &other)
    {
        // Copy data
        directory = other.directory;
        textures_loaded = other.textures_loaded;
        subjects = other.subjects;
        bones = other.bones;
        bone_mapping = other.bone_mapping;
        bone_id_mapping = other.bone_id_mapping;
        scene = nullptr;  // Can't copy Assimp scene
        m_isLoaded = other.m_isLoaded;
    }
    return *this;
}

// Move Constructor
TModel::TModel(TModel&& other) noexcept
    : directory(std::move(other.directory))
    , textures_loaded(std::move(other.textures_loaded))
    , subjects(std::move(other.subjects))
    , bones(std::move(other.bones))
    , bone_mapping(std::move(other.bone_mapping))
    , bone_id_mapping(std::move(other.bone_id_mapping))
    , scene(other.scene)
    , importer()  // Assimp::Importer cannot be moved, create new one
    , m_isLoaded(other.m_isLoaded)
{
    other.scene = nullptr;
}

// Move Assignment Operator
TModel& TModel::operator=(TModel&& other) noexcept
{
    if (this != &other)
    {

        // Move data
        directory = std::move(other.directory);
        textures_loaded = std::move(other.textures_loaded);
        subjects = std::move(other.subjects);
        bones = std::move(other.bones);
        bone_mapping = std::move(other.bone_mapping);
        bone_id_mapping = std::move(other.bone_id_mapping);
        scene = other.scene;
        // Assimp::Importer cannot be moved, it will be default constructed
        m_isLoaded = other.m_isLoaded;

        // Reset moved-from object
        other.scene = nullptr;
        other.m_isLoaded = false;
    }
    return *this;
}

void TModel::LoadModel(const std::string& path) {
    // Clear existing data
    textures_loaded.clear();
    subjects.clear();
    bones.clear();
    bone_mapping.clear();
    bone_id_mapping.clear();
    m_isLoaded = false;

    // Get file extension
    std::string extension = getFileExtension(path);
    uint32_t flags = getAssimpFlags(extension);

    // Load the scene
    scene = importer.ReadFile(path, flags);


    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        flags |= aiProcess_FlipUVs;
        scene = importer.ReadFile(path, flags);
    }


    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }

    directory = path.substr(0, path.find_last_of('/'));

    // Process the node hierarchy first to set up bone mappings
    processNodeHierarchy(scene->mRootNode);

    // Process all nodes and their meshes
    processNode(scene->mRootNode);
    m_isLoaded = true;
}

void TModel::SetupBones(TShader& shader)
{
    // Pass bone matrices to shader only if we have bone data
    if (bones.empty()) return;

    shader.Use();
    for (uint32_t i = 0; i < bones.size(); ++i)
    {
        std::string name = "bones[" + std::to_string(i) + "]";
        shader.setMat4(name.c_str(), bones[i].final_transform);
    }
}

bool TModel::IsLoaded() const
{
    return m_isLoaded;
}

void TModel::processNode(aiNode* node, const glm::mat4& parentTransform) {
    // Create a transform matrix for this node
    aiMatrix4x4 aiNodeTransform = node->mTransformation;
    glm::mat4 nodeTransform = glm::mat4(
        aiNodeTransform.a1, aiNodeTransform.a2, aiNodeTransform.a3, aiNodeTransform.a4,
        aiNodeTransform.b1, aiNodeTransform.b2, aiNodeTransform.b3, aiNodeTransform.b4,
        aiNodeTransform.c1, aiNodeTransform.c2, aiNodeTransform.c3, aiNodeTransform.c4,
        aiNodeTransform.d1, aiNodeTransform.d2, aiNodeTransform.d3, aiNodeTransform.d4
    );
    
    glm::mat4 globalTransform = parentTransform * nodeTransform;

    // Check if this node has meshes (could be a subject object)
    if (node->mNumMeshes > 0) {
        TSubject subject;
        subject.name = node->mName.C_Str();
        subject.transform = globalTransform;

        // Process all meshes in this node
        for(uint32_t i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            subject.meshes.push_back(processMesh(mesh));
        }

        // Add this subject object to our collection
        subjects.push_back(subject);

    } else {
        // If this node doesn't have meshes, process child nodes directly
        for(uint32_t i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], globalTransform);
        }
    }
}

TMesh TModel::processMesh(aiMesh* mesh) {
    std::vector<TVertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<STexture> textures;

    // Process vertices
    for(uint32_t i = 0; i < mesh->mNumVertices; i++) {
        TVertex vertex;

        // Position
        vertex.Position.x = mesh->mVertices[i].x;
        vertex.Position.y = mesh->mVertices[i].y;
        vertex.Position.z = mesh->mVertices[i].z;

        // Normals
        if(mesh->mNormals) {
            vertex.Normal.x = mesh->mNormals[i].x;
            vertex.Normal.y = mesh->mNormals[i].y;
            vertex.Normal.z = mesh->mNormals[i].z;
        }

        // Texture coordinates
        if(mesh->mTextureCoords[0]) {
            vertex.TexCoords.x = mesh->mTextureCoords[0][i].x;
            vertex.TexCoords.y = mesh->mTextureCoords[0][i].y;
        }

        vertices.push_back(vertex);
    }

    // Process bones and assign to vertices
    if(mesh->mBones && mesh->mNumBones > 0) {
        processBones(mesh);

        // Initialize bone IDs and weights
        for(auto& vertex : vertices) {
            vertex.BoneIDs = glm::ivec4(-1);
            vertex.Weights = glm::vec4(0.0f);
        }

        // Assign bone influences to vertices
        for(uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; boneIndex++) {
            aiBone* bone = mesh->mBones[boneIndex];

            // Find the bone in our bone mapping
            auto it = bone_id_mapping.find(bone->mName.C_Str());
            if(it != bone_id_mapping.end()) {
                uint32_t ourBoneIndex = it->second;

                // Assign bone weights to the affected vertices
                for(uint32_t weightIndex = 0; weightIndex < bone->mNumWeights; weightIndex++) {
                    aiVertexWeight weight = bone->mWeights[weightIndex];
                    uint32_t vertexId = weight.mVertexId;

                    if(vertexId < vertices.size()) {
                        TVertex& vertex = vertices[vertexId];

                        // Find an empty slot for bone assignment
                        for(int i = 0; i < 4; i++) {
                            if(vertex.BoneIDs[i] == -1) {
                                vertex.BoneIDs[i] = ourBoneIndex;
                                vertex.Weights[i] = weight.mWeight;
                                break;
                            }
                        }
                    }
                }
            }
        }

        // Normalize weights so they sum to 1.0
        for(auto& vertex : vertices) {
            float totalWeight = vertex.Weights.x + vertex.Weights.y +
                                vertex.Weights.z + vertex.Weights.w;
            if(totalWeight > 0.0f) {
                vertex.Weights /= totalWeight;
            }
        }
    }

    // Process indices
    for(uint32_t i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for(uint32_t j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // Process materials and textures
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

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

    return TMesh(vertices, indices, textures);
}

void TModel::processBones(aiMesh* mesh) {
    for(uint32_t i = 0; i < mesh->mNumBones; i++) {
        aiBone* bone = mesh->mBones[i];
        std::string boneName = bone->mName.C_Str();

        // Check if this bone is already in our mapping
        if(bone_mapping.find(boneName) == bone_mapping.end()) {
            uint32_t boneIndex = bones.size();
            bones.emplace_back(TBoneInfo{});

            // Store bone offset matrix (inverse bind pose)
            aiMatrix4x4 offsetMatrix = bone->mOffsetMatrix;
            bones[boneIndex].offset_matrix = glm::mat4(
                offsetMatrix.a1, offsetMatrix.a2, offsetMatrix.a3, offsetMatrix.a4,
                offsetMatrix.b1, offsetMatrix.b2, offsetMatrix.b3, offsetMatrix.b4,
                offsetMatrix.c1, offsetMatrix.c2, offsetMatrix.c3, offsetMatrix.c4,
                offsetMatrix.d1, offsetMatrix.d2, offsetMatrix.d3, offsetMatrix.d4
                );

            bones[boneIndex].name = boneName;

            // Map bone name to index
            bone_mapping[boneName] = boneIndex;
        }

        // Map node name to bone index (for later hierarchy processing)
        bone_id_mapping[boneName] = bone_mapping[boneName];
    }
}

void TModel::processNodeHierarchy(aiNode* node, const glm::mat4& parent_transform) {
    std::string nodeName = node->mName.C_Str();

    // Convert aiMatrix to glm::mat4
    aiMatrix4x4 nodeTransform = node->mTransformation;
    glm::mat4 transform = glm::mat4(
        nodeTransform.a1, nodeTransform.a2, nodeTransform.a3, nodeTransform.a4,
        nodeTransform.b1, nodeTransform.b2, nodeTransform.b3, nodeTransform.b4,
        nodeTransform.c1, nodeTransform.c2, nodeTransform.c3, nodeTransform.c4,
        nodeTransform.d1, nodeTransform.d2, nodeTransform.d3, nodeTransform.d4
        );

    glm::mat4 globalTransform = parent_transform * transform;

    // If this node corresponds to a bone, update its transform
    auto it = bone_id_mapping.find(nodeName);
    if(it != bone_id_mapping.end()) {
        uint32_t boneIndex = it->second;
        bones[boneIndex].final_transform = globalTransform;
    }

    // Process children
    for(uint32_t i = 0; i < node->mNumChildren; i++) {
        processNodeHierarchy(node->mChildren[i], globalTransform);
    }
}

void TModel::updateBoneTransforms() {
    for(size_t i = 0; i < bones.size(); i++) {
        // For static bind pose rendering, we want the skinning to have no visual effect
        // This means the final transformation should be such that when applied to vertices
        // in bind pose, they remain in their original positions
        //
        // The skinning equation is:
        // final_pos = sum(weight_i * (global_transform_i * offset_matrix_i) * bind_pose_vertex_pos)
        //
        // For bind pose rendering, we want:
        // final_pos = bind_pose_vertex_pos
        //
        // This is achieved when: global_transform_i * offset_matrix_i = identity
        // Which means: global_transform_i = inverse(offset_matrix_i)
        //
        // So the final transformation should be the inverse of the offset matrix
        bones[i].final_transform = glm::inverse(bones[i].offset_matrix);
    }
}

void TModel::Draw(TShader& shader) {
    if (!m_isLoaded) return;
    // Update bone transforms before drawing
    updateBoneTransforms();

    // Send bone matrices to shader
    shader.Use();
    for(uint32_t i = 0; i < bones.size(); i++) {
        std::string boneName = "bones[" + std::to_string(i) + "]";
        shader.setMat4(boneName.c_str(), bones[i].final_transform);
    }

    // Draw all subject objects
    for(auto& subject : subjects) {
        for(uint32_t i = 0; i < subject.meshes.size(); i++) {
            subject.meshes[i].Draw(shader);
        }
    }
}

void TModel::DrawSubject(const std::string& subjectName, TShader& shader) {
    if (!m_isLoaded) return;
    
    // Find the subject object with the given name
    for(auto& subject : subjects) {
        if(subject.name == subjectName) {
            // Update bone transforms before drawing
            updateBoneTransforms();

            // Send bone matrices to shader
            shader.Use();
            for(uint32_t i = 0; i < bones.size(); i++) {
                std::string boneName = "bones[" + std::to_string(i) + "]";
                shader.setMat4(boneName.c_str(), bones[i].final_transform);
            }

            // Draw only the meshes for this specific subject
            for(uint32_t i = 0; i < subject.meshes.size(); i++) {
                subject.meshes[i].Draw(shader);
            }
            
            return; // Exit after drawing the requested subject
        }
    }
}

std::vector<std::string> TModel::GetSubjectNames() const {
    std::vector<std::string> names;
    for(const auto& subject : subjects) {
        names.push_back(subject.name);
    }
    return names;
}

const TSubject* TModel::GetSubject(const std::string& name) const {
    for(const auto& subject : subjects) {
        if(subject.name == name) {
            return &subject;
        }
    }
    return nullptr; // Not found
}

bool TModel::AssignTextureToCharacter(const std::string& subjectName, const STexture& texture) {
    for(auto& subject : subjects) {
        if(subject.name == subjectName) {
            subject.AddTexture(texture);
            return true;
        }
    }
    return false; // Character not found
}

bool TModel::AssignTexturesToCharacter(const std::string& subjectName, const std::vector<STexture>& textures) {
    for(auto& subject : subjects) {
        if(subject.name == subjectName) {
            for(const auto& texture : textures) {
                subject.AddTexture(texture);
            }
            return true;
        }
    }
    return false; // Character not found
}

bool TModel::ReplaceCharacterTextures(const std::string& subjectName, const std::vector<STexture>& textures) {
    for(auto& subject : subjects) {
        if(subject.name == subjectName) {
            subject.ClearTextures(); // Clear existing textures
            for(const auto& texture : textures) {
                subject.AddTexture(texture);
            }
            return true;
        }
    }
    return false; // Character not found
}

std::vector<STexture> TModel::loadMaterialTextures(aiMaterial* mat,
                                                      aiTextureType type,
                                                      const std::string& typeName) {
    std::vector<STexture> textures;

    for(uint32_t i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);

        bool skip = false;
        for(uint32_t j = 0; j < textures_loaded.size(); j++) {
            if(std::strcmp(textures_loaded[j].path.c_str(), str.C_Str()) == 0) {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        }

        if(!skip) {
            STexture texture;
            texture.id = TextureFromFile(str.C_Str());
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture);
        }
    }

    return textures;
}

uint32_t TModel::TextureFromFile(const char* path) {
    TTextureBuilder textureBuilder;
    textureBuilder.setWrapS(GL_REPEAT);
    textureBuilder.setWrapT(GL_REPEAT);
    textureBuilder.setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
    textureBuilder.setMagFilter(GL_LINEAR);
    return textureBuilder.MakeTexture(directory, path);
}

std::string TModel::getFileExtension(const std::string& path) const {
    size_t dotPos = path.find_last_of('.');
    if(dotPos != std::string::npos) {
        std::string ext = path.substr(dotPos + 1);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        return ext;
    }
    return "";
}

uint32_t TModel::getAssimpFlags(const std::string& extension) const {
    uint32_t flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals |
                         aiProcess_CalcTangentSpace;

    // Different file formats may need UV flipping
    // OBJ files typically need UVs flipped, while FBX files often don't
    if(extension == "obj" || extension == "OBJ") {
        flags |= aiProcess_FlipUVs;
    }

    if(extension == "fbx" || extension == "FBX") {
        flags |= aiProcess_PreTransformVertices;
    }

    return flags;
}

const aiAnimation* TModel::GetAnimation(uint32_t index) const {
    if(scene && index < scene->mNumAnimations) {
        return scene->mAnimations[index];
    }
    return nullptr;
}
