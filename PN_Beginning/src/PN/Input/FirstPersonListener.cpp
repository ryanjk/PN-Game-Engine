#include "PN/Input/FirstPersonListener.h"

#include <iostream>

pn::FirstPersonListener::FirstPersonListener(pn::Camera* camera, PlayerBody playerBody) 
	: m_camera(camera), m_playerBody(playerBody), m_scrollSpeed(0.002f), m_moveSpeed(0.08f) {}

void pn::FirstPersonListener::onMouseMove(double from_x, double from_y, double to_x, double to_y) {
	static const int center_x = pn::SettingsManager::g_SettingsManager.getWindowWidth() / 2;
	static const int center_y = pn::SettingsManager::g_SettingsManager.getWindowWidth() / 2;

	m_camera->applyPitch(m_scrollSpeed * -(to_y - center_y));
	m_camera->applyYaw(m_scrollSpeed * -(to_x - center_x));

	m_playerBody->rotatePitch((float)m_scrollSpeed * -(float)(to_y - center_y));
	m_playerBody->rotateYaw((float)m_scrollSpeed * -(float)(to_x - center_x));

//	print_mat4(m_camera->getView() - glm::inverse(m_playerBody->getTransformMatrix()));

	glfwSetCursorPos(pn::WindowManager::g_windowManager.getWindow(), center_x, center_y);
}

void pn::FirstPersonListener::update(double dt) {
	static double time = 0;
	time += dt;

	if (glfwGetKey(pn::WindowManager::g_windowManager.getWindow(), GLFW_KEY_W) == GLFW_PRESS) {
		vec3 movement = vec3(0.0f, 0.0f, -m_moveSpeed);
		m_playerBody->translateLocal(movement);
		m_camera->translate(movement);
	}

	if (glfwGetKey(pn::WindowManager::g_windowManager.getWindow(), GLFW_KEY_S) == GLFW_PRESS) {
		vec3 movement = vec3(0.0f, 0.0f, m_moveSpeed);
		m_playerBody->translateLocal(movement);
		m_camera->translate(movement);
	}

	if (glfwGetKey(pn::WindowManager::g_windowManager.getWindow(), GLFW_KEY_A) == GLFW_PRESS) {
		vec3 movement = vec3(-m_moveSpeed, 0.0f, 0.0f);
		m_playerBody->translateLocal(movement);
		m_camera->translate(movement);
	}

	if (glfwGetKey(pn::WindowManager::g_windowManager.getWindow(), GLFW_KEY_D) == GLFW_PRESS) {
		vec3 movement = vec3(m_moveSpeed, 0.0f, 0.0f);
		m_playerBody->translateLocal(movement);
		m_camera->translate(movement);
	}

/*	std::cout << "[" <<
		m_camera->getPosition()[0] - m_playerBody->getTranslation()[0] << ", " <<
		m_camera->getPosition()[1] - m_playerBody->getTranslation()[1] << ", " <<
		m_camera->getPosition()[2] - m_playerBody->getTranslation()[2] << "]" << std::endl; */
}

void pn::FirstPersonListener::onKeyPress(int key) {
	if (key == GLFW_KEY_LEFT_SHIFT) {
		m_moveSpeed *= 2;
	}
}

void pn::FirstPersonListener::onKeyRelease(int key) {
	if (key == GLFW_KEY_LEFT_SHIFT) {
		m_moveSpeed /= 2;
	}
}
