#include "PN/ConcreteStates/InitialState.h"

#include "PN/ECS/Component/RenderComponent.h"
#include "PN/ECS/Component/TransformComponent.h"

#include "PN/Input/InputManager.h"
#include "PN/Input/FirstPersonListener.h"

#include "PN/Render/RenderFactory.h"
#include "PN/Render/Light.h"
#include "PN/Render/DrawCall.h"
#include "PN/Render/Material.h"

#include "PN/Util/Math.h"

#include "PN/Settings/SettingsManager.h"

#include <iostream>
#include <algorithm>

static auto& settings = pn::SettingsManager::g_SettingsManager;

pn::InitialState::InitialState(const pn::PString& stateFilename) : pn::GameState(stateFilename), m_activeCamera() {

}

void pn::InitialState::update(double dt) {
	static double time = 0.0;
	time += dt;
	
//	auto monkey = getEntity("monkey");
//	auto& monkey_transform = std::dynamic_pointer_cast<pn::TransformComponent>(monkey.getComponent(pn::ComponentType::TRANSFORM));
//	monkey_transform->rotateParent(glm::radians(vec3(0.0f, 5.5f * dt, 0.0f)));

}

void pn::InitialState::startUpAssist() {
	// set up renderable for each entity that needs one
	for (auto& entity : m_entities) {
		bool hasRenderComponent = (entity.getKey() & pn::ComponentType::RENDER) == pn::ComponentType::RENDER;
		if (hasRenderComponent) {
			auto& renderComponent = std::dynamic_pointer_cast<pn::RenderComponent>(entity.getComponent(pn::ComponentType::RENDER));
			auto& entity_material = m_resources.getMaterial(renderComponent->getMaterialFilename());
			
			Renderable entity_renderable;

			entity_material.setUpRenderable(entity_renderable, *renderComponent, m_resources);

			m_renderables.insert({ entity.getID(), entity_renderable });
		}

		bool hasLightComponent = (entity.getKey() & pn::ComponentType::LIGHT) == pn::ComponentType::LIGHT;
		if (hasLightComponent) {
			m_lights.push_back(entity.getID());
		}
	}

	auto& camera = getEntity("camera");
	m_activeCamera = &camera;

	auto& player = getEntity("player");
	auto playerBody = std::dynamic_pointer_cast<pn::TransformComponent>(player.getComponent(pn::ComponentType::TRANSFORM));
	auto handler = pn::InputManager::g_inputManager.getInputHandler();
	handler->addListener(std::make_shared<pn::FirstPersonListener>(playerBody));

	mm::setCursor(false);
	
}

void pn::InitialState::render() {

	DrawCallContainer drawCalls;

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	pn::MatrixStack matrixStack;

	for (auto root_children : getRootEntity().getChildrenID()) {
		buildDrawCalls(root_children, matrixStack, drawCalls);
	}

	renderDrawCalls(drawCalls);

	glUseProgram(0);

}

void pn::InitialState::buildDrawCalls(EntityID current_entity_ID, pn::MatrixStack& matrixStack, DrawCallContainer& drawCalls) {
	const auto& current_entity = getEntity(current_entity_ID);

	// If the entity does not have a position in space, then its children do not, so stop travelling down this branch
	bool hasTransform = (current_entity.getKey() & (pn::ComponentType::TRANSFORM)) == (pn::ComponentType::TRANSFORM);
	if (!hasTransform) {
		return;
	}

	// Push the entity's matrix onto the matrix stack
	const auto& transformComponent = std::dynamic_pointer_cast<pn::TransformComponent>(current_entity.getComponent(pn::ComponentType::TRANSFORM));
	matrixStack.push(transformComponent->getTransformMatrix());

	// If the entity is renderable, create a draw call and add it to the draw call container
	bool hasRender = (current_entity.getKey() & (pn::ComponentType::RENDER)) == (pn::ComponentType::RENDER);
	if (hasRender) {
		auto& renderComponent = std::dynamic_pointer_cast<pn::RenderComponent>(current_entity.getComponent(pn::ComponentType::RENDER));
		auto& material = m_resources.getMaterial(renderComponent->getMaterialFilename());

		auto& entityRenderable = m_renderables[current_entity_ID];

		// world position of entity
		mat4 worldTransform(matrixStack.multiply());

		// amount of vertices to render
		unsigned int num_vertices = m_resources.getMesh(entityRenderable.mesh).getVertices().size();

		// put draw call into container
		pn::DrawCall drawCall({ worldTransform, entityRenderable, material, num_vertices, renderComponent });
		drawCalls.insert({ material.getMaterialID(), drawCall });
	}

	// Continue visiting the entity's children
	for (auto childID : current_entity.getChildrenID()) {
		buildDrawCalls(childID, matrixStack, drawCalls);
	}

	matrixStack.pop();
}

void pn::InitialState::renderDrawCalls(DrawCallContainer& drawCalls) {
	static const auto dynamic_light_material = pn::PString("dynamic_light.sp").getHash();
	static const auto static_light_material = pn::PString("static_light.sp").getHash();

	// Camera position
	const mat4& camera_world_transform = getEntityWorldTransform(m_activeCamera->getID());

	// Get view transform from camera
	const auto view_transform(glm::inverse(camera_world_transform));

	// Get projection transform from screen settings
	const auto proj_transform(glm::perspective(glm::radians(50.0f),
		(float)settings.getWindowWidth() / (float)settings.getWindowHeight(),
		0.1f, 1000.0f));

	HashValue last_material_rendered = 0;
	// render each draw call -- it goes in order of material ID because the map is sorted by this
	for (auto& drawCallIter : drawCalls) {
		auto& drawCall = drawCallIter.second;
		auto current_material_hash = drawCall.material.getMaterialFilename().getHash();
		auto& current_material = drawCall.material;
		
		// If the current program is not the same as the last one used, set new global uniforms
		if (!(current_material_hash == last_material_rendered)) {
			current_material.setGlobalUniforms(this, m_lights, camera_world_transform, view_transform, proj_transform);
			last_material_rendered = current_material_hash;
		}
		
		current_material.setInstanceUniforms(drawCall);
		glDrawArraysInstanced(GL_TRIANGLES, 0, drawCall.num_vertices, 1);

	}
}