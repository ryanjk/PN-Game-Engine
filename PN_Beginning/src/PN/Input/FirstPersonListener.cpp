#include "PN/Input/FirstPersonListener.h"

#include <iostream>

pn::FirstPersonListener::FirstPersonListener(ControlledBody controlledBody) 
	: m_controlledBody(controlledBody), m_scrollSpeed(0.0015f), m_moveSpeed(5.0f),
	m_moveForward(false), m_moveBack(false), m_moveRight(false), m_moveLeft(false)
{}

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

	if (m_moveForward) {
		vec3 movement = vec3(0.0f, 0.0f, -m_moveSpeed);
		m_controlledBody->translateLocal((float)dt * movement);
	}

	if (m_moveBack) {
		vec3 movement = vec3(0.0f, 0.0f, m_moveSpeed);
		m_controlledBody->translateLocal((float)dt * movement);
	}

	if (m_moveLeft) {
		vec3 movement = vec3(-m_moveSpeed, 0.0f, 0.0f);
		m_controlledBody->translateLocal((float)dt * movement);
	}

	if (m_moveRight) {
		vec3 movement = vec3(m_moveSpeed, 0.0f, 0.0f);
		m_controlledBody->translateLocal((float)dt * movement);
	}

}

void pn::FirstPersonListener::onKeyPress(int key) {
	if (key == GLFW_KEY_LEFT_SHIFT) {
		m_moveSpeed *= 2;
	}

	else if (key == GLFW_KEY_W) {
		m_moveForward = true;
	}

	else if (key == GLFW_KEY_S) {
		m_moveBack = true;
	}

	else if (key == GLFW_KEY_D) {
		m_moveRight = true;
	}

	else if (key == GLFW_KEY_A) {
		m_moveLeft = true;
	}
}

void pn::FirstPersonListener::onKeyRelease(int key) {
	if (key == GLFW_KEY_LEFT_SHIFT) {
		m_moveSpeed /= 2;
	}

	else if (key == GLFW_KEY_W) {
		m_moveForward = false;
	}

	else if (key == GLFW_KEY_S) {
		m_moveBack = false;
	}

	else if (key == GLFW_KEY_D) {
		m_moveRight = false;
	}

	else if (key == GLFW_KEY_A) {
		m_moveLeft = false;
	}
}
