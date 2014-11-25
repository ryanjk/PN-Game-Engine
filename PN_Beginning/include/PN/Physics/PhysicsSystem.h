#ifndef PN_PHYSICS_SYS_H
#define PN_PHYSICS_SYS_H

#include "PN/ECS/Component/TransformComponent.h"

#include <vector>
#include <functional>

namespace pn {
	class GameState;
	class Entity;
	class MatrixStack;
	struct CollisionData;

	class PhysicsSystem {
	public:
		PhysicsSystem();
		void startUp(GameState* state);
		void shutdown();

		void run(double dt);

		const std::map<EntityID, std::shared_ptr<pn::BoundingContainer>>& getBoundingContainers() const;

		inline void updateEntity(Entity& entity, double dt);

		void translateWorld(TransformComponent& transformComponent, const vec3& by);
		void translateLocal(TransformComponent& transformComponent, const vec3& by);

		void scale(TransformComponent& transformComponent, const vec3& by);

		void rotateLocal(TransformComponent& transformComponent, const vec3& eulerAngles);
		void rotatePitchLocal(TransformComponent& transformComponent, float pitch);
		void rotateYawLocal(TransformComponent& transformComponent, float yaw);
		void rotateRollLocal(TransformComponent& transformComponent, float roll);

		void rotateParent(TransformComponent& transformComponent, const vec3& eulerAngles, const vec3& point = { 0.0f, 0.0f, 0.0f });
		void rotatePitchParent(TransformComponent& transformComponent, float pitch, const vec3& point = { 0.0f, 0.0f, 0.0f });
		void rotateYawParent(TransformComponent& transformComponent, float yaw, const vec3& point = { 0.0f, 0.0f, 0.0f });
		void rotateRollParent(TransformComponent& transformComponent, float roll, const vec3& point = { 0.0f, 0.0f, 0.0f });

		void setTranslation(TransformComponent& transformComponent, const vec3& translation);
		void setScale(TransformComponent& transformComponent, const vec3& scale);
		void setRotation(TransformComponent& transformComponent, const vec3& rotation);

	private:
		GameState* m_gameState;

		void updateTransformMatrix(TransformComponent& transformComponent);

		void integrateWorld(double dt);
		void updateBoundingContainers(EntityID current_entityID, MatrixStack& matrixStack);

		void detectCollisions(std::vector<CollisionData>& collisionData);
		std::function<bool(BoundingContainer*, BoundingContainer*, CollisionData&)> getCollisionTestFunc(BoundingContainerType, BoundingContainerType);

		bool OBB_OBB(BoundingContainer*, BoundingContainer*, CollisionData&);

		std::map<EntityID, std::shared_ptr<pn::BoundingContainer>> m_boundingContainerMap;
		std::map<EntityID, bool> m_checkedForCollision;
		std::vector<EntityID> m_movingEntities;

		void setEntityWorldMatrixDirty(Entity& entity);
		void setEntityWorldMatrixDirty(TransformComponent& transformComponent);

	};
}

#endif