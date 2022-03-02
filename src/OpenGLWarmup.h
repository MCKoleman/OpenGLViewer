#pragma once
#include "openGLInclude.h"
#include <iostream>
#include <fstream>
#include <string>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
std::string ReadShader(std::string location);