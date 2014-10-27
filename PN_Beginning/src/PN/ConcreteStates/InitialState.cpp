#include "PN/ConcreteStates/InitialState.h"

#include "PN/ECS/Component/RenderComponent.h"
#include "PN/ECS/Component/TransformComponent.h"

#include "PN/Input/InputManager.h"
#include "PN/Input/FirstPersonListener.h"

#include "PN/Render/RenderFactory.h"

#include "PN/Util/ShaderLoader.h"
#include "PN/Util/Math.h"

#include <iostream>
#include <algorithm>

static auto& resources = pn::ResourceManager::g_resourceManager;

static void setUpRenderable(pn::Renderable& m_r) {
	m_r.mesh = pn::PString("monkey.obj").getHash();

	m_r.SHADER_program = pn::ResourceManager::g_resourceManager.getShaderProgram("default.sp").getGLProgramObject();

	glGenVertexArrays(1, &m_r.VAO);
	glBindVertexArray(m_r.VAO);

	glGenBuffers(1, &m_r.VBO_v);
	glBindBuffer(GL_ARRAY_BUFFER, m_r.VBO_v);
	glBufferData(GL_ARRAY_BUFFER, resources.getMesh(m_r.mesh).getVertices().size() * sizeof(GLfloat), &resources.getMesh(m_r.mesh).getVertices()[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glGenBuffers(1, &m_r.VBO_vn);
	glBindBuffer(GL_ARRAY_BUFFER, m_r.VBO_vn);
	glBufferData(GL_ARRAY_BUFFER, resources.getMesh(m_r.mesh).getNormals().size() * sizeof(GLfloat), &resources.getMesh(m_r.mesh).getNormals()[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	/*
	glGenBuffers(1, &m_ss_coord);
	glBindBuffer(GL_ARRAY_BUFFER, m_ss_coord);
	glBufferData(GL_ARRAY_BUFFER, m_ss_model.getTexes().size() * sizeof(GLfloat), &m_ss_model.getTexes()[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	glGenTextures(1, &m_tex_ss);
	glBindTexture(GL_TEXTURE_2D, m_tex_ss);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, m_splashscreen.getWidth(), m_splashscreen.getHeight());
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_splashscreen.getWidth(), m_splashscreen.getHeight(), GL_RGBA, GL_UNSIGNED_BYTE, &m_splashscreen.getPixels()[0]);
	*/
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//	glBindTexture(GL_TEXTURE_2D, 0);
}

static void setUpFloor(pn::Renderable& m_r) {
	m_r.mesh = pn::PString("floor.obj").getHash();

//	m_r.SHADER_v = resources.getVertexShader("monkey.vglsl");
//	m_r.SHADER_f = resources.getFragmentShader("monkey.fglsl");
//	m_r.SHADER_program = pn::ShaderLoader::loadProgram(m_r.SHADER_v, m_r.SHADER_f);

	glGenVertexArrays(1, &m_r.VAO);
	glBindVertexArray(m_r.VAO);

	glGenBuffers(1, &m_r.VBO_v);
	glBindBuffer(GL_ARRAY_BUFFER, m_r.VBO_v);
	glBufferData(GL_ARRAY_BUFFER, resources.getMesh(m_r.mesh).getVertices().size() * sizeof(GLfloat), &resources.getMesh(m_r.mesh).getVertices()[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glGenBuffers(1, &m_r.VBO_vn);
	glBindBuffer(GL_ARRAY_BUFFER, m_r.VBO_vn);
	glBufferData(GL_ARRAY_BUFFER, resources.getMesh(m_r.mesh).getNormals().size() * sizeof(GLfloat), &resources.getMesh(m_r.mesh).getNormals()[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

pn::InitialState::InitialState(pn::PString stateFilename) : pn::GameState(stateFilename), m_camera() {

}

void pn::InitialState::update(double dt) {
	static double time = 0.0;
	time += dt;
	
	auto monkeyItr = std::find_if(m_entities.begin(), m_entities.end(), [](decltype(m_entities[0]) e) -> bool {return e->getID() == pn::PString("monkey").getHash(); });
	auto& monkey_transform = std::dynamic_pointer_cast<pn::TransformComponent>((*monkeyItr)->getComponent(pn::ComponentType::TRANSFORM));
	
	monkey_transform->rotate({ 0.0f, glm::radians((float)glm::cos(time)), 0.0f });
}

void pn::InitialState::startUpAssist() {
	static const HashValue defaultShaderProgram = pn::PString("default.sp").getHash();

	// set up renderable for each entity that needs one
	for (auto& entity : m_entities) {
		bool hasKey = (entity->getKey() & pn::ComponentType::RENDER) == pn::ComponentType::RENDER;
		if (!hasKey) continue;

		auto& renderComponent = std::dynamic_pointer_cast<pn::RenderComponent>(entity->getComponent(pn::ComponentType::RENDER));
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

	auto handler = pn::InputManager::g_inputManager.getInputHandler();
	handler->addListener(std::make_shared<pn::FirstPersonListener>(&m_camera));

	m_light_pos = vec3(0.0f, 0.0f, 2.0f);
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
	
	// Camera position
	auto camera_position = m_camera.getPosition();
	program.setUniform("cameraPosition", camera_position);

	// Get view transform from camera
	auto view_transform = m_camera.getView();
	program.setUniform("view", view_transform);

	// Get projection transform from screen settings
	auto proj_transform = glm::perspective(glm::radians(50.0f), 800.0f / 600.f, 0.1f, 100.0f);
	program.setUniform("proj", proj_transform);

	auto world_transform_index = glGetUniformLocation(program.getGLProgramObject(), "world");
	
	for (auto entity : m_entities) {
		bool hasKey = (entity->getKey() & pn::ComponentType::RENDER) == pn::ComponentType::RENDER;
		if (!hasKey) continue;


		auto& transformComponent = std::dynamic_pointer_cast<pn::TransformComponent>(entity->getComponent(pn::ComponentType::TRANSFORM));
		assert(transformComponent != nullptr);

		auto& renderComponent = std::dynamic_pointer_cast<pn::RenderComponent>(entity->getComponent(pn::ComponentType::RENDER));
		program.setUniform("ambient", renderComponent->getAmbient());
		program.setUniform("diffuse", renderComponent->getDiffuse());
		program.setUniform("specular", renderComponent->getSpecular());
		program.setUniform("gloss", renderComponent->getGloss());

		auto& entityRenderable = m_renderables[entity->getID()];
		glBindVertexArray(entityRenderable.VAO);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glUniformMatrix4fv(world_transform_index, 1, GL_FALSE, glm::value_ptr(transformComponent->getTransformMatrix()));
		glDrawArraysInstanced(GL_TRIANGLES, 0, resources.getMesh(entityRenderable.mesh).getVertices().size(), 1);
	}

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glBindVertexArray(0);
	glUseProgram(0);
}