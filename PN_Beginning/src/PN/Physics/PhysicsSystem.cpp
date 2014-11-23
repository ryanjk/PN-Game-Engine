#include "PN/Physics/PhysicsSystem.h"

#include "PN/GameState/GameState.h"

#include "PN/ECS/Entity/Entity.h"
#include "PN/ECS/Component/MoveComponent.h"

#include "glm/gtx/euler_angles.hpp"

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

		updateEntity(entity, dt);
	}
}

void pn::PhysicsSystem::setEntityWorldMatrixDirty(Entity& entity) {
	auto& children = entity.getChildrenID();
	for (auto child_id : children) {
		m_gameState->m_worldMatrix_needUpdate[child_id] = true;
		auto& child_entity = m_gameState->getEntity(child_id);
		setEntityWorldMatrixDirty(child_entity);
	}
}

void pn::PhysicsSystem::setEntityWorldMatrixDirty(TransformComponent& transformComponent) {
	auto tc_ownerID = transformComponent.getOwner();
	auto& owner = m_gameState->getEntity(tc_ownerID);
	setEntityWorldMatrixDirty(owner);
}

void pn::PhysicsSystem::updateEntity(Entity& entity, double dt) {
	static const vec3 zero_vec3 = vec3(0.0f, 0.0f, 0.0f);

	auto& moveComponent = *std::dynamic_pointer_cast<pn::MoveComponent>(entity.getComponent(pn::ComponentType::MOVE));
	auto& transformComponent = *std::dynamic_pointer_cast<pn::TransformComponent>(entity.getComponent(pn::ComponentType::TRANSFORM));

	const auto& current_velocity = moveComponent.getVelocity();
	const auto& current_acceleration = moveComponent.getAcceleration();

	bool willMove = (current_velocity != zero_vec3 ) || (current_acceleration != zero_vec3);
	if (willMove) {
		const auto& current_position = transformComponent.getTranslation();
		vec3 new_velocity = current_velocity + current_acceleration * (float)dt;
		vec3 new_position = 0.5f * (glm::pow((float)dt, 2.0f)) * current_acceleration + current_velocity * (float)dt + current_position;

		moveComponent.setVelocity(new_velocity);
		translateLocal(transformComponent, new_position - current_position);
	}

	const auto& current_angularVelocity = moveComponent.getAngularVelocity();
	const auto& current_angularAcceleration = moveComponent.getAngularAcceleration();

	bool willRotate = (current_angularVelocity != zero_vec3) || (current_angularAcceleration != zero_vec3);
	if (willRotate) {
		const auto& current_rotation = transformComponent.getRotation();
		vec3 new_angularVelocity = current_angularVelocity + current_angularAcceleration * (float)dt;
		vec3 new_rotation = 0.5f * (glm::pow((float)dt, 2.0f)) * current_angularAcceleration + current_angularVelocity * (float)dt + current_rotation;

		moveComponent.setAngularVelocity(new_angularVelocity);
		rotateLocal(transformComponent, new_rotation - current_rotation);
	}

	if (willMove || willRotate) {
		m_gameState->m_worldMatrix_needUpdate[entity.getID()] = true;
		setEntityWorldMatrixDirty(entity);
	}
}

/*
Transform Component modifications
*/

void pn::PhysicsSystem::translateWorld(TransformComponent& transformComponent, const vec3& by) {
	transformComponent.m_translation += by;
	transformComponent.m_transformMatrix[3] = vec4(transformComponent.m_translation.x, transformComponent.m_translation.y, transformComponent.m_translation.z, 1.0f);
	setEntityWorldMatrixDirty(transformComponent);
}

void pn::PhysicsSystem::translateLocal(TransformComponent& transformComponent, const vec3& by) {
	auto pitch = make_quat({ 1.0f, 0.0f, 0.0f }, transformComponent.m_rotation[0]);
	auto yaw = make_quat({ 0.0f, 1.0f, 0.0f }, transformComponent.m_rotation[1]);
	auto roll = make_quat({ 0.0f, 0.0f, 1.0f }, transformComponent.m_rotation[2]);

	vec3 rotatedDirection = yaw * pitch * roll * by;
	translateWorld(transformComponent, rotatedDirection);

	setEntityWorldMatrixDirty(transformComponent);
}

void pn::PhysicsSystem::setTranslation(TransformComponent& transformComponent, const vec3& translation) {
	transformComponent.m_translation = translation;
	transformComponent.m_transformMatrix[3] = vec4(translation.x, translation.y, translation.z, 1.0f);

	setEntityWorldMatrixDirty(transformComponent);
}

void pn::PhysicsSystem::scale(TransformComponent& transformComponent, const vec3& by) {
	transformComponent.m_scale *= by;
	updateTransformMatrix(transformComponent);

	setEntityWorldMatrixDirty(transformComponent);
}

void pn::PhysicsSystem::setScale(TransformComponent& transformComponent, const vec3& scale) {
	transformComponent.m_scale = scale;
	updateTransformMatrix(transformComponent);

	setEntityWorldMatrixDirty(transformComponent);
}

