#pragma once
#include "fileHelper.h"
#include "objReader.h"
#include "input.h"
#include "options.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
glm::mat4 CalcMVP(Camera camera, Mesh* mesh);