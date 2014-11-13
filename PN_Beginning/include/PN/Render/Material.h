#ifndef PN_SHADER_PROGRAM_H
#define PN_SHADER_PROGRAM_H

#include <vector>

#include "PN/Util/PString.h"
#include "PN/Util/Math.h"

namespace pn {

	struct DrawCall;
	class GameState;
	class RenderComponent;
	class ResourceManager;
	struct Renderable;

	using GLint = int;
	using GLuint = unsigned int;
	using HashValue = unsigned int;

	class Material {
	public:
		Material(GLuint program, pn::PString shader_program_filename, pn::PString vertex_shader, 
			pn::PString fragment_shader, int materialID);

		GLuint getGLProgramObject() const;
		int getMaterialID() const;

		const pn::PString& getVertexShaderFilename() const;
		const pn::PString& getFragmentShaderFilename() const;
		const pn::PString& getMaterialFilename() const;

		template<typename T>
		inline void setUniform(const std::string& uniform, T value) const;

		void setUpRenderable(pn::Renderable& renderable, const pn::RenderComponent& renderComponent, pn::ResourceManager& resourceManager) const;
		void setGlobalUniforms
			(
			pn::GameState& gameState,
			const std::vector<unsigned int>& lights, 
			const mat4& camera,
			const mat4& view,
			const mat4& proj
			) const;
		void setInstanceUniforms(pn::DrawCall& drawCall) const;

	private:
		GLuint m_program;

		int m_materialID;

		pn::PString m_vertex_shader_filename;
		pn::PString m_fragment_shader_filename;
		pn::PString m_shader_program_filename;

		std::vector<HashValue> m_uniforms;

		inline GLint getUniformLocation(const std::string& uniform) const;
	};
}

#endif



