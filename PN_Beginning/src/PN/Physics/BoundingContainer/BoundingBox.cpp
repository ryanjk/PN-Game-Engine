#include "PN/Physics/BoundingContainer/BoundingBox.h"

const mat4 pn::BoundingBox::m_front_quad = 
mat4(vec4(-0.5f, -0.5f, 0.5f, 1.0f),
	vec4(0.5f, -0.5f, 0.5f, 1.0f),
	vec4(0.5f, 0.5f, 0.5f, 1.0f),
	vec4(-0.5f, 0.5f, 0.5f, 1.0f));

const mat4 pn::BoundingBox::m_back_quad =
mat4(vec4(-0.5f, -0.5f, -0.5f, 1.0f),
vec4(0.5f, -0.5f, -0.5f, 1.0f),
vec4(0.5f, 0.5f, -0.5f, 1.0f),
vec4(-0.5f, 0.5f, -0.5f, 1.0f));

pn::BoundingBox::BoundingBox(float length, float width, float height) :
pn::BoundingContainer(pn::BoundingContainerType::BOUNDING_BOX),
m_box_scale(glm::scale(mat4(), vec3(length, height, width)))
{
	m_world_back_quad = m_back_quad;
	m_world_front_quad = m_front_quad;

}

pn::BoundingBox::BoundingBox(BoundingContainer* boundingContainer, float scaleFactor) :
pn::BoundingContainer(pn::BoundingContainerType::BOUNDING_BOX)
{
	if (boundingContainer->getContainerType() == BoundingContainerType::BOUNDING_BOX) {
		m_box_scale = mat4(glm::scale(mat4(), vec3(scaleFactor)) * (*((BoundingBox*)boundingContainer)).m_box_scale);
		m_world_back_quad = m_back_quad;
		m_world_front_quad = m_front_quad;
	}

}

void pn::BoundingBox::update(const mat4& worldMatrix) {

	m_transform = worldMatrix * m_box_scale;

	m_world_front_quad = m_transform * BoundingBox::m_front_quad;
	m_world_back_quad = m_transform * BoundingBox::m_back_quad;
}

const mat4& pn::BoundingBox::getFrontQuad() const {
	return m_world_front_quad;
}

const mat4& pn::BoundingBox::getBackQuad() const {
	return m_world_back_quad;
}

