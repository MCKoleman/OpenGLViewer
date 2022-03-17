#include "camera.h"

Camera::Camera() 
	: Camera(-45.0f, 0.1f, 100.0f) {}

Camera::Camera(float _fov, float _nearClip, float _farClip)
	: Camera(_fov, _nearClip, _farClip, glm::vec3(-2, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)) {}

Camera::Camera(float _fov, float _nearClip, float _farClip, glm::vec3 _pos, glm::vec3 _lookAt, glm::vec3 _up)
	: Camera(_fov, _nearClip, _farClip, _pos, _lookAt, _up, glm::vec2(), true) {}

Camera::Camera(float _fov, float _nearClip, float _farClip, glm::vec3 _pos, glm::vec3 _lookAt, glm::vec3 _up, glm::vec2 _orthSize, bool _isPerspective)
	: fov(_fov), nearClip(_nearClip), farClip(_farClip), pos(_pos), lookAt(_lookAt), up(_up), orthSize(_orthSize), isPerspective(_isPerspective) {}