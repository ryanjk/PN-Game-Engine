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

		vec3 getAcceleration() const;
		void setAcceleration(vec3 acceleration);

		vec3 getVelocity() const;
		void setVelocity(vec3 velocity);

	private:
		vec3 m_acceleration = { 0.0f, 0.0f, 1.0f };
		vec3 m_velocity = { 0.0f, 0.0f, 0.0f };
	};
}

#endif