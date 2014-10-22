#ifndef PN_MESH_H
#define PN_MESH_H

#include <vector>

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

		void setVertices(VertexDataContainer&& vertices);
		void setNormals(VertexDataContainer&& normals);
		void setTexes(VertexDataContainer&& texes);
	
	private:
		VertexDataContainer m_vertices;
		VertexDataContainer m_normals;
		VertexDataContainer m_texes;

	};
}

#endif