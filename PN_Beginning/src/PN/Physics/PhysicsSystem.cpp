#include "PN/Physics/PhysicsSystem.h"
#include "PN/Physics/BoundingContainer/BoundingBox.h"
#include "PN/Physics/CollisionData.h"

#include "PN/GameState/GameState.h"

#include "PN/ECS/Entity/Entity.h"
#include "PN/ECS/Component/MoveComponent.h"
#include "PN/ECS/Component/CollideComponent.h"
#include "PN/ECS/Component/RenderComponent.h"

#include "PN/Util/MatrixStack.h"
#include "PN/Util/Math.h"

#include "glm/gtx/euler_angles.hpp"

#include <algorithm>
#include <limits>

pn::PhysicsSystem::PhysicsSystem() : m_gameState(), m_boundingContainerMap(), m_checkedForCollision(), m_movingEntities() {}

void pn::PhysicsSystem::startUp(pn::GameState* state) {
	m_gameState = state;

	// add all bounding containers to physics system tracking
	for (auto entity_ptr : m_gameState->m_entities) {
		auto& entity = *entity_ptr;

		bool hasBoundingContainer = entity.hasComponents(pn::COLLIDE);
		if (hasBoundingContainer) {
			
			auto collideComponent = std::dynamic_pointer_cast<pn::CollideComponent>(entity.getComponent(pn::ComponentType::COLLIDE));
			float containerScale = collideComponent->getScale();

			// if entity has model-provided bounding container, update by component-provided scale factor then add it to container
			bool hasMeshBoundingContainer = entity.hasComponents(pn::RENDER);
			if (hasMeshBoundingContainer) {
				auto renderComponent = std::dynamic_pointer_cast<pn::RenderComponent>(entity.getComponent(pn::ComponentType::RENDER));
				auto boundingContainer_ptr = m_gameState->m_resources.getMesh(renderComponent->getMeshFilename()).getBoundingContainer();
				boundingContainer_ptr->update(glm::scale(mat4(), vec3(containerScale)));
				m_boundingContainerMap.insert(std::make_pair(entity.getID(), boundingContainer_ptr));
			}

			// entity does not have mesh-provided bounding container, so create one
			else {
				switch (collideComponent->getContainerType()) {
				case pn::BoundingContainerType::BOUNDING_BOX:
				{
					auto boundingContainer_ptr = std::make_shared<pn::BoundingBox>(containerScale, containerScale, containerScale);
					m_boundingContainerMap.insert(std::make_pair(entity.getID(), boundingContainer_ptr));
				}
					break;
				default:
					break;
				}
			}

			// if it moves, add it entities to check all others for collisions with
			bool hasMove = entity.hasComponents(pn::MOVE);
			if (hasMove) {
				m_movingEntities.push_back(entity.getID());
				m_checkedForCollision.insert(std::make_pair(entity.getID(), false));
			}
		}
	}
}

void pn::PhysicsSystem::shutdown() {}

void pn::PhysicsSystem::run(double dt) {
	integrateWorld(dt);

	// update bounding containers for every collidable entity
	pn::MatrixStack matrixStack{};
	for (auto root_children : m_gameState->getRootEntity().getChildrenID()) {
		updateBoundingContainers(root_children, matrixStack);	
	}

	std::vector < CollisionData > collisionData{};
	detectCollisions(collisionData);
}

void pn::PhysicsSystem::integrateWorld(double dt) {
	static const EntityID playerID = pn::PString("player").getHash();
	
	for (auto& entity_ptr : m_gameState->m_entities) {
		auto& entity = *entity_ptr;
		if (entity.getID() == playerID) continue; // player is updated more frequently by the state itself
		if (!entity.hasComponents(pn::ComponentType::MOVE | pn::ComponentType::TRANSFORM)) continue;

		updateEntity(entity, dt);
	}
}

void pn::PhysicsSystem::detectCollisions(std::vector<CollisionData>& collisionDataContainer) {
	for (auto entityID : m_movingEntities) {
		auto& entity_boundingContainer = *(m_boundingContainerMap.find(entityID)->second);
		for (auto key : m_boundingContainerMap) {
			EntityID otherID = key.first;
			if (otherID == entityID) continue;
			if (m_checkedForCollision.find(otherID)->second == true) continue;

			auto& other_boundingContainer = *key.second;
			auto test_collision_function = getCollisionTestFunc(entity_boundingContainer.getContainerType(), other_boundingContainer.getContainerType());

			CollisionData collisionData{};
			collisionData.e1 = entityID;
			collisionData.e2 = otherID;
			bool isCollision = test_collision_function(&entity_boundingContainer, &other_boundingContainer, collisionData);
			if (isCollision) {
		//		std::cout << "COLLISION!!!!!!!!!!\n";
				collisionDataContainer.push_back(collisionData);
			}

		}
		m_checkedForCollision.find(entityID)->second = true;
	}
	for (auto entityID : m_movingEntities) {
		m_checkedForCollision.find(entityID)->second = false;
	}
}

std::function<bool(pn::BoundingContainer*, pn::BoundingContainer*, pn::CollisionData&)>
pn::PhysicsSystem::getCollisionTestFunc(BoundingContainerType b1, BoundingContainerType b2) {
	if (b1 == pn::BoundingContainerType::BOUNDING_BOX) {
		if (b2 == pn::BoundingContainerType::BOUNDING_BOX) {
			return ([&](BoundingContainer* b1, BoundingContainer* b2, pn::CollisionData& collisionData){ return OBB_OBB(b1, b2, collisionData); });
		}

		else {
			return nullptr;
		}
	}

	else {
		assert(false);
		return ([&](BoundingContainer* b1, BoundingContainer* b2, pn::CollisionData& collisionData){ return OBB_OBB(b1, b2, collisionData); });
	}
}

