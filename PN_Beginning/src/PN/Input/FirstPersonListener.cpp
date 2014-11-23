#include "PN/Input/FirstPersonListener.h"

#include <iostream>

pn::FirstPersonListener::FirstPersonListener(ControlledBody controlledBody, PhysicsSystem* physicsSystem) 
	: m_body_moveComponent(std::dynamic_pointer_cast<pn::MoveComponent>(controlledBody->getComponent(pn::ComponentType::MOVE))), 
	m_body_transformComponent(std::dynamic_pointer_cast<pn::TransformComponent>(controlledBody->getComponent(pn::ComponentType::TRANSFORM))),
	m_physicsSystem(physicsSystem),
	m_scrollSpeed(0.0015f), m_moveSpeed(5.0f),
	m_back_key(pn::mm::KeyEnum::KEY_S), m_forward_key(pn::mm::KeyEnum::KEY_W),
	m_left_key(pn::mm::KeyEnum::KEY_A), m_right_key(pn::mm::KeyEnum::KEY_D),
	move_forward(false), move_back(false), move_left(false), move_right(false)
{
	assert(controlledBody->hasComponents(pn::ComponentType::MOVE | pn::ComponentType::TRANSFORM));
}

void pn::FirstPersonListener::onMouseMove(double from_x, double from_y, double to_x, double to_y) {
	static const int center_x = pn::SettingsManager::g_SettingsManager.getWindowWidth() / 2;
	static const int center_y = pn::SettingsManager::g_SettingsManager.getWindowWidth() / 2;

	m_physicsSystem->rotatePitchLocal(*m_body_transformComponent, (float)m_scrollSpeed * -(float)(to_y - center_y));
	m_physicsSystem->rotateYawLocal(*m_body_transformComponent, (float)m_scrollSpeed * -(float)(to_x - center_x));

	pn::mm::moveCursorToPos(pn::WindowManager::g_windowManager.getWindow(), center_x, center_y);
}

void pn::FirstPersonListener::update(double dt) {
	static double time = 0;
	time += dt;
}

void pn::FirstPersonListener::onKeyPress(int key) {
	if (key == mm::KEY_LEFT_SHIFT) {
		m_moveSpeed *= 2;
		m_body_moveComponent->setVelocity(m_body_moveComponent->getVelocity() * 2.0f);
	}

	else if (key == m_forward_key) {
		move_forward = true;

		auto current_dir = safe_normalize(m_body_moveComponent->getVelocity());
		if (move_back) {
			m_body_moveComponent->setVelocity(vec3(1.0f, 0.0f, 0.0f) * current_dir * m_moveSpeed);
		}
		else {
			auto new_dir = m_moveSpeed * safe_normalize(current_dir + vec3(0.0f, 0.0f, -1.0f));
			m_body_moveComponent->setVelocity(new_dir);
		}
	}

	else if (key == m_back_key) {
		move_back = true;
		
		auto current_dir = safe_normalize(m_body_moveComponent->getVelocity());
		if (move_forward) {
			m_body_moveComponent->setVelocity(vec3(1.0f, 0.0f, 0.0f) * current_dir * m_moveSpeed);
		}
		else {
			auto new_dir = m_moveSpeed * safe_normalize(current_dir + vec3(0.0f, 0.0f, 1.0f));
			m_body_moveComponent->setVelocity(new_dir);
		}
	}

	else if (key == m_right_key) {
		move_right = true;
		
		auto current_dir = safe_normalize(m_body_moveComponent->getVelocity());
		if (move_left) {
			m_body_moveComponent->setVelocity(vec3(0.0f, 0.0f, 1.0f) * current_dir * m_moveSpeed);
		}
		else {
			auto new_dir = m_moveSpeed * safe_normalize(current_dir + vec3(1.0f, 0.0f, 0.0f));
			m_body_moveComponent->setVelocity(new_dir);
		}
	}

	else if (key == m_left_key) {
		move_left = true;
		
		auto current_dir = safe_normalize(m_body_moveComponent->getVelocity());
		if (move_right) {
			m_body_moveComponent->setVelocity(vec3(0.0f, 0.0f, 1.0f) * current_dir * m_moveSpeed);
		}
		else {
			auto new_dir = m_moveSpeed * safe_normalize(current_dir + vec3(-1.0f, 0.0f, 0.0f));
			m_body_moveComponent->setVelocity(new_dir);
		}
	}
}

void pn::FirstPersonListener::onKeyRelease(int key) {
	if (key == mm::KEY_LEFT_SHIFT) {
		m_moveSpeed *= 0.5f;
		m_body_moveComponent->setVelocity(m_body_moveComponent->getVelocity() * 0.5f);
	}

	else if (key == m_forward_key) {
		move_forward = false;
		
		auto current_dir = safe_normalize(m_body_moveComponent->getVelocity());
		if (move_back) {
			auto new_dir = m_moveSpeed * safe_normalize(current_dir + vec3(0.0f, 0.0f, 1.0f));
			m_body_moveComponent->setVelocity(new_dir);
		}
		else {
			m_body_moveComponent->setVelocity(vec3(1.0f, 0.0f, 0.0f) * current_dir * m_moveSpeed);
		}
	}

	else if (key == m_back_key) {
		move_back = false;
		
		auto current_dir = safe_normalize(m_body_moveComponent->getVelocity());
		if (move_forward) {
			auto new_dir = m_moveSpeed * safe_normalize(current_dir + vec3(0.0f, 0.0f, -1.0f));
			m_body_moveComponent->setVelocity(new_dir);
		}
		else {
			m_body_moveComponent->setVelocity(vec3(1.0f, 0.0f, 0.0f) * current_dir * m_moveSpeed);
		}
	}

	else if (key == m_right_key) {
		move_right = false;
		
		auto current_dir = safe_normalize(m_body_moveComponent->getVelocity());
		if (move_left) {
			auto new_dir = m_moveSpeed * safe_normalize(current_dir + vec3(-1.0f, 0.0f, 0.0f));
			m_body_moveComponent->setVelocity(new_dir);
		}
		else {
			m_body_moveComponent->setVelocity(vec3(0.0f, 0.0f, 1.0f) * current_dir * m_moveSpeed);
		}
	}

	else if (key == m_left_key) {
		move_left = false;
		
		auto current_dir = safe_normalize(m_body_moveComponent->getVelocity());
		if (move_right) {
			auto new_dir = m_moveSpeed * safe_normalize(current_dir + vec3(1.0f, 0.0f, 0.0f));
			m_body_moveComponent->setVelocity(new_dir);
		}
		else {
			m_body_moveComponent->setVelocity(vec3(0.0f, 0.0f, 1.0f) * current_dir * m_moveSpeed);
		}
	}
}
