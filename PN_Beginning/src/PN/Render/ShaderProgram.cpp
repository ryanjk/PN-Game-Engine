#include "PN/Render/ShaderProgram.h"

#include "PN/Util/Math.h"

#include <iostream>

pn::ShaderProgram::ShaderProgram(GLuint program, pn::PString vertex_shader, pn::PString fragment_shader) 
	: m_program(program), m_vertex_shader_filename(vertex_shader), m_fragment_shader_filename(fragment_shader) {}

GLuint pn::ShaderProgram::getGLProgramObject() const {
	return m_program;
}

pn::PString pn::ShaderProgram::getVertexShaderFilename() const {
	return m_vertex_shader_filename;
}

pn::PString pn::ShaderProgram::getFragmentShaderFilename() const {
	return m_fragment_shader_filename;
}

void pn::ShaderProgram::addUniform(pn::PString uniform) {
	m_uniforms.push_back(uniform.getHash());
}

template<>
void pn::ShaderProgram::setUniform<int>(pn::PString uniform, int value) {
	GLint uniform_location = getUniformLocation(uniform);
	glUniform1i(uniform_location, value);
}

template<>
void pn::ShaderProgram::setUniform<float>(pn::PString uniform, float value) {
	GLint uniform_location = getUniformLocation(uniform);
	glUniform1f(uniform_location, value);
}

template<>
void pn::ShaderProgram::setUniform<float*>(pn::PString uniform, float* value) {
	GLint uniform_location = getUniformLocation(uniform);
	glUniform1fv(uniform_location, 1, value);
}

template<>
void pn::ShaderProgram::setUniform<vec3>(pn::PString uniform, vec3 value) {
	GLint uniform_location = getUniformLocation(uniform);
	glUniform3fv(uniform_location, 1, glm::value_ptr(value));
}

template<>
void pn::ShaderProgram::setUniform<vec4>(pn::PString uniform, vec4 value) {
	GLint uniform_location = getUniformLocation(uniform);
	glUniform4fv(uniform_location, 1, glm::value_ptr(value));
}

template<>
void pn::ShaderProgram::setUniform<mat4>(pn::PString uniform, mat4 value) {
	GLint uniform_location = getUniformLocation(uniform);
	glUniformMatrix4fv(uniform_location, 1, GL_FALSE, glm::value_ptr(value));
}

GLint pn::ShaderProgram::getUniformLocation(pn::PString uniform) {
	GLint uniform_location = glGetUniformLocation(m_program, uniform.getText().c_str());
	if (uniform_location == -1) {
	//	std::cout << "ERROR: Couldn't set uniform " << uniform.getText() << ": Not found in program" << std::endl;
	}
	return uniform_location;
}