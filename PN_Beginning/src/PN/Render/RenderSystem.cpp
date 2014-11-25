#include "PN/Render/RenderSystem.h"

#include "PN/GameState/GameState.h"

#include "PN/ECS/Component/RenderComponent.h"
#include "PN/ECS/Component/TransformComponent.h"

#include "PN/Physics/BoundingContainer/BoundingBox.h"

#include "PN/Settings/SettingsManager.h"

static auto& settings = pn::SettingsManager::g_SettingsManager;

pn::RenderSystem::RenderSystem() {}

void pn::RenderSystem::startUp(pn::GameState* state) {
	m_state = state;

	for (auto& entity_i : m_state->m_entities) {
		auto& entity = *entity_i;

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

}

void pn::RenderSystem::run() {
	DrawCallContainer drawCalls{};

	pn::MatrixStack matrixStack;

	for (auto root_children : m_state->getRootEntity().getChildrenID()) {
		buildDrawCalls(root_children, matrixStack, drawCalls);
	}

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	renderDrawCalls(drawCalls);

	glUseProgram(0);
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

#ifdef _DEBUG
	// Camera position
	const mat4& camera_world_transform = m_state->getEntityWorldTransform(m_activeCamera->getID(), nullptr);

	// Get view transform from camera
	const auto view_transform(glm::inverse(camera_world_transform));
	
	for (auto entity : m_state->m_entities) {
		if (entity->getName().getText() == "player") continue;
		auto map_itr = m_state->m_physicsSystem.getBoundingContainers().find(entity->getID());
		if (map_itr != m_state->m_physicsSystem.getBoundingContainers().end()) {
			auto& boundingContainer = map_itr->second;

			GLuint collision_draw_program = m_state->m_resources.getMaterial("collision_container_draw.sp").getGLProgramObject();
			glUseProgram(collision_draw_program);

			int worldIndex = glGetUniformLocation(collision_draw_program, "world");
			const auto& world_matrix = m_state->getEntityWorldTransform(entity->getID(), nullptr);
			glUniformMatrix4fv(worldIndex, 1, GL_FALSE, glm::value_ptr(world_matrix));

			int viewIndex = glGetUniformLocation(collision_draw_program, "view");
			glUniformMatrix4fv(viewIndex, 1, GL_FALSE, glm::value_ptr(view_transform));

			int projIndex = glGetUniformLocation(collision_draw_program, "proj");
			glUniformMatrix4fv(projIndex, 1, GL_FALSE, glm::value_ptr(settings.getProjectionMatrix()));

			if (boundingContainer->getContainerType() == pn::BoundingContainerType::BOUNDING_BOX) {
				glBindVertexArray(boundingContainer->getVAO());
				glBindBuffer(GL_ARRAY_BUFFER, boundingContainer->getVBO());
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boundingContainer->getIBO());
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, NULL);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

				glDisableVertexAttribArray(0);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glUseProgram(0);
			}
		}
	}

#endif
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
		auto& mesh = m_state->m_resources.getMesh(renderComponent->getMeshFilename());

		// world position of entity
		const mat4& worldTransform = m_state->getEntityWorldTransform(current_entity_ID, [&]()->mat4 { return matrixStack.multiply(); });

		// put draw call into container
		pn::DrawCall drawCall({ worldTransform, renderComponent });
		drawCalls.insert({ (mesh.getVAO() << 2) | material.getMaterialID(), std::move(drawCall) });
	}

	// Continue visiting the entity's children
	for (auto childID : current_entity.getChildrenID()) {
		buildDrawCalls(childID, matrixStack, drawCalls);
	}

	matrixStack.pop();
}

void pn::RenderSystem::renderDrawCalls(DrawCallContainer& drawCalls) {

	// Camera position
	const mat4& camera_world_transform = m_state->getEntityWorldTransform(m_activeCamera->getID(), nullptr);

	// Get view transform from camera
	const auto view_transform(glm::inverse(camera_world_transform));

	HashValue last_material_rendered = 0;
	HashValue last_mesh_rendered = 0;

	// render each draw call -- it goes in order of material ID because the map is sorted by this
	for (auto& drawCallIter : drawCalls) {
		auto& drawCall = drawCallIter.second;

		auto current_material_hash = drawCall.renderComponent->getMaterialFilename().getHash();
		auto& current_material = m_state->m_resources.getMaterial(current_material_hash);

		auto current_mesh_hash = drawCall.renderComponent->getMeshFilename().getHash();
		auto& current_mesh = m_state->m_resources.getMesh(current_mesh_hash);

		auto current_texture_hash = drawCall.renderComponent->getDiffuse().getHash();
		auto& current_texture = m_state->m_resources.getImage(current_texture_hash);

		// If the current program is not the same as the last one used, set new global uniforms

		if (current_material_hash != last_material_rendered) {
			current_material.setGlobalUniforms(*m_state, m_lights, camera_world_transform, view_transform);
			last_material_rendered = current_material_hash;
		}

		// if rendering a different mesh, change VAO, otherwise keep from last time
		bool swapVAO = current_mesh_hash != last_mesh_rendered;
		last_mesh_rendered = current_mesh_hash;

		current_material.setInstanceUniforms(drawCall.world_transform, *drawCall.renderComponent, current_mesh, current_texture, swapVAO);

		glDrawArraysInstanced(GL_TRIANGLES, 0, current_mesh.getNumVertices(), 1);

		current_material.postRender();
	}
}