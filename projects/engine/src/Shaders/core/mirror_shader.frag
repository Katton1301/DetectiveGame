R"(#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 WorldPos;
in vec3 Normal;

// Material properties
uniform sampler2D reflectionTexture; // Reflection texture from FBO
uniform float metallic = 1.0;       // High metallic for mirror
uniform float roughness = 0.02;     // Very low roughness for mirror
uniform vec3 albedo = vec3(1.0);    // White base color for mirror

// Lighting
uniform vec3 viewPos;
uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];

// Function to calculate fresnel factor for realistic mirror reflections
float fresnelSchlick(float cosTheta, float F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main()
{
    // Sample the reflection texture (from our FBO)
    vec3 reflectedColor = texture(reflectionTexture, TexCoord).rgb;

    // Calculate view direction
    vec3 viewDir = normalize(viewPos - WorldPos);
    vec3 normal = normalize(Normal);

    // Calculate fresnel term for realistic mirror effect
    float cosTheta = clamp(dot(viewDir, normal), 0.0, 1.0);
    float reflectance = fresnelSchlick(cosTheta, 0.04);

    // For a perfect mirror, use high reflectance
    vec3 mirrorReflection = reflectedColor * reflectance;

    // Add slight edge darkening effect for realism
    float edgeFactor = 1.0 - pow(cosTheta, 3.0);
    mirrorReflection *= mix(1.0, 0.8, edgeFactor);

    // Ambient lighting to give the mirror some presence in the scene
    vec3 ambient = 0.05 * albedo;

    // Simple lighting calculation
    vec3 lighting = ambient;
    for(int i = 0; i < 4; i++)
    {
        vec3 lightDir = normalize(lightPositions[i] - WorldPos);
        float diff = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = diff * lightColors[i];
        lighting += diffuse * 0.1; // Reduced contribution for mirror surface
    }

    // Combine reflection with lighting
    vec3 color = mix(mirrorReflection, mirrorReflection * lighting, 0.2);

    // Gamma correction
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));

    FragColor = vec4(color, 1.0);
})"