#ifndef FIRST_PERSON_LISTENER_H
#define FIRST_PERSON_LISTENER_H

#include "PN/ECS/Component/TransformComponent.h"

#include "PN/Input/InputEventListener.h"

#include "PN/Window/WindowManager.h"

#include "PN/Settings/SettingsManager.h"

#include <memory>

using ControlledBody = std::shared_ptr < pn::TransformComponent > ;

namespace pn {
	class FirstPersonListener : public pn::InputEventListener {
	public:
		FirstPersonListener(ControlledBody controlledBody);

		void onMouseMove(double from_x, double from_y, double to_x, double to_y) override;
		
		void onKeyPress(int key) override;
		void onKeyRelease(int key) override;

		void update(double dt) override;

	private:
		ControlledBody m_controlledBody;

		double m_scrollSpeed;
		double m_moveSpeed;

		bool m_moveForward;
		bool m_moveBack;
		bool m_moveRight;
		bool m_moveLeft;
	};
}

#endif