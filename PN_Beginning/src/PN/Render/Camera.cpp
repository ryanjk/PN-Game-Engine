#include "PN/Render/Camera.h"

#include <iostream>

pn::Camera::Camera() {}

void pn::Camera::setCamera(CameraComponentPointer cameraComponent) {
	m_currentCamera = cameraComponent;
}

mat4 pn::Camera::getView() {
	double pitch = m_currentCamera->getPitch();
	double yaw = m_currentCamera->getYaw();
	
	double cosP = glm::cos(pitch);
	double sinP = glm::sin(pitch);
	double cosY = glm::cos(yaw);
	double sinY = glm::sin(yaw);

	m_currentCamera->setAt(vec3(-cosP * sinY, sinP, -cosY * cosP) + m_currentCamera->getOrigin());

	mat4 m = glm::lookAt(
		m_currentCamera->getOrigin(), 
		m_currentCamera->getAt(), 
		m_currentCamera->getUp()
		);
	return m;

	
}

vec3 pn::Camera::getPosition() {
	return m_currentCamera->getOrigin();
}

void pn::Camera::translate(vec3 amount) {
	double pitch = m_currentCamera->getPitch();
	double yaw = m_currentCamera->getYaw();

	auto p = make_quat({1.0f, 0.0f, 0.0f}, (float) pitch);
	auto y = make_quat({ 0.0f, 1.0f, 0.0f }, (float) yaw);

	amount = p * y * amount;

	m_currentCamera->setOrigin(m_currentCamera->getOrigin() + amount);
}

void pn::Camera::applyPitch(double radians) {
	m_currentCamera->setPitch(m_currentCamera->getPitch() + radians);
	
}

void pn::Camera::applyYaw(double radians) {
	m_currentCamera->setYaw(m_currentCamera->getYaw() + radians);
}
