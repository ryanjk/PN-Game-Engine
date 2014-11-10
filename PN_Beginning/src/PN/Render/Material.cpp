#include "PN/Render/Material.h"
#include "PN/Render/Light.h"

#include "PN/Util/Math.h"

#include <iostream>

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
	GLint uniform_location = getUniformLocation(uniform);
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