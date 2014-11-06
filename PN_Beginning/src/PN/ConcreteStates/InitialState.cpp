#include "PN/ConcreteStates/InitialState.h"

#include "PN/ECS/Component/RenderComponent.h"
#include "PN/ECS/Component/TransformComponent.h"
#include "PN/ECS/Component/LightComponent.h"

#include "PN/Input/InputManager.h"
#include "PN/Input/FirstPersonListener.h"

#include "PN/Render/RenderFactory.h"
#include "PN/Render/Light.h"

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
	
	auto monkey = getEntity("monkey");
	auto& monkey_transform = std::dynamic_pointer_cast<pn::TransformComponent>(monkey.getComponent(pn::ComponentType::TRANSFORM));
	monkey_transform->rotateParent(glm::radians(vec3(0.0f, 5.5f * dt, 0.0f)));

}

void pn::InitialState::startUpAssist() {
	const HashValue defaultShaderProgram = pn::PString("default.sp").getHash();

	// set up renderable for each entity that needs one
	for (auto& entity : m_entities) {
		bool hasRenderComponent = (entity.getKey() & pn::ComponentType::RENDER) == pn::ComponentType::RENDER;
		if (hasRenderComponent) {
			auto& renderComponent = std::dynamic_pointer_cast<pn::RenderComponent>(entity.getComponent(pn::ComponentType::RENDER));
			auto shaderProgramName = renderComponent->getShaderProgram();
		
			if (shaderProgramName.getHash() == defaultShaderProgram) {
				Renderable entity_renderable;

				// shader program sets up renderable its own way

				entity_renderable.mesh = renderComponent->getMesh().getHash();
				entity_renderable.SHADER_program = m_resources.getShaderProgram(renderComponent->getShaderProgram()).getGLProgramObject();

				glGenVertexArrays(1, &entity_renderable.VAO);
				glBindVertexArray(entity_renderable.VAO);

				glGenBuffers(1, &entity_renderable.VBO_v);
				glBindBuffer(GL_ARRAY_BUFFER, entity_renderable.VBO_v);
				glBufferData(GL_ARRAY_BUFFER, m_resources.getMesh(
					entity_renderable.mesh).getVertices().size() * sizeof(GLfloat), 
					&m_resources.getMesh(entity_renderable.mesh).getVertices()[0], 
					GL_STATIC_DRAW);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

				glGenBuffers(1, &entity_renderable.VBO_vn);
				glBindBuffer(GL_ARRAY_BUFFER, entity_renderable.VBO_vn);
				glBufferData(GL_ARRAY_BUFFER, m_resources.getMesh(
					entity_renderable.mesh).getNormals().size() * sizeof(GLfloat), 
					&m_resources.getMesh(entity_renderable.mesh).getNormals()[0], 
					GL_STATIC_DRAW);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

				glBindVertexArray(0);
				glBindBuffer(GL_ARRAY_BUFFER, 0);

				m_renderables.insert({ entity.getID(), entity_renderable });
			}	
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
	
}

void pn::InitialState::render() {

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
	
	auto& program = m_resources.getShaderProgram("default.sp");

	glUseProgram(program.getGLProgramObject());

	int num_lights_set = 0;
	for (auto entityID : m_lights) {
		auto& entity = getEntity(entityID);
		auto& light_component = std::dynamic_pointer_cast<pn::LightComponent>(entity.getComponent(pn::ComponentType::LIGHT));

		mat4 transform = getEntityWorldTransform(entityID);

		Light entity_light;
		entity_light.position = transform[3].xyz;
		entity_light.direction = -vec3(transform[2].xyz);
		entity_light.colour = light_component->getColour();
		entity_light.innerRadians = light_component->getInnerRadians();
		entity_light.outerRadians = light_component->getOuterRadians();
		entity_light.intensity = light_component->getIntensity();
		entity_light.maxRadius = light_component->getMaxRadius();
		entity_light.type = light_component->getLightType();

		program.setUniform("lightUni[" + std::to_string(num_lights_set) + "]", entity_light);
		num_lights_set++;
	}

	program.setUniform("num_lights", num_lights_set);
	
	// Camera position
	const mat4& camera_world_transform = getEntityWorldTransform(m_activeCamera->getID());

	const vec3& camera_position = camera_world_transform[3].xyz;
	program.setUniform("cameraPosition", camera_position);

	// Get view transform from camera
	auto view_transform = glm::inverse(camera_world_transform);
	program.setUniform("view", view_transform);

	// Get projection transform from screen settings
	auto proj_transform = glm::perspective(glm::radians(50.0f), 
		(float) settings.getWindowWidth() / (float )settings.getWindowHeight(), 
		0.1f, 1000.0f);
	program.setUniform("proj", proj_transform);

	pn::MatrixStack matrixStack;

	for (auto root_children : getRootEntity().getChildrenID()) {
		renderSceneGraph(root_children, matrixStack);
	}

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glBindVertexArray(0);
	glUseProgram(0);
}

void pn::InitialState::renderSceneGraph(EntityID start, pn::MatrixStack& matrixStack) {
	auto& current_entity = getEntity(start);

	bool hasTransform = (current_entity.getKey() & (pn::ComponentType::TRANSFORM)) == (pn::ComponentType::TRANSFORM);
	if (!hasTransform) {
		return;
	}

	auto& transformComponent = std::dynamic_pointer_cast<pn::TransformComponent>(current_entity.getComponent(pn::ComponentType::TRANSFORM));
	matrixStack.push(transformComponent->getTransformMatrix());

	bool hasRender = (current_entity.getKey() & (pn::ComponentType::RENDER)) == (pn::ComponentType::RENDER);
	if (hasRender) {
		auto& renderComponent = std::dynamic_pointer_cast<pn::RenderComponent>(current_entity.getComponent(pn::ComponentType::RENDER));
		auto& program = m_resources.getShaderProgram(renderComponent->getShaderProgram());

		program.setUniform("ambient", renderComponent->getAmbient());
		program.setUniform("diffuse", renderComponent->getDiffuse());
		program.setUniform("specular", renderComponent->getSpecular());
		program.setUniform("gloss", renderComponent->getGloss());

		auto world_transform_index = glGetUniformLocation(program.getGLProgramObject(), "world");

		auto& entityRenderable = m_renderables[start];
		glBindVertexArray(entityRenderable.VAO);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		mat4 worldTransform = matrixStack.multiply();
		glUniformMatrix4fv(world_transform_index, 1, GL_FALSE, glm::value_ptr(worldTransform));
		glDrawArraysInstanced(GL_TRIANGLES, 0, m_resources.getMesh(entityRenderable.mesh).getVertices().size(), 1);
	}

	for (auto childID : current_entity.getChildrenID()) {
		renderSceneGraph(childID, matrixStack);
	}

	matrixStack.pop();
}