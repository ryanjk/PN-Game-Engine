#include "PN/Physics/BoundingContainer/BoundingOBB.h"

const mat4 pn::BoundingOBB::m_front_quad = 
mat4(vec4(-0.5f, -0.5f, 0.5f, 1.0f),
	vec4(0.5f, -0.5f, 0.5f, 1.0f),
	vec4(0.5f, 0.5f, 0.5f, 1.0f),
	vec4(-0.5f, 0.5f, 0.5f, 1.0f));

const mat4 pn::BoundingOBB::m_back_quad =
mat4(vec4(-0.5f, -0.5f, -0.5f, 1.0f),
vec4(0.5f, -0.5f, -0.5f, 1.0f),
vec4(0.5f, 0.5f, -0.5f, 1.0f),
vec4(-0.5f, 0.5f, -0.5f, 1.0f));

pn::BoundingOBB::BoundingOBB(float length, float width, float height) :
pn::BoundingContainer(pn::BoundingContainerType::BOUNDING_OBB),
m_box_scale(glm::scale(mat4(), vec3(length, height, width)))
{
	m_world_back_quad = m_back_quad;
	m_world_front_quad = m_front_quad;

}

pn::BoundingOBB::BoundingOBB(BoundingContainer* boundingContainer, float scaleFactor) :
pn::BoundingContainer(pn::BoundingContainerType::BOUNDING_OBB)
{
	if (boundingContainer->getContainerType() == BoundingContainerType::BOUNDING_OBB) {
		m_box_scale = mat4(glm::scale(mat4(), vec3(scaleFactor)) * (*((BoundingOBB*)boundingContainer)).m_box_scale);
		m_world_back_quad = m_back_quad;
		m_world_front_quad = m_front_quad;
	}

}

void pn::BoundingOBB::update(const mat4& worldMatrix) {

	m_transform = worldMatrix * m_box_scale;

	m_world_front_quad = m_transform * BoundingOBB::m_front_quad;
	m_world_back_quad = m_transform * BoundingOBB::m_back_quad;
}

const mat4& pn::BoundingOBB::getFrontQuad() const {
	return m_world_front_quad;
}

const mat4& pn::BoundingOBB::getBackQuad() const {
	return m_world_back_quad;
}