bool pn::PhysicsSystem::OBB_OBB(pn::BoundingContainer* b1, pn::BoundingContainer* b2, CollisionData& collisionData) {
	static const float maxFloat = std::numeric_limits<float>::max();
	static const float minFloat = std::numeric_limits<float>::min();

	pn::BoundingBox* bb1 = (pn::BoundingBox*)b1;
	pn::BoundingBox* bb2 = (pn::BoundingBox*)b2;

	const mat4& bb1_front_square = bb1->getFrontQuad();
	const mat4& bb1_back_square = bb1->getBackQuad();

	const mat4& bb2_front_square = bb2->getFrontQuad();
	const mat4& bb2_back_square = bb2->getBackQuad();

	const vec3 bb1_corners[8] =
	{
		vec3(bb1_front_square[0].xyz), vec3(bb1_front_square[1].xyz), vec3(bb1_front_square[2].xyz), vec3(bb1_front_square[3].xyz),
		vec3(bb1_back_square[0].xyz), vec3(bb1_back_square[1].xyz), vec3(bb1_back_square[2].xyz), vec3(bb1_back_square[3].xyz)
	};

	const vec3 bb2_corners[8] =
	{
		vec3(bb2_front_square[0].xyz), vec3(bb2_front_square[1].xyz), vec3(bb2_front_square[2].xyz), vec3(bb2_front_square[3].xyz),
		vec3(bb2_back_square[0].xyz), vec3(bb2_back_square[1].xyz), vec3(bb2_back_square[2].xyz), vec3(bb2_back_square[3].xyz)
	};

	static auto proj_BB_onto_axis = [](const vec3* bb_corners, const vec3& axis, float& minProj, float& maxProj){
		for (int i = 0; i < 8; i++) {
			float proj = glm::dot(bb_corners[i], axis);
			if (proj < minProj) {
				minProj = proj;
			}
			if (proj > maxProj) {
				maxProj = proj;
			}
		}
	};

	// return true iff there is overlap on the axis
	static auto is_overlap = [&](const vec3& axis) -> bool {
		float bb1_minProj = maxFloat;
		float bb1_maxProj = minFloat;
		proj_BB_onto_axis(bb1_corners, axis, bb1_minProj, bb1_maxProj);
		float bb2_minProj = maxFloat;
		float bb2_maxProj = minFloat;
		proj_BB_onto_axis(bb2_corners, axis, bb2_minProj, bb2_maxProj);

		if (((bb1_minProj < bb2_maxProj) && (bb1_maxProj > bb2_minProj)) ||
			((bb2_minProj < bb1_maxProj) && (bb2_maxProj > bb1_minProj)) ||
			((bb1_minProj > bb2_minProj) && (bb1_maxProj < bb2_maxProj)) ||
			((bb2_minProj > bb1_minProj) && (bb2_maxProj < bb1_maxProj))) {
			return true;
		}
		else {
			return false;
		}
	};

	vec3 axis1 = glm::cross(bb1_corners[1] - bb1_corners[0], bb1_corners[3] - bb1_corners[0]);
	if (!is_overlap(axis1)) {
		return false;
	}

	vec3 axis2 = glm::cross(bb1_corners[2] - bb1_corners[3], bb1_corners[7] - bb1_corners[3]);
	if (!is_overlap(axis2)) {
		return false;
	}

	vec3 axis3 = glm::cross(bb1_corners[5] - bb1_corners[1], bb1_corners[2] - bb1_corners[1]);
	if (!is_overlap(axis3)) {
		return false;
	}

	vec3 axis4 = glm::cross(bb2_corners[1] - bb2_corners[0], bb2_corners[3] - bb2_corners[0]);
	if (!is_overlap(axis4)) {
		return false;
	}

	vec3 axis5 = glm::cross(bb2_corners[2] - bb2_corners[3], bb2_corners[7] - bb2_corners[3]);
	if (!is_overlap(axis5)) {
		return false;
	}

	vec3 axis6 = glm::cross(bb2_corners[5] - bb2_corners[1], bb2_corners[2] - bb2_corners[1]);
	if (!is_overlap(axis6)) {
		return false;
	}

	return true;

}

void pn::PhysicsSystem::updateBoundingContainers(EntityID entityID, MatrixStack& matrixStack) {
	const auto& current_entity = m_gameState->getEntity(entityID);

	// If the entity does not have a position in space, then its children do not, so stop travelling down this branch
	bool hasTransform = current_entity.hasComponents(pn::ComponentType::TRANSFORM);
	if (!hasTransform) {
		return;
	}

	// Push the entity's matrix onto the matrix stack
	const auto& transformComponent = std::dynamic_pointer_cast<pn::TransformComponent>(current_entity.getComponent(pn::ComponentType::TRANSFORM));
	matrixStack.push(transformComponent->getTransformMatrix());

	// If the enitity has a bounding container, update its container's position 
	bool hasBoundingContainer = current_entity.hasComponents(pn::ComponentType::COLLIDE);
	if (hasBoundingContainer) {

		const mat4& worldTransform = m_gameState->getEntityWorldTransform(entityID, [&]()->mat4 { return matrixStack.multiply(); });
		auto boundingContainer = m_boundingContainerMap[entityID];
		boundingContainer->update(worldTransform);
	}

	// Continue visiting the entity's children
	for (auto childID : current_entity.getChildrenID()) {
		updateBoundingContainers(childID, matrixStack);
	}

	matrixStack.pop();
}

void pn::PhysicsSystem::setEntityWorldMatrixDirty(Entity& entity) {
	m_gameState->m_worldMatrix_needUpdate[entity.getID()] = true;
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