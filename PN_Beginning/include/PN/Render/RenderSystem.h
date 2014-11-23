#ifndef PN_RENDER_SYSTEM_H
#define PN_RENDER_SYSTEM_H

#include "PN/ECS/Entity/Entity.h"

#include "PN/Render/DrawCall.h"

#include "PN/Util/MatrixStack.h"

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

		pn::GameState* m_state;
		std::vector<EntityID> m_lights;
		Entity* m_activeCamera;

		GLuint g_buffer;
		GLuint g_buffer_tex[3];
		GLuint deferred_program;
	};
}

#endif