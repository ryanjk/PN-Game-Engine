#include "PN/Physics/PhysicsSystem.h"

#include "PN/GameState/GameState.h"

#include "PN/ECS/Component/MoveComponent.h"
#include "PN/ECS/Component/TransformComponent.h"

pn::PhysicsSystem::PhysicsSystem() : m_gameState() {}

void pn::PhysicsSystem::startUp(pn::GameState* state) {
	m_gameState = state;
}

void pn::PhysicsSystem::shutdown() {}

void pn::PhysicsSystem::run(double dt) {
	integrateWorld(dt);
}

void pn::PhysicsSystem::integrateWorld(double dt) {
	static const EntityID playerID = pn::PString("player").getHash();
	
	for (auto& entity_ptr : m_gameState->m_entities) {
		auto& entity = *entity_ptr;
		if (entity.getID() == playerID) continue;
		if (!entity.hasComponents(pn::ComponentType::MOVE | pn::ComponentType::TRANSFORM)) continue;

		auto& moveComponent = *std::dynamic_pointer_cast<pn::MoveComponent>(entity.getComponent(pn::ComponentType::MOVE));
		auto& transformComponent = *std::dynamic_pointer_cast<pn::TransformComponent>(entity.getComponent(pn::ComponentType::TRANSFORM));

		const auto& current_velocity = moveComponent.getVelocity();
		const auto& current_acceleration = moveComponent.getAcceleration();
		const auto& current_position = transformComponent.getTranslation();

		vec3 new_velocity = current_velocity + current_acceleration * (float)dt;
		vec3 new_position = 0.5f * (glm::pow((float)dt, 2.0f)) * current_acceleration + current_velocity * (float)dt + current_position;

		moveComponent.setVelocity(new_velocity);
		transformComponent.translateLocal(new_position - current_position);

		const auto& current_angularVelocity = moveComponent.getAngularVelocity();
		const auto& current_angularAcceleration = moveComponent.getAngularAcceleration();
		const auto& current_rotation = transformComponent.getRotation();

		vec3 new_angularVelocity = current_angularVelocity + current_angularAcceleration * (float)dt;
		vec3 new_rotation = 0.5f * (glm::pow((float)dt, 2.0f)) * current_angularAcceleration + current_angularVelocity * (float)dt + current_rotation;

		moveComponent.setAngularVelocity(new_angularVelocity);
		transformComponent.rotateLocal(new_rotation - current_rotation);
	}
}