R"(#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

// Material textures
uniform sampler2D material_texture_diffuse1;
uniform sampler2D material_texture_specular1;
uniform sampler2D material_texture_normal1;
uniform sampler2D material_texture_height1;

// Lighting
uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];
uniform vec3 viewPos;

// Material properties
uniform vec3 material_ambient;
uniform vec3 material_diffuse;
uniform vec3 material_specular;
uniform float material_shininess;

void main()
{
    // Sample textures
    vec3 diffuseTexture = texture(material_texture_diffuse1, TexCoords).rgb;
    vec3 specularTexture = texture(material_texture_specular1, TexCoords).rgb;

    // Normalize normal
    vec3 norm = normalize(Normal);

    // View direction
    vec3 viewDir = normalize(viewPos - WorldPos);

    // Ambient lighting
    vec3 ambient = material_ambient * diffuseTexture;

    // Diffuse and Specular lighting
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    // Process each light
    for(int i = 0; i < 4; i++)
    {
        // Light direction
        vec3 lightDir = normalize(lightPositions[i] - WorldPos);

        // Distance and attenuation
        float distance = length(lightPositions[i] - WorldPos);
        float attenuation = 1.0 / (distance * distance);

        // Diffuse shading
        float diff = max(dot(norm, lightDir), 0.0);
        diffuse += (diff * lightColors[i] * diffuseTexture) * attenuation;

        // Specular shading
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material_shininess);
        specular += (spec * lightColors[i] * specularTexture) * attenuation;
    }

    // Combine results
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
})"