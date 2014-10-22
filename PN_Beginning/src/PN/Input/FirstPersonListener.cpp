#include "PN/Input/FirstPersonListener.h"

pn::FirstPersonListener::FirstPersonListener(pn::Camera* camera) : m_camera(camera), m_scrollSpeed(0.002f), m_moveSpeed(0.08f) {}

void pn::FirstPersonListener::onMouseMove(double from_x, double from_y, double to_x, double to_y) {
	static const int center_x = pn::SettingsManager::g_SettingsManager.getWindowWidth() / 2;
	static const int center_y = pn::SettingsManager::g_SettingsManager.getWindowWidth() / 2;

	m_camera->applyPitch(m_scrollSpeed * (to_y - center_y));
	m_camera->applyYaw(m_scrollSpeed * (to_x - center_x));
	glfwSetCursorPos(pn::WindowManager::g_windowManager.getWindow(), center_x, center_y);
}

void pn::FirstPersonListener::update(double dt) {
	static double time = 0;
	time += dt;

	if (int p = glfwGetKey(pn::WindowManager::g_windowManager.getWindow(), 'W') == GLFW_PRESS) {
		m_camera->translate(vec3(0.0f, 0.0f, -m_moveSpeed));
	}

	if (int p = glfwGetKey(pn::WindowManager::g_windowManager.getWindow(), 'S') == GLFW_PRESS) {
		m_camera->translate(vec3(0.0f, 0.0f, m_moveSpeed));
	}

	if (int p = glfwGetKey(pn::WindowManager::g_windowManager.getWindow(), 'A') == GLFW_PRESS) {
		m_camera->translate(vec3(-m_moveSpeed, 0.0f, 0.0f));
	}

	if (int p = glfwGetKey(pn::WindowManager::g_windowManager.getWindow(), 'D') == GLFW_PRESS) {
		m_camera->translate(vec3(m_moveSpeed, 0.0f, 0.0f));
	}
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
