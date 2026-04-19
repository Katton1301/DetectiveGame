R"(#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 WorldPos;
out vec3 Normal;
out float gl_ClipDistance[1];

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 normalVec;
uniform vec4 clipPlane;

void main()
{
    WorldPos = vec3(model * vec4(aPos, 1.0));
    TexCoord = aTexCoord;
    Normal = normalVec;

    gl_Position = projection * view * vec4(WorldPos, 1.0);
    // Calculate clip distance for plane clipping
    //gl_ClipDistance[0] = dot(model * vec4(aPos, 1.0), clipPlane);
})"