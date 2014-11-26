#ifndef PN_RENDER_SYSTEM_H
#define PN_RENDER_SYSTEM_H

#include "PN/ECS/Entity/Entity.h"

#include "PN/Render/DrawCall.h"

#include "PN/Util/MatrixStack.h"

#include "PN/Physics/BoundingContainer/BoundingContainer.h"

#include <memory>
#include <map>

namespace pn {
	class RenderSystem {
	public:
		RenderSystem();
		void startUp(pn::GameState* state);
		void shutdown();

		void run();

	private:
		void buildDrawCalls(EntityID current_entity_ID, MatrixStack& matrixStack, DrawCallContainer& drawCalls);
		void renderDrawCalls(DrawCallContainer& drawCalls);

		void createCollisionPrimitiveGLObjects();

		struct glObjects {
			GLuint VAO;
			GLuint VBO;
			GLuint IBO;
			unsigned int num_tris;
		};

		std::map<pn::BoundingContainerType, glObjects> m_primitiveGLObjects;

		pn::GameState* m_state;
		std::vector<EntityID> m_lights;
		Entity* m_activeCamera;
	};
}

#endif