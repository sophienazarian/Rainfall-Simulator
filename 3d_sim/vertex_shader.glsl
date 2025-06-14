#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Calculate fragment position in world space
    FragPos = vec3(model * vec4(position, 1.0));
    
    // Calculate normal in world space
    // Use transpose(inverse(model)) to handle non-uniform scaling correctly
    Normal = normalize(mat3(transpose(inverse(model))) * normal);
    
    // Calculate final position
    gl_Position = projection * view * model * vec4(position, 1.0);
}