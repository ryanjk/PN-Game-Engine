#include "PN/Input/InputManager.h"

#include <iostream>

pn::InputManager pn::InputManager::g_inputManager;

void pn::InputManager::mouse_move_callback(Window* window, double x, double y) {
	static double old_x = 0;
	static double old_y = 0;

	g_inputManager.m_handler->handleMouseMove(old_x, old_y, x, y);

	old_x = x;
	old_y = y;
}

void pn::InputManager::curson_enter_callback(Window* window, int enter) {
	if (enter == GL_TRUE) {
		g_inputManager.m_handler->handleCursorEnter();
	}

	else {
		g_inputManager.m_handler->handleCursorExit();
	}
}

void pn::InputManager::mouse_button_callback(Window* window, int button, int action, int mods) {
	double x;
	double y;
		
	glfwGetCursorPos(window, &x, &y);
	
	switch (button) {
	case GLFW_MOUSE_BUTTON_LEFT: 
		if (action == GLFW_PRESS) {
			g_inputManager.m_handler->handleLeftMousePress(x, y);
		}
		else if (action == GLFW_RELEASE) {
			g_inputManager.m_handler->handleLeftMouseRelease(x, y);
		}
		break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		if (action == GLFW_PRESS) {
			g_inputManager.m_handler->handleRightMousePress(x, y);
		}
		else if (action == GLFW_RELEASE) {
			g_inputManager.m_handler->handleRightMouseRelease(x, y);
		}
		break;
	case GLFW_MOUSE_BUTTON_MIDDLE:
		if (action == GLFW_PRESS) {
			g_inputManager.m_handler->handleMiddleMousePress(x, y);
		}
		else if (action == GLFW_RELEASE) {
			g_inputManager.m_handler->handleMiddleMouseRelease(x, y);
		}
		break;
	default:
		break;
	}
}

void pn::InputManager::keyboard_key_callback(Window* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		g_inputManager.m_handler->handleKeyPress(key);
	}
	else if (action == GLFW_RELEASE) {
		g_inputManager.m_handler->handleKeyRelease(key);
	}
}

pn::InputManager::InputManager() {

}

void pn::InputManager::shutdown() {

}

void pn::InputManager::startUp(Window* window, HandlerPointer handler) {
//	glfwSetCharCallback
	glfwSetCursorEnterCallback(window, curson_enter_callback);
	glfwSetCursorPosCallback(window, mouse_move_callback);
	glfwSetKeyCallback(window, keyboard_key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
//	glfwSetScrollCallback

	setInputEventHandler(handler);
}

void pn::InputManager::update(double dt) {
	g_inputManager.m_handler->update(dt);
}

void pn::InputManager::setInputEventHandler(HandlerPointer handler) {
	g_inputManager.m_handler = handler;
}

const HandlerPointer pn::InputManager::getInputHandler() {
	return m_handler;
}