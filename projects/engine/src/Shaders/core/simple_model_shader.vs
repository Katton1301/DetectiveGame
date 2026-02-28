R"(#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in ivec4 aBoneIDs;
layout (location = 4) in vec4 aWeights;

out vec2 TexCoords;
out vec3 WorldPos;
out vec3 Normal;

#define MAX_BONES 200
#define MAX_BONE_INFLUENCE 4

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;
uniform mat4 bones[MAX_BONES];

void main()
{
    vec4 localPos = vec4(aPos, 1.0);
    vec4 localNormal = vec4(aNormal, 0.0);

    // Check if this vertex is influenced by bones
    bool hasBones = (aBoneIDs[0] >= 0);

    if(hasBones) {
        mat4 boneTransform = mat4(0.0);

        // Accumulate bone transformations based on weights
        for(int i = 0; i < MAX_BONE_INFLUENCE; i++) {
            if(aBoneIDs[i] != -1 && aWeights[i] > 0) {
                int boneIndex = aBoneIDs[i];
                if(boneIndex < MAX_BONES) {
                    boneTransform += aWeights[i] * bones[boneIndex];
                }
            }
        }

        // Apply bone transformation
        localPos = boneTransform * localPos;
        localNormal = boneTransform * localNormal;
    }

    // Transform to world space
    vec4 worldPos = model * localPos;

    // Pass to fragment shader
    WorldPos = worldPos.xyz;
    Normal = normalMatrix * vec3(localNormal);
    TexCoords = aTexCoords;

    gl_Position = projection * view * worldPos;
})"