void pn::PhysicsSystem::rotateLocal(TransformComponent& transformComponent, const vec3& eulerAngles) {
	transformComponent.m_rotation += eulerAngles;

	rotateParent(transformComponent, eulerAngles, transformComponent.m_translation);

	setEntityWorldMatrixDirty(transformComponent);
}

void pn::PhysicsSystem::rotateParent(TransformComponent& transformComponent, const vec3& eulerAngles, const vec3& point) {
	transformComponent.m_transformMatrix[3].xyz = transformComponent.m_transformMatrix[3].xyz - point;
	transformComponent.m_transformMatrix = glm::eulerAngleYXZ(eulerAngles[1], eulerAngles[0], eulerAngles[2]) * transformComponent.m_transformMatrix;
	transformComponent.m_transformMatrix[3].xyz = transformComponent.m_transformMatrix[3].xyz + point;

	transformComponent.m_translation = transformComponent.m_transformMatrix[3].xyz;

	setEntityWorldMatrixDirty(transformComponent);
}

void pn::PhysicsSystem::rotatePitchLocal(TransformComponent& transformComponent, float pitch) {
	transformComponent.m_rotation[0] += pitch;

	rotatePitchParent(transformComponent, pitch, transformComponent.m_transformMatrix[3].xyz);

	setEntityWorldMatrixDirty(transformComponent);
}

void pn::PhysicsSystem::rotatePitchParent(TransformComponent& transformComponent, float pitch, const vec3& point) {
	transformComponent.m_transformMatrix[3].xyz = transformComponent.m_transformMatrix[3].xyz - point;
	transformComponent.m_transformMatrix = glm::rotate(mat4(), pitch, vec3(1.0f, 0.0f, 0.0f)) * transformComponent.m_transformMatrix;
	transformComponent.m_transformMatrix[3].xyz = transformComponent.m_transformMatrix[3].xyz + point;

	transformComponent.m_translation = transformComponent.m_transformMatrix[3].xyz;

	updateTransformMatrix(transformComponent);

	setEntityWorldMatrixDirty(transformComponent);
}

void pn::PhysicsSystem::rotateYawLocal(TransformComponent& transformComponent, float yaw) {
	transformComponent.m_rotation[1] += yaw;

	rotateYawParent(transformComponent, yaw, transformComponent.m_transformMatrix[3].xyz);

	setEntityWorldMatrixDirty(transformComponent);
}

void pn::PhysicsSystem::rotateYawParent(TransformComponent& transformComponent, float yaw, const vec3& point) {
	transformComponent.m_transformMatrix[3].xyz = transformComponent.m_transformMatrix[3].xyz - point;
	transformComponent.m_transformMatrix = glm::rotate(mat4(), yaw, vec3(0.0f, 1.0f, 0.0f)) * transformComponent.m_transformMatrix;
	transformComponent.m_transformMatrix[3].xyz = transformComponent.m_transformMatrix[3].xyz + point;

	transformComponent.m_translation = transformComponent.m_transformMatrix[3].xyz;
	updateTransformMatrix(transformComponent);

	setEntityWorldMatrixDirty(transformComponent);
}

void pn::PhysicsSystem::rotateRollLocal(TransformComponent& transformComponent, float roll) {
	transformComponent.m_rotation[2] += roll;

	rotateRollParent(transformComponent, roll, transformComponent.m_translation);

	setEntityWorldMatrixDirty(transformComponent);
}

void pn::PhysicsSystem::rotateRollParent(TransformComponent& transformComponent, float roll, const vec3& point) {
	transformComponent.m_transformMatrix[3].xyz = transformComponent.m_transformMatrix[3].xyz - point;
	transformComponent.m_transformMatrix = glm::rotate(mat4(), roll, vec3(0.0f, 0.0f, 1.0f)) * transformComponent.m_transformMatrix;
	transformComponent.m_transformMatrix[3].xyz = transformComponent.m_transformMatrix[3].xyz + point;

	transformComponent.m_translation = transformComponent.m_transformMatrix[3].xyz;
	updateTransformMatrix(transformComponent);

	setEntityWorldMatrixDirty(transformComponent);
}

void pn::PhysicsSystem::setRotation(TransformComponent& transformComponent, const vec3& rotation) {
	transformComponent.m_rotation = rotation;

	updateTransformMatrix(transformComponent);

	setEntityWorldMatrixDirty(transformComponent);
}

void pn::PhysicsSystem::updateTransformMatrix(TransformComponent& transformComponent) {
	transformComponent.m_transformMatrix =
		glm::translate(mat4(), transformComponent.m_translation) *
		glm::eulerAngleYXZ(transformComponent.m_rotation[1], transformComponent.m_rotation[0], transformComponent.m_rotation[2]) *
		glm::scale(mat4(), transformComponent.m_scale);

	setEntityWorldMatrixDirty(transformComponent);
}