#ifndef PN_MESH_H
#define PN_MESH_H

#include <vector>
#include <memory>

#include "PN/Window/WindowManager.h"

#include "PN/Physics/BoundingContainer/BoundingContainer.h"

using VertexData = float;
using VertexDataContainer = std::vector < VertexData > ;

namespace pn {
	class Mesh {
	public:
		friend class RenderFactory;
		Mesh();
		~Mesh();

		Mesh(Mesh&& mesh);
		Mesh& operator=(Mesh&& mesh);
		
		const GLuint& getVAO() const;
		const GLuint& getVBO() const;
		unsigned int getNumVertices() const;
		const std::shared_ptr<pn::BoundingContainer>& getBoundingContainer() const;

		void setBoundingContainer(std::shared_ptr<pn::BoundingContainer> boundingContainer);
	
	private:
		unsigned int m_num_vertices;
		GLuint m_vao;
		GLuint m_vbo;

		std::shared_ptr<pn::BoundingContainer> m_boundingContainer;

	};
}

#endif