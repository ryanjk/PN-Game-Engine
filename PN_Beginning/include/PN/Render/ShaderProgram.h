#ifndef PN_SHADER_PROGRAM_H
#define PN_SHADER_PROGRAM_H

#include "PN/Window/WindowManager.h"

#include "PN/Util/PString.h"

#include <vector>

namespace pn {
	class ShaderProgram {
	public:
		ShaderProgram(GLuint program, pn::PString vertex_shader, pn::PString fragment_shader);

		GLuint getGLProgramObject() const;

		pn::PString getVertexShaderFilename() const;
		pn::PString getFragmentShaderFilename() const;

		template<typename T>
		void setUniform(pn::PString uniform, T value);

		void addUniform(pn::PString uniform);

	private:
		GLuint m_program;

		pn::PString m_vertex_shader_filename;
		pn::PString m_fragment_shader_filename;

		std::vector<HashValue> m_uniforms;

		GLint getUniformLocation(pn::PString uniform);
	};
}

#endif