#include "PN/Render/Material.h"
#include "PN/Render/Light.h"
#include "PN/Render/Renderable.h"
#include "PN/Render/DrawCall.h"

#include "PN/GameState/GameState.h"

#include "PN/Util/ResourceManager.h"
#include "PN/Util/Math.h"

#include "PN/ECS/Component/RenderComponent.h"
#include "PN/ECS/Component/LightComponent.h"

#include "PN/Window/WindowManager.h"

#include "PN/Settings/SettingsManager.h"

#include <iostream>

static auto& settings = pn::SettingsManager::g_SettingsManager;

pn::Material::Material(GLuint program, pn::PString shader_program_filename, 
	pn::PString vertex_shader, pn::PString fragment_shader, int materialID) 
	: m_program(program), m_vertex_shader_filename(vertex_shader), m_fragment_shader_filename(fragment_shader), 
	m_shader_program_filename(shader_program_filename), m_materialID(materialID) {}

GLuint pn::Material::getGLProgramObject() const {
	return m_program;
}

int pn::Material::getMaterialID() const {
	return m_materialID;
}

const pn::PString& pn::Material::getVertexShaderFilename() const {
	return m_vertex_shader_filename;
}

const pn::PString& pn::Material::getFragmentShaderFilename() const {
	return m_fragment_shader_filename;
}

const pn::PString& pn::Material::getMaterialFilename() const {
	return m_shader_program_filename;
}

template<>
void pn::Material::setUniform<int>(const std::string& uniform, int value) const {
	GLint uniform_location = getUniformLocation(uniform);
	glUniform1i(uniform_location, value);
}

template<>
void pn::Material::setUniform<float>(const std::string& uniform, float value) const {
	GLint uniform_location = getUniformLocation(uniform);
	glUniform1f(uniform_location, value);
}

template<>
void pn::Material::setUniform<float*>(const std::string& uniform, float* value) const {
	GLint uniform_location = getUniformLocation(uniform);
	glUniform1fv(uniform_location, 1, value);
}

template<>
void pn::Material::setUniform<vec3>(const std::string& uniform, vec3 value) const {
	GLint uniform_location = getUniformLocation(uniform);
	glUniform3fv(uniform_location, 1, glm::value_ptr(value));
}

template<>
void pn::Material::setUniform<vec4>(const std::string& uniform, vec4 value) const {
	GLint uniform_location = getUniformLocation(uniform);
	glUniform4fv(uniform_location, 1, glm::value_ptr(value));
}

template<>
void pn::Material::setUniform<mat4>(const std::string& uniform, mat4 value) const {
	GLint uniform_location = getUniformLocation(uniform);
	glUniformMatrix4fv(uniform_location, 1, GL_FALSE, glm::value_ptr(value));
}

template<>
void pn::Material::setUniform<pn::Light>(const std::string& uniform, pn::Light value) const {
	setUniform(uniform + ".position", value.position);
	setUniform(uniform + ".direction", value.direction);
	setUniform(uniform + ".type", value.type);
	setUniform(uniform + ".colour", value.colour);
	setUniform(uniform + ".intensity", value.intensity);
	setUniform(uniform + ".innerRadians", value.innerRadians);
	setUniform(uniform + ".outerRadians", value.outerRadians);
	setUniform(uniform + ".maxRadius", value.maxRadius);
}

GLint pn::Material::getUniformLocation(const std::string& uniform) const {
	GLint uniform_location = glGetUniformLocation(m_program, uniform.c_str());
	assert(uniform_location != -1);
	return uniform_location;
}

