#include "PN/Util/ShaderLoader.h"

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