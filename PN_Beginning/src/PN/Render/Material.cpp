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
void pn::Material::setUniform<int>(const PString& uniform, int value) const {
	GLint uniform_location = getUniformLocation(uniform);
	glUniform1i(uniform_location, value);
}

template<>
void pn::Material::setUniform<float>(const PString& uniform, float value) const {
	GLint uniform_location = getUniformLocation(uniform);
	glUniform1f(uniform_location, value);
}

template<>
void pn::Material::setUniform<float*>(const PString& uniform, float* value) const {
	GLint uniform_location = getUniformLocation(uniform);
	glUniform1fv(uniform_location, 1, value);
}

template<>
void pn::Material::setUniform<vec3>(const PString& uniform, vec3 value) const {
	GLint uniform_location = getUniformLocation(uniform);
	glUniform3fv(uniform_location, 1, glm::value_ptr(value));
}

template<>
void pn::Material::setUniform<vec4>(const PString& uniform, vec4 value) const {
	GLint uniform_location = getUniformLocation(uniform);
	glUniform4fv(uniform_location, 1, glm::value_ptr(value));
}

template<>
void pn::Material::setUniform<mat4>(const PString& uniform, mat4 value) const {
	GLint uniform_location = getUniformLocation(uniform);
	glUniformMatrix4fv(uniform_location, 1, GL_FALSE, glm::value_ptr(value));
}

template<>
void pn::Material::setUniform<pn::Light>(const PString& uniform, pn::Light value) const {
	setUniform(uniform + ".position", value.position);
	setUniform(uniform + ".direction", value.direction);
	setUniform(uniform + ".type", value.type);
	setUniform(uniform + ".colour", value.colour);
	setUniform(uniform + ".intensity", value.intensity);
	setUniform(uniform + ".innerRadians", value.innerRadians);
	setUniform(uniform + ".outerRadians", value.outerRadians);
	setUniform(uniform + ".maxRadius", value.maxRadius);
}

GLint pn::Material::getUniformLocation(const PString& uniform) const {
	HashValue hash = uniform.getHash();
	if (m_uniform_location_cache.count(hash) == 1) {
		return m_uniform_location_cache.at(hash);
	}
	else {
		GLint uniform_location = glGetUniformLocation(m_program, uniform.c_str());
		assert(uniform_location != -1);
		m_uniform_location_cache[hash] = uniform_location;
		return m_uniform_location_cache[hash];
	}
}

void pn::Material::setGlobalUniforms(pn::GameState& gameState, const std::vector<EntityID>& lights,
	const mat4& camera, const mat4& view) const {

	static const PString p_world = PString("world");
	static const PString p_num_lights = PString("num_lights");
	static const PString p_camera_position = PString("cameraPosition");
	static const PString p_view = PString("view");
	static const PString p_proj = PString("proj");

	switch (m_materialID) {

	case MaterialID::DYNAMIC_LIGHT:
	{
		{
			glUseProgram(getGLProgramObject());

			int num_lights_set = 0;
			for (auto entityID : lights) {
				auto& entity = gameState.getEntity(entityID);
				auto& light_component = std::dynamic_pointer_cast<pn::LightComponent>(entity.getComponent(pn::ComponentType::LIGHT));

				mat4 transform = gameState.getEntityWorldTransform(entityID, nullptr);

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

			setUniform(p_num_lights, num_lights_set);
		}

		const vec3& camera_position(camera[3].xyz);
		setUniform(p_camera_position, camera_position);
		setUniform(p_view, view);
		setUniform(p_proj, settings.getProjectionMatrix());
	}
		break;

	case MaterialID::STATIC_LIGHT:
	{
		glUseProgram(getGLProgramObject());

		const vec3& camera_position(camera[3].xyz);
		setUniform(p_view, view);
		setUniform(p_proj, settings.getProjectionMatrix());
	}
		break;
	case MaterialID::SCREEN_OVERLAY:
		glUseProgram(getGLProgramObject());

		setUniform(p_proj, settings.getHUDMatrix());
	default:
		break;

	}
}

void pn::Material::setInstanceUniforms(const mat4& worldTransform, const RenderComponent& renderComponent, 
	const Mesh& mesh, const Image& diffuse_texture, bool swapVAO) const {

	static const PString p_ambient = PString("ambient");
	static const PString p_specular = PString("specular");
	static const PString p_gloss = PString("gloss");
	static const PString p_alpha = PString("alpha");
	static const PString p_world = PString("world");
	static const PString p_diffuseMap = PString("diffuseMap");

	
	if (swapVAO) {
		glBindVertexArray(mesh.getVAO());

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
	}

	switch (m_materialID) {

	case MaterialID::DYNAMIC_LIGHT:
	{
		setUniform(p_ambient, renderComponent.getAmbient());
		setUniform(p_specular, renderComponent.getSpecular());
		setUniform(p_gloss, renderComponent.getGloss());
		setUniform(p_alpha, renderComponent.getAlpha());

		auto world_transform_index = getUniformLocation(p_world);
		glUniformMatrix4fv(world_transform_index, 1, GL_FALSE, glm::value_ptr(worldTransform));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuse_texture.getTBO());
		glBindSampler(0, diffuse_texture.getSamplerObject());
		setUniform(p_diffuseMap, 0);
	}

		break;

	case MaterialID::STATIC_LIGHT:
	{
		auto world_transform_index = getUniformLocation(p_world);
		glUniformMatrix4fv(world_transform_index, 1, GL_FALSE, glm::value_ptr(worldTransform));
		setUniform(p_alpha, renderComponent.getAlpha());

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuse_texture.getTBO());
		glBindSampler(0, diffuse_texture.getSamplerObject());
		setUniform(p_diffuseMap, 0);
	}
		break;
	case MaterialID::SCREEN_OVERLAY:
	{
		auto world_transform_index = getUniformLocation(p_world);
		glUniformMatrix4fv(world_transform_index, 1, GL_FALSE, glm::value_ptr(worldTransform));
		setUniform(p_alpha, renderComponent.getAlpha());

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuse_texture.getTBO());
		glBindSampler(0, diffuse_texture.getSamplerObject());
		setUniform(p_diffuseMap, 0);

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
