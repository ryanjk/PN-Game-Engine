#ifndef FIRST_PERSON_LISTENER_H
#define FIRST_PERSON_LISTENER_H

#include "PN/ECS/Entity/Entity.h"
#include "PN/ECS/Component/MoveComponent.h"
#include "PN/ECS/Component/TransformComponent.h"

#include "PN/Input/InputEventListener.h"

#include "PN/Window/WindowManager.h"

#include "PN/Settings/SettingsManager.h"

#include "PN/Physics/PhysicsSystem.h"

#include <memory>

using ControlledBody = pn::Entity* ;

namespace pn {
	class FirstPersonListener : public pn::InputEventListener {
	public:
		FirstPersonListener(ControlledBody controlledBody, PhysicsSystem* physicsSystem);

		void onMouseMove(double from_x, double from_y, double to_x, double to_y) override;
		
		void onKeyPress(int key) override;
		void onKeyRelease(int key) override;

		void update(double dt) override;

	private:
		std::shared_ptr<MoveComponent> m_body_moveComponent;
		std::shared_ptr<TransformComponent> m_body_transformComponent;
		PhysicsSystem* m_physicsSystem;

		double m_scrollSpeed;
		float m_moveSpeed;

		bool move_forward;
		bool move_back;
		bool move_right;
		bool move_left;

		pn::mm::KeyEnum m_forward_key;
		pn::mm::KeyEnum m_back_key;
		pn::mm::KeyEnum m_right_key;
		pn::mm::KeyEnum m_left_key;
	};
}

#endif