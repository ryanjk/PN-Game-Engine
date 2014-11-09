#include "PN/ConcreteStates/InitialState.h"

#include "PN/ECS/Component/RenderComponent.h"
#include "PN/ECS/Component/TransformComponent.h"


#include "PN/Input/InputManager.h"
#include "PN/Input/FirstPersonListener.h"

#include "PN/Render/RenderFactory.h"
#include "PN/Render/Light.h"
#include "PN/Render/DrawCall.h"

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
	const HashValue defaultShaderProgram = pn::PString("default.sp").getHash();

	// set up renderable for each entity that needs one
	for (auto& entity : m_entities) {
		bool hasRenderComponent = (entity.getKey() & pn::ComponentType::RENDER) == pn::ComponentType::RENDER;
		if (hasRenderComponent) {
			auto& renderComponent = std::dynamic_pointer_cast<pn::RenderComponent>(entity.getComponent(pn::ComponentType::RENDER));
			auto shaderProgramName = renderComponent->getShaderProgram();
		
		//	if (shaderProgramName.getHash() == defaultShaderProgram) {
			Renderable entity_renderable;

			entity_renderable.mesh = renderComponent->getMesh().getHash();
			entity_renderable.image_diffuse = renderComponent->getDiffuse().getHash();
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

			glGenBuffers(1, &entity_renderable.VBO_vt);
			glBindBuffer(GL_ARRAY_BUFFER, entity_renderable.VBO_vt);
			glBufferData(GL_ARRAY_BUFFER, m_resources.getMesh(
				entity_renderable.mesh).getTexes().size() * sizeof(GLfloat),
				&m_resources.getMesh(entity_renderable.mesh).getTexes()[0],
				GL_STATIC_DRAW);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);

			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glGenTextures(1, &entity_renderable.TBO_diffuse);
			glBindTexture(GL_TEXTURE_2D, entity_renderable.TBO_diffuse);
				const pn::Image& img = m_resources.getImage(entity_renderable.image_diffuse);
				glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, img.getWidth(), img.getHeight());
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, img.getWidth(), img.getHeight(), GL_RGBA, GL_UNSIGNED_BYTE, &img.getPixels()[0]);
			glBindTexture(GL_TEXTURE_2D, 0);

			glGenSamplers(1, &entity_renderable.sampler_diffuse);
			glSamplerParameteri(entity_renderable.sampler_diffuse, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glSamplerParameteri(entity_renderable.sampler_diffuse, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glSamplerParameteri(entity_renderable.sampler_diffuse, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

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
		const auto& renderComponent = std::dynamic_pointer_cast<pn::RenderComponent>(current_entity.getComponent(pn::ComponentType::RENDER));
		const auto& program = m_resources.getShaderProgram(renderComponent->getShaderProgram());

		auto& entityRenderable = m_renderables[current_entity_ID];

		// world position of entity
		mat4 worldTransform(matrixStack.multiply());

		// amount of vertices to render
		unsigned int num_vertices = m_resources.getMesh(entityRenderable.mesh).getVertices().size();

		// put draw call into container
		pn::DrawCall drawCall({ worldTransform, entityRenderable, program, num_vertices, renderComponent });
		drawCalls.insert({ program.getMaterialID(), drawCall });
	}

	// Continue visiting the entity's children
	for (auto childID : current_entity.getChildrenID()) {
		buildDrawCalls(childID, matrixStack, drawCalls);
	}

	matrixStack.pop();
}

void pn::InitialState::renderDrawCalls(DrawCallContainer& drawCalls) {
	static const auto defaultShader = pn::PString("default.sp").getHash();
	static const auto static_light_shader = pn::PString("static_light.sp").getHash();

	// Camera position
	const mat4& camera_world_transform = getEntityWorldTransform(m_activeCamera->getID());

	// Get view transform from camera
	const auto view_transform(glm::inverse(camera_world_transform));

	// Get projection transform from screen settings
	const auto proj_transform(glm::perspective(glm::radians(50.0f),
		(float)settings.getWindowWidth() / (float)settings.getWindowHeight(),
		0.1f, 1000.0f));

	HashValue last_program_rendered = 0;
	for (const auto& drawCallIter : drawCalls) {
		const auto& drawCall = drawCallIter.second;
		auto current_program = drawCall.shader_program.getShaderProgramFilename().getHash();
		if (current_program == defaultShader) {

			// If the current program is not the same as the last one used, set new global uniforms
			if (!(current_program == last_program_rendered)) {

				glUseProgram(drawCall.shader_program.getGLProgramObject());

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

					drawCall.shader_program.setUniform("lightUni[" + std::to_string(num_lights_set) + "]", entity_light);
					num_lights_set++;
				}

				drawCall.shader_program.setUniform("num_lights", num_lights_set);

				const vec3& camera_position(camera_world_transform[3].xyz);
				drawCall.shader_program.setUniform("cameraPosition", camera_position);
				drawCall.shader_program.setUniform("view", view_transform);
				drawCall.shader_program.setUniform("proj", proj_transform);
			}

			drawCall.shader_program.setUniform("ambient", drawCall.renderComponent->getAmbient());
			drawCall.shader_program.setUniform("specular", drawCall.renderComponent->getSpecular());
			drawCall.shader_program.setUniform("gloss", drawCall.renderComponent->getGloss());

			auto world_transform_index = glGetUniformLocation(drawCall.shader_program.getGLProgramObject(), "world");

			glBindVertexArray(drawCall.gl_objects.VAO);

			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);

			glUniformMatrix4fv(world_transform_index, 1, GL_FALSE, glm::value_ptr(drawCall.world_transform));

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, drawCall.gl_objects.TBO_diffuse);
			glBindSampler(0, drawCall.gl_objects.sampler_diffuse);

			glDrawArraysInstanced(GL_TRIANGLES, 0, drawCall.num_vertices, 1);

			last_program_rendered = current_program;

		}

		else if (drawCall.shader_program.getShaderProgramFilename().getHash() == static_light_shader) {

			if (!(current_program == last_program_rendered)) {

				glUseProgram(drawCall.shader_program.getGLProgramObject());

				const vec3& camera_position(camera_world_transform[3].xyz);
				drawCall.shader_program.setUniform("cameraPosition", camera_position);
				drawCall.shader_program.setUniform("view", view_transform);
				drawCall.shader_program.setUniform("proj", proj_transform);

			}

			auto world_transform_index = glGetUniformLocation(drawCall.shader_program.getGLProgramObject(), "world");

			glBindVertexArray(drawCall.gl_objects.VAO);

			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);

			glUniformMatrix4fv(world_transform_index, 1, GL_FALSE, glm::value_ptr(drawCall.world_transform));

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, drawCall.gl_objects.TBO_diffuse);
			glBindSampler(0, drawCall.gl_objects.sampler_diffuse);

			glDrawArraysInstanced(GL_TRIANGLES, 0, drawCall.num_vertices, 1);

			last_program_rendered = current_program;

		}
	}
}