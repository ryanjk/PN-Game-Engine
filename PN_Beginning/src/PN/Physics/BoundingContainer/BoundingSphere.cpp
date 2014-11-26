#include "PN/Physics/BoundingContainer/BoundingSphere.h"

#include <iostream>

pn::BoundingSphere::BoundingSphere(float radius) :
pn::BoundingContainer(pn::BoundingContainerType::BOUNDING_SPHERE),
m_radius(radius), m_world_position(), m_scaleFactor()
{

}

pn::BoundingSphere::BoundingSphere(BoundingContainer* boundingContainer, float scaleFactor) :
pn::BoundingContainer(pn::BoundingContainerType::BOUNDING_SPHERE), m_world_position()
{
	if (boundingContainer->getContainerType() == BoundingContainerType::BOUNDING_SPHERE) {
		auto& otherBoundingSphere = *((BoundingSphere*)boundingContainer);

		m_radius = scaleFactor * otherBoundingSphere.getRadius();
		m_scaleFactor = m_radius;
	}
}

void pn::BoundingSphere::update(const mat4& worldMatrix) {
	m_radius = glm::length(vec3(worldMatrix[0].xyz)) * m_scaleFactor;
	m_world_position += vec3(worldMatrix[2].xyz);
	m_transform = worldMatrix * glm::scale(mat4(), vec3(m_scaleFactor));
}

const vec3& pn::BoundingSphere::getPosition() const {
	return m_world_position;
}

float pn::BoundingSphere::getRadius() const {
	return m_radius;
}
