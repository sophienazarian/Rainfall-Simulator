#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 groundColor;
uniform vec3 dropletColor;
uniform float objectAlpha = 1.0; // Default to fully opaque if not specified

void main()
{
    // Water properties
    float ambientStrength = 0.1;
    float diffuseStrength = 0.6;
    float specularStrength = 0.8;
    float shininess = 256.0; // Higher shininess for water
    float alpha = 0.9; // Water transparency
    
    // Ambient
    vec3 ambient = ambientStrength * vec3(1.0, 1.0, 1.0);
    
    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffuseStrength * diff * vec3(1.0, 1.0, 1.0);
    
    // Specular - using Blinn-Phong for better water highlights
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * vec3(1.0, 1.0, 1.0);
    
    // Fresnel effect for water
    float fresnelBias = 0.1;
    float fresnelScale = 1.0;
    float fresnelPower = 2.0;
    float fresnelTerm = fresnelBias + fresnelScale * pow(1.0 - max(dot(norm, viewDir), 0.0), fresnelPower);
    
    vec3 result;
    float finalAlpha;
    
    // Use a more reliable way to detect if we're rendering the ground
    // Ground plane is at y = -2.0 as defined in your code
    if (abs(FragPos.y + 2.0) < 0.1) { // Ground plane with some tolerance
        result = groundColor;
        finalAlpha = 1.0; // Ground is opaque
    } else {
        // Water droplet with blue tint and refraction-like effect
        vec3 baseColor = mix(dropletColor, vec3(0.7, 0.85, 1.0), 0.5); // Mix blue with light cyan
        
        // Add depth-based coloring for droplets (darker at center)
        float distFromCenter = length(FragPos.xz); // Distance from vertical axis
        float depthFactor = smoothstep(0.0, 0.1, distFromCenter);
        baseColor = mix(baseColor * 0.7, baseColor, depthFactor);
        
        // Combine lighting components
        result = baseColor * (ambient + diffuse) + specular + fresnelTerm * vec3(0.8, 0.8, 1.0);
        finalAlpha = alpha;
    }

    // Apply the object's alpha value (for particles)
    FragColor = vec4(result, finalAlpha * objectAlpha);
}