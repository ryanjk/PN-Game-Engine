#ifndef PN_SHADER_LOADER_H
#define PN_SHADER_LOADER_H

#include "PN/Window/WindowManager.h"

#include <string>

using ShaderType = GLenum;

namespace pn {
	class ShaderLoader {
	public:
		static GLuint loadShader(const char* filename, ShaderType shaderType);
		static GLuint loadProgram(GLuint vertexShader, GLuint fragmentShader);
	private:
		static std::string readFile(const char* filename);

	};
}

#endif