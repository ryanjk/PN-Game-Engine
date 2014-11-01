#include "PN/Input/FirstPersonListener.h"

#include <iostream>

pn::FirstPersonListener::FirstPersonListener(ControlledBody controlledBody) 
	: m_controlledBody(controlledBody), m_scrollSpeed(0.002f), m_moveSpeed(0.08f) {}

void pn::FirstPersonListener::onMouseMove(double from_x, double from_y, double to_x, double to_y) {
	static const int center_x = pn::SettingsManager::g_SettingsManager.getWindowWidth() / 2;
	static const int center_y = pn::SettingsManager::g_SettingsManager.getWindowWidth() / 2;

	m_controlledBody->rotatePitchLocal((float)m_scrollSpeed * -(float)(to_y - center_y));
	m_controlledBody->rotateYawLocal((float)m_scrollSpeed * -(float)(to_x - center_x));

	glfwSetCursorPos(pn::WindowManager::g_windowManager.getWindow(), center_x, center_y);
}

void pn::FirstPersonListener::update(double dt) {
	static double time = 0;
	time += dt;

	if (glfwGetKey(pn::WindowManager::g_windowManager.getWindow(), GLFW_KEY_W) == GLFW_PRESS) {
		vec3 movement = vec3(0.0f, 0.0f, -m_moveSpeed);
		m_controlledBody->translateLocal(movement);
	}

	if (glfwGetKey(pn::WindowManager::g_windowManager.getWindow(), GLFW_KEY_S) == GLFW_PRESS) {
		vec3 movement = vec3(0.0f, 0.0f, m_moveSpeed);
		m_controlledBody->translateLocal(movement);
	}

	if (glfwGetKey(pn::WindowManager::g_windowManager.getWindow(), GLFW_KEY_A) == GLFW_PRESS) {
		vec3 movement = vec3(-m_moveSpeed, 0.0f, 0.0f);
		m_controlledBody->translateLocal(movement);
	}

	if (glfwGetKey(pn::WindowManager::g_windowManager.getWindow(), GLFW_KEY_D) == GLFW_PRESS) {
		vec3 movement = vec3(m_moveSpeed, 0.0f, 0.0f);
		m_controlledBody->translateLocal(movement);
	}

}

void pn::FirstPersonListener::onKeyPress(int key) {
	if (key == GLFW_KEY_LEFT_SHIFT) {
		m_moveSpeed *= 2;
	}

	if (key == GLFW_KEY_Y) {
		m_controlledBody->rotateYawLocal((float)m_scrollSpeed * 20);
//		std::cout << "yaw: " << to_x - center_x << std::endl;
	}
}

void pn::FirstPersonListener::onKeyRelease(int key) {
	if (key == GLFW_KEY_LEFT_SHIFT) {
		m_moveSpeed /= 2;
	}
}