void pn::Material::setUpRenderable(pn::Renderable& renderable, const pn::RenderComponent& renderComponent, 
	pn::ResourceManager& resources) const {

	switch (m_materialID) {
	case MaterialID::DYNAMIC_LIGHT:
		renderable.mesh = renderComponent.getMeshFilename().getHash();
		renderable.image_diffuse = renderComponent.getDiffuse().getHash();

		glGenVertexArrays(1, &renderable.VAO);
		glBindVertexArray(renderable.VAO);

		glGenBuffers(1, &renderable.VBO_v);
		glBindBuffer(GL_ARRAY_BUFFER, renderable.VBO_v);
		glBufferData(GL_ARRAY_BUFFER, resources.getMesh(
			renderable.mesh).getVertices().size() * sizeof(GLfloat),
			&resources.getMesh(renderable.mesh).getVertices()[0],
			GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		glGenBuffers(1, &renderable.VBO_vn);
		glBindBuffer(GL_ARRAY_BUFFER, renderable.VBO_vn);
		glBufferData(GL_ARRAY_BUFFER, resources.getMesh(
			renderable.mesh).getNormals().size() * sizeof(GLfloat),
			&resources.getMesh(renderable.mesh).getNormals()[0],
			GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		glGenBuffers(1, &renderable.VBO_vt);
		glBindBuffer(GL_ARRAY_BUFFER, renderable.VBO_vt);
		glBufferData(GL_ARRAY_BUFFER, resources.getMesh(
			renderable.mesh).getTexes().size() * sizeof(GLfloat),
			&resources.getMesh(renderable.mesh).getTexes()[0],
			GL_STATIC_DRAW);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenTextures(1, &renderable.TBO_diffuse);
		glBindTexture(GL_TEXTURE_2D, renderable.TBO_diffuse);
		{
			const pn::Image& img = resources.getImage(renderable.image_diffuse);
			glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, img.getWidth(), img.getHeight());
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, img.getWidth(), img.getHeight(), GL_RGBA, GL_UNSIGNED_BYTE, &img.getPixels()[0]);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		glGenSamplers(1, &renderable.sampler_diffuse);
		glSamplerParameteri(renderable.sampler_diffuse, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glSamplerParameteri(renderable.sampler_diffuse, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glSamplerParameteri(renderable.sampler_diffuse, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		break;
	case MaterialID::STATIC_LIGHT:
	case MaterialID::SCREEN_OVERLAY:
		renderable.mesh = renderComponent.getMeshFilename().getHash();
		renderable.image_diffuse = renderComponent.getDiffuse().getHash();

		glGenVertexArrays(1, &renderable.VAO);
		glBindVertexArray(renderable.VAO);

		glGenBuffers(1, &renderable.VBO_v);
		glBindBuffer(GL_ARRAY_BUFFER, renderable.VBO_v);
		glBufferData(GL_ARRAY_BUFFER, resources.getMesh(
			renderable.mesh).getVertices().size() * sizeof(GLfloat),
			&resources.getMesh(renderable.mesh).getVertices()[0],
			GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		glGenBuffers(1, &renderable.VBO_vt);
		glBindBuffer(GL_ARRAY_BUFFER, renderable.VBO_vt);
		glBufferData(GL_ARRAY_BUFFER, resources.getMesh(
			renderable.mesh).getTexes().size() * sizeof(GLfloat),
			&resources.getMesh(renderable.mesh).getTexes()[0],
			GL_STATIC_DRAW);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenTextures(1, &renderable.TBO_diffuse);
		glBindTexture(GL_TEXTURE_2D, renderable.TBO_diffuse);
		{
			const pn::Image& img = resources.getImage(renderable.image_diffuse);
			glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, img.getWidth(), img.getHeight());
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, img.getWidth(), img.getHeight(), GL_RGBA, GL_UNSIGNED_BYTE, &img.getPixels()[0]);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		glGenSamplers(1, &renderable.sampler_diffuse);
		glSamplerParameteri(renderable.sampler_diffuse, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glSamplerParameteri(renderable.sampler_diffuse, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glSamplerParameteri(renderable.sampler_diffuse, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		break;
	default:
		assert(false && "Couldn't identify material to set up renderable for");
		break;
	}
}

void pn::Material::setGlobalUniforms(pn::GameState& gameState, const std::vector<EntityID>& lights,
	const mat4& camera, const mat4& view) const {
	switch (m_materialID) {

	case MaterialID::DYNAMIC_LIGHT:
	{
		{
			glUseProgram(getGLProgramObject());

			int num_lights_set = 0;
			for (auto entityID : lights) {
				auto& entity = gameState.getEntity(entityID);
				auto& light_component = std::dynamic_pointer_cast<pn::LightComponent>(entity.getComponent(pn::ComponentType::LIGHT));

				mat4 transform = gameState.getEntityWorldTransform(entityID);

				Light entity_light;
				entity_light.position = transform[3].xyz;
				entity_light.direction = -vec3(transform[2].xyz);
				entity_light.colour = light_component->getColour();
				entity_light.innerRadians = light_component->getInnerRadians();
				entity_light.outerRadians = light_component->getOuterRadians();
				entity_light.intensity = light_component->getIntensity();
				entity_light.maxRadius = light_component->getMaxRadius();
				entity_light.type = light_component->getLightType();

				setUniform("lightUni[" + std::to_string(num_lights_set) + "]", entity_light);
				num_lights_set++;
			}

			setUniform("num_lights", num_lights_set);
		}

		const vec3& camera_position(camera[3].xyz);
		setUniform("cameraPosition", camera_position);
		setUniform("view", view);
		setUniform("proj", settings.getProjectionMatrix());
	}
		break;

	case MaterialID::STATIC_LIGHT:
	{
		glUseProgram(getGLProgramObject());

		const vec3& camera_position(camera[3].xyz);
		setUniform("view", view);
		setUniform("proj", settings.getProjectionMatrix());
	}
		break;
	case MaterialID::SCREEN_OVERLAY:
		glUseProgram(getGLProgramObject());

		setUniform("proj", settings.getHUDMatrix());
	default:
		break;

	}
}

void pn::Material::setInstanceUniforms(pn::DrawCall& drawCall) const {
	switch (m_materialID) {

	case MaterialID::DYNAMIC_LIGHT:
	{
		setUniform("ambient", drawCall.renderComponent->getAmbient());
		setUniform("specular", drawCall.renderComponent->getSpecular());
		setUniform("gloss", drawCall.renderComponent->getGloss());
		setUniform("alpha", drawCall.renderComponent->getAlpha());

		glBindVertexArray(drawCall.gl_objects.VAO);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		auto world_transform_index = glGetUniformLocation(getGLProgramObject(), "world");
		glUniformMatrix4fv(world_transform_index, 1, GL_FALSE, glm::value_ptr(drawCall.world_transform));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, drawCall.gl_objects.TBO_diffuse);
		glBindSampler(0, drawCall.gl_objects.sampler_diffuse);
		setUniform("diffuseMap", 0);
	}

		break;

	case MaterialID::STATIC_LIGHT:
	{
		glBindVertexArray(drawCall.gl_objects.VAO);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(2);

		auto world_transform_index = glGetUniformLocation(getGLProgramObject(), "world");
		glUniformMatrix4fv(world_transform_index, 1, GL_FALSE, glm::value_ptr(drawCall.world_transform));
		setUniform("alpha", drawCall.renderComponent->getAlpha());

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, drawCall.gl_objects.TBO_diffuse);
		glBindSampler(0, drawCall.gl_objects.sampler_diffuse);
		setUniform("diffuseMap", 0);
	}
		break;
	case MaterialID::SCREEN_OVERLAY:
	{
		glBindVertexArray(drawCall.gl_objects.VAO);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(2);

		auto world_transform_index = glGetUniformLocation(getGLProgramObject(), "world");
		glUniformMatrix4fv(world_transform_index, 1, GL_FALSE, glm::value_ptr(drawCall.world_transform));
		setUniform("alpha", drawCall.renderComponent->getAlpha());

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, drawCall.gl_objects.TBO_diffuse);
		glBindSampler(0, drawCall.gl_objects.sampler_diffuse);
		setUniform("diffuseMap", 0);

		glDisable(GL_DEPTH_TEST);
	}
		break;
	default:
		break;
	}

}

void pn::Material::postRender() const {
	switch (m_materialID) {
	case MaterialID::SCREEN_OVERLAY:
		glEnable(GL_DEPTH_TEST);
		break;
	default:
		break;
	}
}
