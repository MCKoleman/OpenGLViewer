#pragma once
#include "openGLInclude.h"

struct Camera {
	float fov;
	float nearClip;
	float farClip;
	glm::vec3 pos;
	glm::vec3 dir;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 rotation;
	glm::vec2 orthSize;
	bool isPerspective;

	void CalcBasis();

	Camera();
	Camera(float _fov, float _nearClip, float _farClip);
	Camera(float _fov, float _nearClip, float _farClip, glm::vec3 _pos, glm::vec3 _dir, glm::vec3 _up);
	Camera(float _fov, float _nearClip, float _farClip, glm::vec3 _pos, glm::vec3 _dir, glm::vec3 _up, glm::vec2 _orthSize, bool _isPerspective);
};