#ifndef PN_SHADER_PROGRAM_H
#define PN_SHADER_PROGRAM_H

#include "PN/Window/WindowManager.h"

#include "PN/Util/PString.h"

#include <vector>

namespace pn {
	class ShaderProgram {
	public:
		ShaderProgram(GLuint program, pn::PString shader_program_filename, pn::PString vertex_shader, 
			pn::PString fragment_shader, int materialID);

		GLuint getGLProgramObject() const;
		int getMaterialID() const;

		const pn::PString& getVertexShaderFilename() const;
		const pn::PString& getFragmentShaderFilename() const;
		const pn::PString& getShaderProgramFilename() const;

		template<typename T>
		void setUniform(const std::string& uniform, T value) const;

		void addUniform(const pn::PString& uniform);

	private:
		GLuint m_program;

		int m_materialID;

		pn::PString m_vertex_shader_filename;
		pn::PString m_fragment_shader_filename;
		pn::PString m_shader_program_filename;

		std::vector<HashValue> m_uniforms;

		GLint getUniformLocation(const std::string& uniform) const;
	};
}

#endif