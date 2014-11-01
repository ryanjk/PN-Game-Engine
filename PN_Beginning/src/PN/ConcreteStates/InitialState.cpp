#include "PN/ConcreteStates/InitialState.h"

#include "PN/ECS/Component/RenderComponent.h"
#include "PN/ECS/Component/TransformComponent.h"

#include "PN/Input/InputManager.h"
#include "PN/Input/FirstPersonListener.h"

#include "PN/Render/RenderFactory.h"

#include "PN/Util/ShaderLoader.h"
#include "PN/Util/Math.h"

#include "PN/Settings/SettingsManager.h"

#include <iostream>
#include <algorithm>

static auto& resources = pn::ResourceManager::g_resourceManager;
static auto& settings = pn::SettingsManager::g_SettingsManager;

pn::InitialState::InitialState(pn::PString stateFilename) : pn::GameState(stateFilename), m_activeCamera() {

}

void pn::InitialState::update(double dt) {
	static double time = 0.0;
	time += dt;
	
//	auto monkey = getEntity("child_monkey");
//	auto& monkey_transform = std::dynamic_pointer_cast<pn::TransformComponent>(monkey->getComponent(pn::ComponentType::TRANSFORM));
//	monkey_transform->rotateParent(glm::radians(vec3(0.0f, 5.0f, 0.0f)));

//	auto child_child_monkey = getEntity("child_child_monkey");
//	auto& child_child_monkey_transform = std::dynamic_pointer_cast<pn::TransformComponent>(child_child_monkey->getComponent(pn::ComponentType::TRANSFORM));


/*	auto dragon = getEntity("dragon2");
	auto& dragon_transform = std::dynamic_pointer_cast<pn::TransformComponent>(dragon->getComponent(pn::ComponentType::TRANSFORM));

	dragon_transform->translateLocal({ 0.01f, 0.0f, 0.0f });

	auto dragon2 = getEntity("dragon");
	auto& dragon2_transform = std::dynamic_pointer_cast<pn::TransformComponent>(dragon2->getComponent(pn::ComponentType::TRANSFORM));

	dragon2_transform->translateWorld({ 0.01f, 0.0f, 0.0f }); */

//	m_light_pos = vec3(0.0f, 3 * glm::sin(time), 3 * glm::cos(time));
}

void pn::InitialState::startUpAssist() {
	const HashValue defaultShaderProgram = pn::PString("default.sp").getHash();

	// set up renderable for each entity that needs one
	for (auto& entity : m_entities) {
		bool hasKey = (entity->getKey() & pn::ComponentType::RENDER) == pn::ComponentType::RENDER;
		if (!hasKey) continue;

		auto& renderComponent = std::dynamic_pointer_cast<pn::RenderComponent>(entity->getComponent(pn::ComponentType::RENDER));
	//	auto& renderComponent = entity->getComponent<pn::RenderComponent>(pn::ComponentType::RENDER);
		auto shaderProgramName = renderComponent->getShaderProgram();
		
		if (shaderProgramName.getHash() == defaultShaderProgram) {
			Renderable entity_renderable;

			// shader program sets up renderable its own way

			entity_renderable.mesh = renderComponent->getMesh().getHash();
			entity_renderable.SHADER_program = resources.getShaderProgram(renderComponent->getShaderProgram()).getGLProgramObject();

			glGenVertexArrays(1, &entity_renderable.VAO);
			glBindVertexArray(entity_renderable.VAO);

			glGenBuffers(1, &entity_renderable.VBO_v);
			glBindBuffer(GL_ARRAY_BUFFER, entity_renderable.VBO_v);
			glBufferData(GL_ARRAY_BUFFER, resources.getMesh(
				entity_renderable.mesh).getVertices().size() * sizeof(GLfloat), 
				&resources.getMesh(entity_renderable.mesh).getVertices()[0], 
				GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

			glGenBuffers(1, &entity_renderable.VBO_vn);
			glBindBuffer(GL_ARRAY_BUFFER, entity_renderable.VBO_vn);
			glBufferData(GL_ARRAY_BUFFER, resources.getMesh(
				entity_renderable.mesh).getNormals().size() * sizeof(GLfloat), 
				&resources.getMesh(entity_renderable.mesh).getNormals()[0], 
				GL_STATIC_DRAW);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			m_renderables.insert({ entity->getID(), entity_renderable });
		}

	}

	auto camera = getEntity("camera");
	m_activeCamera = camera;

	auto player = getEntity("player");
	auto playerBody = std::dynamic_pointer_cast<pn::TransformComponent>(player->getComponent(pn::ComponentType::TRANSFORM));

	auto handler = pn::InputManager::g_inputManager.getInputHandler();

	handler->addListener(std::make_shared<pn::FirstPersonListener>(playerBody));

	m_light_pos = vec3(0.0f, 100.0f, 0.0f);
	m_light2_pos = vec3(0.0f, 3.0f, 0.0f);
}

void pn::InitialState::render() {

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
	
	auto& program = pn::ResourceManager::g_resourceManager.getShaderProgram("default.sp");

	glUseProgram(program.getGLProgramObject());

	// Light position
	program.setUniform("lightPosition", m_light_pos);
	
//	auto cameraParent = getEntity(m_activeCamera->getOwner());
//	auto playerBody = std::dynamic_pointer_cast<pn::TransformComponent>(cameraParent->getComponent(pn::ComponentType::TRANSFORM));
//	auto playerBody = cameraParent->getComponent<pn::TransformComponent>(pn::ComponentType::TRANSFORM);
	
	// Camera position
	mat4 camera_world_transform = getEntityWorldTransform(m_activeCamera->getID());

	vec3 camera_position = camera_world_transform[3].xyz;
	program.setUniform("cameraPosition", camera_position);

	// Get view transform from camera
	auto view_transform = glm::inverse(camera_world_transform);
	program.setUniform("view", view_transform);

	// Get projection transform from screen settings
	auto proj_transform = glm::perspective(glm::radians(50.0f), 
		(float) settings.getWindowWidth() / (float )settings.getWindowHeight(), 
		0.1f, 1000.0f);
	program.setUniform("proj", proj_transform);

	auto world_transform_index = glGetUniformLocation(program.getGLProgramObject(), "world");
	
	for (auto entity : m_entities) {
		bool hasKey = (entity->getKey() & (pn::ComponentType::RENDER | pn::ComponentType::TRANSFORM)) == 
			(pn::ComponentType::RENDER | pn::ComponentType::TRANSFORM);
		if (!hasKey) continue;

		auto& transformComponent = std::dynamic_pointer_cast<pn::TransformComponent>(entity->getComponent(pn::ComponentType::TRANSFORM));
		auto& renderComponent = std::dynamic_pointer_cast<pn::RenderComponent>(entity->getComponent(pn::ComponentType::RENDER));

		program.setUniform("ambient", renderComponent->getAmbient());
		program.setUniform("diffuse", renderComponent->getDiffuse());
		program.setUniform("specular", renderComponent->getSpecular());
		program.setUniform("gloss", renderComponent->getGloss());

		auto& entityRenderable = m_renderables[entity->getID()];
		glBindVertexArray(entityRenderable.VAO);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glUniformMatrix4fv(world_transform_index, 1, GL_FALSE, glm::value_ptr(getEntityWorldTransform(entity->getID())));
		glDrawArraysInstanced(GL_TRIANGLES, 0, resources.getMesh(entityRenderable.mesh).getVertices().size(), 1);
	}
	
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glBindVertexArray(0);
	glUseProgram(0);
}