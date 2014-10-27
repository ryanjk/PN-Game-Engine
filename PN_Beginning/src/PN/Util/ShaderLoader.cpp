#include "PN/Util/ShaderLoader.h"
#include "PN/Util/ResourceManager.h"

#include "json/json.h"

#include <fstream>
#include <iostream>
#include <vector>

GLuint pn::ShaderLoader::loadShader(const char* filename, ShaderType shaderType) {
	/*
	From Nex at:
	http://www.nexcius.net/2012/11/20/how-to-load-a-glsl-shader-in-opengl-using-c/
	*/
	
	GLuint shader = glCreateShader(shaderType);

	std::string shaderText = readFile(filename);
	const char* shader_cStr = shaderText.c_str();

	GLint result = GL_FALSE;
	int logLength;

	glShaderSource(shader, 1, &shader_cStr, NULL);
	glCompileShader(shader);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
	bool isError = logLength > 1;
	if (isError) {
		std::vector<char> vertShaderError(logLength);
		glGetShaderInfoLog(shader, logLength, NULL, &vertShaderError[0]);
		std::cout << &vertShaderError[0] << std::endl;
	}

	return shader;
}

pn::ShaderProgram pn::ShaderLoader::loadProgram(const char* filename) {
	Json::Reader reader;
	Json::Value root;

	std::ifstream program_file;
	program_file.open(filename);

	if (!program_file.is_open()) {
		std::cout << "ShaderLoader: Could not locate shader program file: " << std::string(filename) << std::endl;
		getchar();
		exit(-1);
	}

	bool success = reader.parse(program_file, root);
	if (!success) {
		std::cout << "ShaderLoader: Could not parse shader program file: " << reader.getFormattedErrorMessages() << std::endl;
		getchar();
		exit(-1);
	}

	program_file.close();

	auto vertex_shader = root["vertex"];
	if (vertex_shader.isNull()) {
		std::cout << "ShaderLoader: Could not find vertex shader" << std::endl;
		getchar();
		exit(-1);
	}
	pn::ResourceManager::g_resourceManager.load(vertex_shader.asString());

	auto fragment_shader = root["fragment"];
	if (fragment_shader.isNull()) {
		std::cout << "ShaderLoader: Could not find fragment shader" << std::endl;
		getchar();
		exit(-1);
	}
	pn::ResourceManager::g_resourceManager.load(fragment_shader.asString());

	GLuint vertex_shader_object = pn::ResourceManager::g_resourceManager.getVertexShader(vertex_shader.asString());
	GLuint fragment_shader_object = pn::ResourceManager::g_resourceManager.getFragmentShader(fragment_shader.asString());
	GLuint program = pn::ShaderLoader::loadProgram(vertex_shader_object, fragment_shader_object);

	pn::ShaderProgram shader_program(program, vertex_shader.asString(), fragment_shader.asString());

	GLint num_active_uniforms;
	glGetProgramInterfaceiv(program, GL_UNIFORM, GL_ACTIVE_RESOURCES, &num_active_uniforms);

	std::vector<GLchar> nameData(256);
	std::vector<GLenum> properties = { GL_NAME_LENGTH };
	std::vector<GLint> values(properties.size());
	for (int uniform = 0; uniform < num_active_uniforms; uniform++) {
		glGetProgramResourceiv(program, GL_UNIFORM, uniform, properties.size(), &properties[0], values.size(), NULL, &values[0]);

		nameData.resize(values[0]);

		glGetProgramResourceName(program, GL_UNIFORM, uniform, nameData.size(), NULL, &nameData[0]);

		std::string name((char*)&nameData[0], nameData.size() - 1);
		shader_program.addUniform(name);
	}

	return shader_program;
}

GLuint pn::ShaderLoader::loadProgram(GLuint vertexShader, GLuint fragmentShader) {
	GLuint program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	GLint result = GL_FALSE;
	int logLength;

	glGetProgramiv(program, GL_LINK_STATUS, &result);
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
	bool isError = logLength > 1;
	if (isError) {
		std::vector<char> programError(logLength);
		glGetProgramInfoLog(program, logLength, NULL, &programError[0]);
		std::cout << &programError[0] << std::endl;
	}
	return program;
}

std::string pn::ShaderLoader::readFile(const char* filename) {
	/*
	From Nex at:
	http://www.nexcius.net/2012/11/20/how-to-load-a-glsl-shader-in-opengl-using-c/
	*/

	std::string content;
	std::ifstream fileStream(filename, std::ios::in);

	if (!fileStream.is_open()) {
		std::cerr << "ShaderLoader: Could not read file " << filename << ". File does not exist." << std::endl;
		return "";
	}

	std::string line = "";
	while (!fileStream.eof()) {
		std::getline(fileStream, line);
		content.append(line + "\n");
	}

	fileStream.close();
	return content;
}