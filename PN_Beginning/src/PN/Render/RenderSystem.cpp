#include "PN/Render/RenderSystem.h"

#include "PN/GameState/GameState.h"

#include "PN/ECS/Component/RenderComponent.h"
#include "PN/ECS/Component/TransformComponent.h"

#include "PN/Physics/BoundingContainer/BoundingBox.h"

#include "PN/Settings/SettingsManager.h"

static const auto& settings = pn::SettingsManager::g_SettingsManager;

pn::RenderSystem::RenderSystem() {}

void pn::RenderSystem::startUp(pn::GameState* state) {
	m_state = state;

	// set up renderable for each entity that needs one
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

	glGenFramebuffers(1, &g_buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, g_buffer);

	glGenTextures(3, g_buffer_tex);
	glBindTexture(GL_TEXTURE_2D, g_buffer_tex[0]);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32UI, 2048, 2048);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, g_buffer_tex[1]);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, 2048, 2048);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, g_buffer_tex[2]);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, 2048, 2048);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, g_buffer_tex[0], 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, g_buffer_tex[1], 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, g_buffer_tex[2], 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


}

void pn::RenderSystem::shutdown() {

}

void pn::RenderSystem::run() {
	pn::MatrixStack matrixStack;
	
	DrawCallContainer drawCalls{};

	for (auto root_children : m_state->getRootEntity().getChildrenID()) {
		buildDrawCalls(root_children, matrixStack, drawCalls);
	}

//	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
//	glClear(GL_COLOR_BUFFER_BIT);
//	glClear(GL_DEPTH_BUFFER_BIT);

	static const GLenum draw_buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	static const GLuint uint_zeros[] = { 0, 0, 0, 0 };
	static const GLfloat float_zeros[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	static const GLfloat float_ones[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	glBindFramebuffer(GL_FRAMEBUFFER, g_buffer);
	glDrawBuffers(2, draw_buffers);
	glClearBufferuiv(GL_COLOR, 0, uint_zeros);
	glClearBufferuiv(GL_COLOR, 1, uint_zeros);
	glClearBufferfv(GL_DEPTH, 0, float_ones);
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	renderDrawCalls(drawCalls);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDrawBuffer(GL_BACK);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, g_buffer_tex[0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, g_buffer_tex[1]);

	deferred_program = m_state->m_resources.getMaterial("deferred_finish.sp").getGLProgramObject();
	glUseProgram(deferred_program);

	glDisable(GL_DEPTH_TEST);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

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
		auto& material = m_state->m_resources.getMaterial(renderComponent->getMaterialFilename().getHash());
		auto& mesh = m_state->m_resources.getMesh(renderComponent->getMeshFilename());

		// world position of entity
		mat4 worldTransform = m_state->getEntityWorldTransform(current_entity_ID, [&]()->mat4 { return matrixStack.multiply(); });

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
//		std::cout << current_mesh_hash << ", " << current_material_hash << std::endl;
		glDrawArraysInstanced(GL_TRIANGLES, 0, current_mesh.getNumVertices(), 1);

		current_material.postRender();
	}
}