#pragma once
#include "fileHelper.h"
#include "objReader.h"
#include "input.h"
#include "options.h"
#include "light.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
glm::mat4 CalcMVP(Camera* camera, Mesh* mesh);
glm::mat4 GetProjectionMatrix(Camera* camera);
glm::mat4 GetViewMatrix(Camera* camera);
glm::mat4 GetModelMatrix(Mesh* mesh);