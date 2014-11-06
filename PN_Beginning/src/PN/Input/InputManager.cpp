#include "PN/Input/InputManager.h"

#include <iostream>

pn::InputManager pn::InputManager::g_inputManager;

void pn::InputManager::mouse_move_callback(double x, double y) {
	static double old_x = 0;
	static double old_y = 0;

	g_inputManager.m_handler->handleMouseMove(old_x, old_y, x, y);

	old_x = x;
	old_y = y;
}

void pn::InputManager::cursor_enter_callback(int enter) {
	if (enter == GL_TRUE) {
		g_inputManager.m_handler->handleCursorEnter();
	}

	else {
		g_inputManager.m_handler->handleCursorExit();
	}
}

void pn::InputManager::mouse_button_callback(int button, int action) {
	int x;
	int y;
		
	mm::getCursorPos(&x, &y);
	
	switch (button) {
	case mm::MOUSE_LEFT: 
		if (action == mm::MOUSE_PRESS) {
			g_inputManager.m_handler->handleLeftMousePress(x, y);
		}
		else if (action == mm::MOUSE_RELEASE) {
			g_inputManager.m_handler->handleLeftMouseRelease(x, y);
		}
		break;
	case mm::MOUSE_RIGHT:
		if (action == mm::MOUSE_PRESS) {
			g_inputManager.m_handler->handleRightMousePress(x, y);
		}
		else if (action == mm::MOUSE_RELEASE) {
			g_inputManager.m_handler->handleRightMouseRelease(x, y);
		}
		break;
	case mm::MOUSE_MIDDLE:
		if (action == mm::MOUSE_PRESS) {
			g_inputManager.m_handler->handleMiddleMousePress(x, y);
		}
		else if (action == mm::MOUSE_RELEASE) {
			g_inputManager.m_handler->handleMiddleMouseRelease(x, y);
		}
		break;
	default:
		break;
	}
}

void pn::InputManager::keyboard_key_callback(int key, int action) {
	if (action == mm::KEY_PRESS) {
		g_inputManager.m_handler->handleKeyPress(key);
	}
	else if (action == mm::KEY_RELEASE) {
		g_inputManager.m_handler->handleKeyRelease(key);
	}
}

pn::InputManager::InputManager() {

}

void pn::InputManager::shutdown() {

}

void pn::InputManager::startUp(HandlerPointer handler) {
//	glfwSetCharCallback
//	glfwSetScrollCallback

	setInputEventHandler(handler);
}

void pn::InputManager::sendEvent(pn::mm::InputEvent inputEvent) {
	switch (inputEvent.type) {
	case mm::WINDOW_EVENT:
		switch (inputEvent.window.event) {
		case mm::MOUSE_ENTER_WINDOW_EVENT:
			cursor_enter_callback(GL_TRUE);
			break;
		case mm::MOUSE_EXIT_WINDOW_EVENT:
			cursor_enter_callback(GL_FALSE);
			break;
		default:
			break;
		}
		break;
	case mm::MOUSE_MOVE_EVENT:
		mouse_move_callback(inputEvent.motion.x, inputEvent.motion.y);
		break;
	case mm::MOUSE_BUTTON_DOWN_EVENT:
	case mm::MOUSE_BUTTON_UP_EVENT:
		mouse_button_callback(inputEvent.button.button, inputEvent.button.state);
		break;
	case mm::KEY_DOWN_EVENT:
	case mm::KEY_UP_EVENT:
		if (inputEvent.key.repeat == 0) {
			keyboard_key_callback(inputEvent.key.keysym.sym, inputEvent.key.state);
		}
		break;
	default:
		break;
	}
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