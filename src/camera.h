#pragma once
#include "openGLInclude.h"

struct Camera {
	float fov;
	float nearClip;
	float farClip;
	glm::vec3 pos;
	glm::vec3 lookAt;
	glm::vec3 up;
	glm::vec2 orthSize;
	bool isPerspective;

	Camera();
	Camera(float _fov, float _nearClip, float _farClip);
	Camera(float _fov, float _nearClip, float _farClip, glm::vec3 _pos, glm::vec3 _lookAt, glm::vec3 _up);
	Camera(float _fov, float _nearClip, float _farClip, glm::vec3 _pos, glm::vec3 _lookAt, glm::vec3 _up, glm::vec2 _orthSize, bool _isPerspective);
};