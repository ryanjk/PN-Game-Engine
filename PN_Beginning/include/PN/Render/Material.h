#ifndef PN_SHADER_PROGRAM_H
#define PN_SHADER_PROGRAM_H

#include <vector>

#include "PN/Util/PString.h"
#include "PN/Util/Math.h"

#include <map>

namespace pn {

	struct DrawCall;
	class GameState;
	class RenderComponent;
	class ResourceManager;
	class Mesh;
	class Image;

	using GLint = int;
	using GLuint = unsigned int;
	using HashValue = unsigned int;

	enum MaterialID {
		DYNAMIC_LIGHT = 1,
		STATIC_LIGHT = 2,
		SCREEN_OVERLAY = 3
	};

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
		inline void setUniform(const PString& uniform, T value) const;

		void setGlobalUniforms
			(
			pn::GameState& gameState,
			const std::vector<unsigned int>& lights, 
			const mat4& camera,
			const mat4& view
			) const;
		void setInstanceUniforms
			(
			const mat4& worldTransform, 
			const RenderComponent& renderComponent, 
			const Mesh& mesh, 
			const Image& diffuse_texture,
			bool swapVAO
			) const;
		void postRender() const;

	private:
		GLuint m_program;

		int m_materialID;

		pn::PString m_vertex_shader_filename;
		pn::PString m_fragment_shader_filename;
		pn::PString m_shader_program_filename;

		std::vector<HashValue> m_uniforms;

		inline GLint getUniformLocation(const PString& uniform) const;
		mutable std::map<HashValue, GLint> m_uniform_location_cache;
	};
}

#endif



