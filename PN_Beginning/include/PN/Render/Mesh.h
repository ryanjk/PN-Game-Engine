#ifndef PN_MESH_H
#define PN_MESH_H

#include <vector>
#include <memory>

#include "PN/Physics/BoundingContainer/BoundingContainer.h"

using VertexData = float;
using VertexDataContainer = std::vector < VertexData > ;

namespace pn {
	struct Mesh {
	public:
		Mesh();
		Mesh(const Mesh& mesh) = default;
		Mesh(Mesh&& mesh) _NOEXCEPT;

		Mesh& operator=(Mesh&& mesh) _NOEXCEPT;
		
		const VertexDataContainer& getVertices() const;
		const VertexDataContainer& getNormals() const;
		const VertexDataContainer& getTexes() const;
		const std::shared_ptr<pn::BoundingContainer>& getBoundingContainer() const;

		void setVertices(VertexDataContainer&& vertices);
		void setNormals(VertexDataContainer&& normals);
		void setTexes(VertexDataContainer&& texes);
		void setBoundingContainer(std::shared_ptr<pn::BoundingContainer> boundingContainer);
	
	private:
		VertexDataContainer m_vertices;
		VertexDataContainer m_normals;
		VertexDataContainer m_texes;

		std::shared_ptr<pn::BoundingContainer> m_boundingContainer;

	};
}

#endif