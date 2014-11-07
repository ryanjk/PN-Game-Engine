#ifndef RENDERABLE_H
#define RENDERABLE_H

#include "PN/Window/WindowManager.h"

#include "PN/Util/PString.h"

namespace pn {
	struct Renderable {
		GLuint VAO;

		GLuint VBO_v;
		GLuint VBO_vn;
		GLuint VBO_vt;

		GLuint TBO_diffuse;
		GLuint sampler_diffuse;
		HashValue image_diffuse;

		GLuint SHADER_v;
		GLuint SHADER_f;
		GLuint SHADER_program;

		HashValue mesh;
		
	};
}

#endif