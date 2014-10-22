#include "PN/Render/Mesh.h"

pn::Mesh::Mesh() {}

pn::Mesh::Mesh(Mesh&& mesh)
	: m_vertices(std::move(mesh.m_vertices)), m_normals(std::move(mesh.m_normals)), m_texes(std::move(mesh.m_texes))
{}

pn::Mesh& pn::Mesh::operator=(Mesh&& mesh) {
	m_vertices = std::move(mesh.m_vertices);
	m_normals = std::move(mesh.m_normals);
	m_texes = std::move(mesh.m_texes);
	return *this;
}

void pn::Mesh::setVertices(VertexDataContainer&& vertices) {
	m_vertices = std::move(vertices);
}

void pn::Mesh::setNormals(VertexDataContainer&& normals) {
	m_normals = std::move(normals);
}

void pn::Mesh::setTexes(VertexDataContainer&& texes) {
	m_texes = std::move(texes);
}

const VertexDataContainer& pn::Mesh::getVertices() const {
	return m_vertices;
}

const VertexDataContainer& pn::Mesh::getNormals() const {
	return m_normals;
}

const VertexDataContainer& pn::Mesh::getTexes() const {
	return m_texes;
}