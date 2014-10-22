#include "PN/Input/InputEventHandler.h"

#include <iostream>

pn::InputEventHandler::InputEventHandler() {

}

void pn::InputEventHandler::addListener(ListenerPointer listener) {
	m_listeners.push_back(listener);
}

void pn::InputEventHandler::removeListener(ListenerPointer listener) {
	auto it = std::find(m_listeners.begin(), m_listeners.end(), listener);
	if (it != m_listeners.end()) {
		m_listeners.erase(it);
	}
}

void pn::InputEventHandler::update(double dt) {
	for (auto i : m_listeners) {
		i->update(dt);
	}
}

void pn::InputEventHandler::handleLeftMousePress(double x, double y) {
	for (auto i : m_listeners) {
		i->onLeftMousePress(x, y);
	}
}

void pn::InputEventHandler::handleLeftMouseRelease(double x, double y) {
	for (auto i : m_listeners) {
		i->onLeftMouseRelease(x, y);
	}
}

void pn::InputEventHandler::handleRightMousePress(double x, double y) {
	for (auto i : m_listeners) {
		i->onRightMousePress(x, y);
	}
}

void pn::InputEventHandler::handleRightMouseRelease(double x, double y) {
	for (auto i : m_listeners) {
		i->onRightMouseRelease(x, y);
	}
}

void pn::InputEventHandler::handleMiddleMousePress(double x, double y) {
	for (auto i : m_listeners) {
		i->onMiddleMousePress(x, y);
	}
}

void pn::InputEventHandler::handleMiddleMouseRelease(double x, double y) {
	for (auto i : m_listeners) {
		i->onMiddleMouseRelease(x, y);
	}
}

void pn::InputEventHandler::handleMouseMove(double from_x, double from_y, double to_x, double to_y) {
	for (auto i : m_listeners) {
		i->onMouseMove(from_x, from_y, to_x, to_y);
	}
}

void pn::InputEventHandler::handleKeyPress(Key key) {
	for (auto i : m_listeners) {
		i->onKeyPress(key);
	}
}

void pn::InputEventHandler::handleKeyRelease(Key key) {
	for (auto i : m_listeners) {
		i->onKeyRelease(key);
	}
}

void pn::InputEventHandler::handleCursorEnter() {
	for (auto i : m_listeners) {
		i->onCursorEnter();
	}
}

void pn::InputEventHandler::handleCursorExit() {
	for (auto i : m_listeners) {
		i->onCursorExit();
	}
}