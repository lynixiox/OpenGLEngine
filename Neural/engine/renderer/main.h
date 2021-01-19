#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

typedef void(*GL_GENBUFFERS) (GLsizei, GLuint*);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


//GL_GENBUFFERS glGenbuffers = (GL_GENBUFFERS)wglGetProcAddress("glGenBuffers");