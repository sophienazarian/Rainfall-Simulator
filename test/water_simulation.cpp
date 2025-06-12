#include <GL/glew.h>
 #include <GLFW/glfw3.h>
 #include <glm/glm.hpp>
 #include <glm/gtc/matrix_transform.hpp>
 #include <glm/gtc/type_ptr.hpp>
 #include <iostream>
 #include <cmath>
 #include <vector>
 
 // Window dimensions
 const GLuint WIDTH = 800, HEIGHT = 600;
 
 // Vertex shader
 const GLchar* vertexShaderSource = R"(
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
         FragPos = vec3(model * vec4(position, 1.0));
         Normal = mat3(transpose(inverse(model))) * normal;
         gl_Position = projection * view * model * vec4(position, 1.0);
     }
 )";
 
 // Fragment shader
 const GLchar* fragmentShaderSource = R"(
     #version 330 core
     out vec4 FragColor;
     
     in vec3 FragPos;
     in vec3 Normal;
     
     uniform vec3 lightPos;
     uniform vec3 viewPos;
     
     void main()
     {
         // Water properties
         vec3 waterColor = vec3(0.2, 0.4, 0.8);
         float ambientStrength = 0.2;
         float specularStrength = 0.8;
         float shininess = 64.0;
         
         // Ambient
         vec3 ambient = ambientStrength * vec3(1.0, 1.0, 1.0);
         
         // Diffuse
         vec3 norm = normalize(Normal);
         vec3 lightDir = normalize(lightPos - FragPos);
         float diff = max(dot(norm, lightDir), 0.0);
         vec3 diffuse = diff * vec3(1.0, 1.0, 1.0);
         
         // Specular
         vec3 viewDir = normalize(viewPos - FragPos);
         vec3 reflectDir = reflect(-lightDir, norm);
         float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
         vec3 specular = specularStrength * spec * vec3(1.0, 1.0, 1.0);
         
         // Fresnel effect for water (simplified)
         float fresnelFactor = pow(1.0 - max(dot(norm, viewDir), 0.0), 4.0);
         vec3 fresnel = fresnelFactor * vec3(1.0, 1.0, 1.0);
         
         vec3 result = (ambient + diffuse) * waterColor + specular + fresnel * 0.5;
         FragColor = vec4(result, 0.85); // Slight transparency
     }
 )";
 
 // Create a sphere mesh
 void createSphere(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices, float radius, int sectors, int stacks) {
     float PI = 3.14159265359f;
     float sectorStep = 2 * PI / sectors;
     float stackStep = PI / stacks;
     float sectorAngle, stackAngle;
 
     // Generate vertices
     for (int i = 0; i <= stacks; ++i) {
         stackAngle = PI / 2 - i * stackStep;
         float xy = radius * cosf(stackAngle);
         float z = radius * sinf(stackAngle);
 
         for (int j = 0; j <= sectors; ++j) {
             sectorAngle = j * sectorStep;
 
             // Vertex position
             float x = xy * cosf(sectorAngle);
             float y = xy * sinf(sectorAngle);
             vertices.push_back(x);
             vertices.push_back(y);
             vertices.push_back(z);
 
             // Normal vector
             float nx = x / radius;
             float ny = y / radius;
             float nz = z / radius;
             vertices.push_back(nx);
             vertices.push_back(ny);
             vertices.push_back(nz);
         }
     }
 
     // Generate indices
     for (int i = 0; i < stacks; ++i) {
         int k1 = i * (sectors + 1);
         int k2 = k1 + sectors + 1;
 
         for (int j = 0; j < sectors; ++j, ++k1, ++k2) {
             if (i != 0) {
                 indices.push_back(k1);
                 indices.push_back(k2);
                 indices.push_back(k1 + 1);
             }
 
             if (i != (stacks - 1)) {
                 indices.push_back(k1 + 1);
                 indices.push_back(k2);
                 indices.push_back(k2 + 1);
             }
         }
     }
 }
 
 // Function to compile shaders
 GLuint compileShader(GLenum type, const GLchar* source) {
     GLuint shader = glCreateShader(type);
     glShaderSource(shader, 1, &source, NULL);
     glCompileShader(shader);
     
     GLint success;
     GLchar infoLog[512];
     glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
     if (!success) {
         glGetShaderInfoLog(shader, 512, NULL, infoLog);
         std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
         return 0;
     }
     return shader;
 }
 
 // Water droplet properties
 struct Droplet {
     glm::vec3 position;
     glm::vec3 velocity;
     float size;
     
     Droplet(glm::vec3 pos, glm::vec3 vel, float sz) : position(pos), velocity(vel), size(sz) {}
     
     void update(float deltaTime) {
         // Apply gravity
         velocity.y -= 9.8f * deltaTime;
         
         // Update position
         position += velocity * deltaTime;
         
         // Simple ground collision
         if (position.y - size < -2.0f) {
             position.y = -2.0f + size;
             velocity.y = -velocity.y * 0.3f; // Dampen bounce
             
             // Stop bouncing when energy is low
             if (std::abs(velocity.y) < 0.5f) {
                 velocity.y = 0.0f;
             }
         }
     }
 };
 
 int main() {
     // Initialize GLFW
     if (!glfwInit()) {
         std::cerr << "Failed to initialize GLFW" << std::endl;
         return -1;
     }
     
     // Set OpenGL version
     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
     glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
     
     // Create window
     GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Water Droplet Simulation", NULL, NULL);
     if (!window) {
         std::cerr << "Failed to create GLFW window" << std::endl;
         glfwTerminate();
         return -1;
     }
     glfwMakeContextCurrent(window);
     
     // Initialize GLEW
     glewExperimental = GL_TRUE;
     if (glewInit() != GLEW_OK) {
         std::cerr << "Failed to initialize GLEW" << std::endl;
         return -1;
     }
     
     // Enable depth testing and alpha blending
     glEnable(GL_DEPTH_TEST);
     glEnable(GL_BLEND);
     glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
     
     // Create and compile shaders
     GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
     GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
     
     // Create shader program
     GLuint shaderProgram = glCreateProgram();
     glAttachShader(shaderProgram, vertexShader);
     glAttachShader(shaderProgram, fragmentShader);
     glLinkProgram(shaderProgram);
     
     // Check for linking errors
     GLint success;
     GLchar infoLog[512];
     glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
     if (!success) {
         glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
         std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
         return -1;
     }
     
     glDeleteShader(vertexShader);
     glDeleteShader(fragmentShader);
     
     // Create sphere mesh for water droplet
     std::vector<GLfloat> sphereVertices;
     std::vector<GLuint> sphereIndices;
     createSphere(sphereVertices, sphereIndices, 0.1f, 32, 16);
     
     // Create VAO, VBO, EBO
     GLuint VAO, VBO, EBO;
     glGenVertexArrays(1, &VAO);
     glGenBuffers(1, &VBO);
     glGenBuffers(1, &EBO);
     
     glBindVertexArray(VAO);
     
     glBindBuffer(GL_ARRAY_BUFFER, VBO);
     glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(GLfloat), sphereVertices.data(), GL_STATIC_DRAW);
     
     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
     glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(GLuint), sphereIndices.data(), GL_STATIC_DRAW);
     
     // Position attribute
     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
     glEnableVertexAttribArray(0);
     
     // Normal attribute
     glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
     glEnableVertexAttribArray(1);
     
     glBindVertexArray(0);
     
     // Create a ground plane VAO
     GLfloat groundVertices[] = {
         // Positions          // Normals
         -5.0f, -2.0f, -5.0f,  0.0f, 1.0f, 0.0f,
          5.0f, -2.0f, -5.0f,  0.0f, 1.0f, 0.0f,
          5.0f, -2.0f,  5.0f,  0.0f, 1.0f, 0.0f,
         -5.0f, -2.0f,  5.0f,  0.0f, 1.0f, 0.0f
     };
     
     GLuint groundIndices[] = {
         0, 1, 2,
         0, 2, 3
     };
     
     GLuint groundVAO, groundVBO, groundEBO;
     glGenVertexArrays(1, &groundVAO);
     glGenBuffers(1, &groundVBO);
     glGenBuffers(1, &groundEBO);
     
     glBindVertexArray(groundVAO);
     
     glBindBuffer(GL_ARRAY_BUFFER, groundVBO);
     glBufferData(GL_ARRAY_BUFFER, sizeof(groundVertices), groundVertices, GL_STATIC_DRAW);
     
     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, groundEBO);
     glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(groundIndices), groundIndices, GL_STATIC_DRAW);
     
     // Position attribute
     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
     glEnableVertexAttribArray(0);
     
     // Normal attribute
     glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
     glEnableVertexAttribArray(1);
     
     glBindVertexArray(0);
     
     // Create a droplet
     Droplet droplet(glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 0.1f);
     
     // Camera position
     glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
     
     // Light position
     glm::vec3 lightPos = glm::vec3(2.0f, 3.0f, 2.0f);
     
     // Time tracking for animation
     float lastFrame = 0.0f;
     
     // Main loop
     while (!glfwWindowShouldClose(window)) {
         // Calculate delta time
         float currentFrame = glfwGetTime();
         float deltaTime = currentFrame - lastFrame;
         lastFrame = currentFrame;
         
         // Process input
         if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
             glfwSetWindowShouldClose(window, true);
         
         // Update droplet physics
         droplet.update(deltaTime);
         
         // Clear the screen
         glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
         
         // Activate shader
         glUseProgram(shaderProgram);
         
         // Create transformations
         glm::mat4 view = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
         glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
         
         // Set uniforms
         glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, glm::value_ptr(lightPos));
         glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(cameraPos));
         glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
         glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
         
         // Draw the ground
         glBindVertexArray(groundVAO);
         glm::mat4 groundModel = glm::mat4(1.0f);
         glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(groundModel));
         glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
         
         // Draw the water droplet
         glBindVertexArray(VAO);
         glm::mat4 model = glm::mat4(1.0f);
         model = glm::translate(model, droplet.position);
         model = glm::scale(model, glm::vec3(droplet.size));
         glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
         glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0);
         
         // Swap buffers and poll events
         glfwSwapBuffers(window);
         glfwPollEvents();
     }
     
     // Clean up
     glDeleteVertexArrays(1, &VAO);
     glDeleteVertexArrays(1, &groundVAO);
     glDeleteBuffers(1, &VBO);
     glDeleteBuffers(1, &EBO);
     glDeleteBuffers(1, &groundVBO);
     glDeleteBuffers(1, &groundEBO);
     glDeleteProgram(shaderProgram);
     
     // Terminate GLFW
     glfwTerminate();
     
     return 0;
 }
