#pragma once
#include "fileHelper.h"

// Storage of all options for the program
struct Options {
	std::string objName;
	float objScale;
	glm::vec3 objPos;
	Material defaultColor;
	int vertexModel;
	int wireframe;
	int print;

	// Camera options
	glm::vec3 camPos;
	glm::vec3 camLookAt;
	glm::vec3 camUp;
	float camFov;
	float camNearClip;
	float camFarClip;
	glm::vec2 camSize;
	bool isPerspective;
};

Options ReadOptions(std::string fileName);