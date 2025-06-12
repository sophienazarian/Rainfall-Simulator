#ifndef SHADER_UTILS_H
#define SHADER_UTILS_H

#include <string>
#include <GL/glew.h>

// Function to load shader source code from a file
std::string loadShaderSource(const char* filePath);

// Function to compile a shader
GLuint compileShader(GLenum type, const GLchar* source);

#endif