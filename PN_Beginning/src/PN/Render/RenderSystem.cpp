#include "PN/Render/RenderSystem.h"

#include "PN/GameState/GameState.h"

#include "PN/ECS/Component/RenderComponent.h"
#include "PN/ECS/Component/TransformComponent.h"

pn::RenderSystem::RenderSystem() {}

void pn::RenderSystem::startUp(pn::GameState* state) {
	m_state = state;

	// set up renderable for each entity that needs one
	for (auto& entity_i : m_state->m_entities) {
		auto& entity = *entity_i;
		bool hasRenderComponent = entity.hasComponents(pn::ComponentType::RENDER);
		if (hasRenderComponent) {
			auto& renderComponent = std::dynamic_pointer_cast<pn::RenderComponent>(entity.getComponent(pn::ComponentType::RENDER));
			auto& entity_material = m_state->m_resources.getMaterial(renderComponent->getMaterialFilename());

			Renderable entity_renderable;

			entity_material.setUpRenderable(entity_renderable, *renderComponent, m_state->m_resources);

			m_renderables.insert({ entity.getID(), entity_renderable });
		}

		// add to light entities if applicable
		bool hasLightComponent = entity.hasComponents(pn::ComponentType::LIGHT);
		if (hasLightComponent) {
			m_lights.push_back(entity.getID());
		}
	}

	// set the state's camera
	auto& camera = m_state->getEntity("camera");
	m_activeCamera = &camera;
}

void pn::RenderSystem::shutdown() {
	// delete gl objects used by renderables
	for (auto& i : m_renderables) {
		auto& renderable = i.second;

		glDeleteVertexArrays(1, &renderable.VAO);

		glDeleteBuffers(1, &renderable.VBO_v);
		glDeleteBuffers(1, &renderable.VBO_vn);
		glDeleteBuffers(1, &renderable.VBO_vt);

		glDeleteTextures(1, &renderable.TBO_diffuse);
		glDeleteSamplers(1, &renderable.sampler_diffuse);
	}
}

void pn::RenderSystem::run() {
	DrawCallContainer drawCalls;

	pn::MatrixStack matrixStack;

	for (auto root_children : m_state->getRootEntity().getChildrenID()) {
		buildDrawCalls(root_children, matrixStack, drawCalls);
	}

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	renderDrawCalls(drawCalls);

	glUseProgram(0);
}

void pn::RenderSystem::buildDrawCalls(EntityID current_entity_ID, MatrixStack& matrixStack, DrawCallContainer& drawCalls) {
	const auto& current_entity = m_state->getEntity(current_entity_ID);

	// If the entity does not have a position in space, then its children do not, so stop travelling down this branch
	bool hasTransform = current_entity.hasComponents(pn::ComponentType::TRANSFORM);
	if (!hasTransform) {
		return;
	}

	// Push the entity's matrix onto the matrix stack
	const auto& transformComponent = std::dynamic_pointer_cast<pn::TransformComponent>(current_entity.getComponent(pn::ComponentType::TRANSFORM));
	matrixStack.push(transformComponent->getTransformMatrix());

	// If the entity is renderable, create a draw call and add it to the draw call container
	bool hasRender = current_entity.hasComponents(pn::ComponentType::RENDER);
	if (hasRender) {
		auto& renderComponent = std::dynamic_pointer_cast<pn::RenderComponent>(current_entity.getComponent(pn::ComponentType::RENDER));
		auto& material = m_state->m_resources.getMaterial(renderComponent->getMaterialFilename());

		auto& entityRenderable = m_renderables[current_entity_ID];

		// world position of entity
		mat4 worldTransform(matrixStack.multiply());

		// amount of vertices to render
		unsigned int num_vertices = m_state->m_resources.getMesh(entityRenderable.mesh).getVertices().size();

		// put draw call into container
		pn::DrawCall drawCall({ worldTransform, entityRenderable, material, num_vertices, renderComponent });
		drawCalls.insert({ std::move(material.getMaterialID()), std::move(drawCall) });
	}

	// Continue visiting the entity's children
	for (auto childID : current_entity.getChildrenID()) {
		buildDrawCalls(childID, matrixStack, drawCalls);
	}

	matrixStack.pop();
}

void pn::RenderSystem::renderDrawCalls(DrawCallContainer& drawCalls) {

	// Camera position
	const mat4& camera_world_transform = m_state->getEntityWorldTransform(m_activeCamera->getID());

	// Get view transform from camera
	const auto view_transform(glm::inverse(camera_world_transform));

	HashValue last_material_rendered = 0;
	// render each draw call -- it goes in order of material ID because the map is sorted by this
	for (auto& drawCallIter : drawCalls) {
		auto& drawCall = drawCallIter.second;
		auto current_material_hash = drawCall.material.getMaterialFilename().getHash();
		auto& current_material = drawCall.material;

		// If the current program is not the same as the last one used, set new global uniforms
		if (!(current_material_hash == last_material_rendered)) {
			current_material.setGlobalUniforms(*m_state, m_lights, camera_world_transform, view_transform);
			last_material_rendered = current_material_hash;
		}

		current_material.setInstanceUniforms(drawCall);
		glDrawArraysInstanced(GL_TRIANGLES, 0, drawCall.num_vertices, 1);

		current_material.postRender();

	}
}