#include "PN/ConcreteStates/InitialState.h"

#include "PN/Input/InputManager.h"
#include "PN/Input/FirstPersonListener.h"

#include "PN/Render/RenderFactory.h"

#include "PN/Util/ShaderLoader.h"
#include "PN/Util/Math.h"

#include <iostream>

static auto& resources = pn::ResourceManager::g_resourceManager;

static void setUpRenderable(pn::Renderable& m_r) {
	m_r.mesh = pn::PString("monkey.obj").getHash();

	m_r.SHADER_v = resources.getVertexShader("monkey.vglsl");
	m_r.SHADER_f = resources.getFragmentShader("monkey.fglsl");
	m_r.SHADER_program = pn::ShaderLoader::loadProgram(m_r.SHADER_v, m_r.SHADER_f);

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
	
	m_monkey_world_transform = glm::rotate(mat4(1.0f), glm::radians((float)glm::cos(time) * 50), vec3(0.0f, 1.0f, 0.0f));
//	m_light_pos = vec3(4 * glm::cos(2 * time), 0.0f, glm::sin(2 * time) * 4);
}

void pn::InitialState::startUpAssist() {
	setUpRenderable(m_r);
	setUpFloor(m_floor_r);

	auto handler = pn::InputManager::g_inputManager.getInputHandler();
	handler->addListener(std::make_shared<pn::FirstPersonListener>(&m_camera));

	m_monkey_world_transform = mat4();
	m_floor_world_transform = mat4();

	m_light_pos = vec3(0.0f, 0.0f, 2.0f);
	m_light2_pos = vec3(0.0f, 3.0f, 0.0f);
}

void pn::InitialState::render() {

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
	
	glUseProgram(m_r.SHADER_program);

	// Light position
	auto light_position_index = glGetUniformLocation(m_r.SHADER_program, "lightPosition");
	glUniform3fv(light_position_index, 1, glm::value_ptr(m_light_pos));

	// Camera position
	auto view_matrix = m_camera.getView();
	auto camera_position = m_camera.getPosition();
	auto camera_position_index = glGetUniformLocation(m_r.SHADER_program, "cameraPosition");
	glUniform3fv(camera_position_index, 1, glm::value_ptr(camera_position));

	// Get view transform from camera
	auto view_transform = view_matrix;
	auto view_transform_index = glGetUniformLocation(m_r.SHADER_program, "view");
	glUniformMatrix4fv(view_transform_index, 1, GL_FALSE, glm::value_ptr(view_transform));

	// Get projection transform from screen settings
	auto proj_transform = glm::perspective(glm::radians(50.0f), 800.0f / 600.f, 0.1f, 100.0f);
	auto proj_transform_index = glGetUniformLocation(m_r.SHADER_program, "proj");
	glUniformMatrix4fv(proj_transform_index, 1, GL_FALSE, glm::value_ptr(proj_transform));

	// Get world transform for monkey
	auto world_transform_index = glGetUniformLocation(m_r.SHADER_program, "world");
	
	// Draw monkey
	glBindVertexArray(m_r.VAO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glUniformMatrix4fv(world_transform_index, 1, GL_FALSE, glm::value_ptr(m_monkey_world_transform));
	glDrawArraysInstanced(GL_TRIANGLES, 0, resources.getMesh(m_r.mesh).getVertices().size(), 1);

	// Draw floor
	glBindVertexArray(m_floor_r.VAO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glUniformMatrix4fv(world_transform_index, 1, GL_FALSE, glm::value_ptr(m_floor_world_transform));
	glDrawArraysInstanced(GL_TRIANGLES, 0, resources.getMesh(m_floor_r.mesh).getVertices().size(), 1);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glBindVertexArray(0);
	glUseProgram(0);
}