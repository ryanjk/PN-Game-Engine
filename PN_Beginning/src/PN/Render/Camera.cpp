#include "PN/Render/Camera.h"

#include <iostream>

pn::Camera::Camera() {}

mat4 pn::Camera::getView() {
	double cosP = glm::cos(m_pitch);
	double sinP = glm::sin(m_pitch);
	double cosY = glm::cos(m_yaw);
	double sinY = glm::sin(m_yaw);

	m_at = vec3(cosP * sinY, -sinP, -cosY * cosP) + m_origin;
	mat4 m = glm::lookAt(m_origin, m_at, m_up);
	return m;
}

vec3 pn::Camera::getPosition() {
	return m_origin;
}

void pn::Camera::translate(vec3 amount) {
	double cosP = glm::cos(m_pitch);
	double sinP = glm::sin(m_pitch);
	double cosY = glm::cos(m_yaw);
	double sinY = glm::sin(m_yaw);

	// Before translating, rotate the direction to translate so it is parallel to the camera's rotation
	// Create the rotation matrices for pitch and yaw
	auto rotateYaw = glm::mat3(vec3(cosY, 0, -sinY), vec3(0, 1, 0), vec3(sinY, 0, cosY));
	auto rotatePitch = glm::mat3(vec3(1, 0, 0), vec3(0, cosP, sinP), vec3(0, -sinP, cosP));

	// Apply the rotation and add this vector to the current position
	amount = vec3(amount * rotatePitch * rotateYaw);

	m_origin += amount;
}

void pn::Camera::applyPitch(double radians) {
	m_pitch += radians;
	
}

void pn::Camera::applyYaw(double radians) {
	m_yaw += radians;
	
}
