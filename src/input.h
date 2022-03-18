#pragma once
#include "openGLInclude.h"
#include "camera.h"
#include "mesh.h"

bool processInput(GLFWwindow* window, Camera* camera, Mesh* mesh, 
	float deltaTime, float mouseMoveSpeed, float mouseTurnSpeed, float moveSpeed, float turnSpeed, float scaleRate,
	int* prevX, int* prevY);