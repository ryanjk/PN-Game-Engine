#include "PN/Render/Mesh.h"

#include <iostream>

pn::Mesh::Mesh() : m_vao(0), m_vbo(0), m_num_vertices(0), m_boundingContainer() {}

pn::Mesh::~Mesh() {

}

pn::Mesh::Mesh(Mesh&& mesh)
	: m_boundingContainer(mesh.m_boundingContainer), m_vao(mesh.m_vao), 
	m_vbo(mesh.m_vbo), m_num_vertices(mesh.m_num_vertices)
{}

pn::Mesh& pn::Mesh::operator=(Mesh&& mesh) {
	m_vao = mesh.m_vao;
	m_vbo = mesh.m_vbo;
	m_boundingContainer = mesh.m_boundingContainer;
	m_num_vertices = mesh.m_num_vertices;
	return *this;
}

unsigned int pn::Mesh::getNumVertices() const {
	return m_num_vertices;
}

const GLuint& pn::Mesh::getVAO() const {
	return m_vao;
}

const GLuint& pn::Mesh::getVBO() const {
	return m_vbo;
}

void pn::Mesh::setBoundingContainer(std::shared_ptr<pn::BoundingContainer> boundingContainer) {
	m_boundingContainer = boundingContainer;
}

const std::shared_ptr<pn::BoundingContainer>& pn::Mesh::getBoundingContainer() const {
	return m_boundingContainer;
}