#ifndef MOVE_COMPONENT_H
#define MOVE_COMPONENT_H

#include "PN/ECS/Component/IComponent.h"

#include "PN/Util/Math.h"
#include "PN/Util/PString.h"

namespace pn {
	class MoveComponent : public pn::IComponent {
	public:
		static std::shared_ptr<MoveComponent> make(const ComponentData& data, pn::ResourceManager& resources);
		ComponentType getType() const override;

		MoveComponent();

		const vec3& getAcceleration() const;
		void setAcceleration(const vec3& acceleration);

		const vec3& getVelocity() const;
		void setVelocity(const vec3& velocity);

		const vec3& getAngularVelocity() const;
		void setAngularVelocity(const vec3& angularVelocity);

		const vec3& getAngularAcceleration() const;
		void setAngularAcceleration(const vec3& angularAcceleration);

	private:
		vec3 m_acceleration = { 0.0f, 0.0f, 0.0f };
		vec3 m_velocity = { 0.0f, 0.0f, 0.0f };
		vec3 m_angularAcceleration = { 0.0f, 0.0f, 0.0f };
		vec3 m_angularVelocity = { 0.0f, 0.0f, 0.0f };
	};
}

#endif