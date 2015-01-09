#include "PN/Physics/BoundingContainer/BoundingAABB.h"

pn::BoundingAABB::BoundingAABB(float length, float width, float height) :
pn::BoundingContainer(pn::BoundingContainerType::BOUNDING_AABB), m_center(), m_original_radius( length / 2.0f, height / 2.0f, width / 2.0f ), m_scale(1), m_radius()
{
	m_transform = glm::translate(mat4(), m_center) * glm::scale(mat4(), vec3(length, height, width));  // as applied to unit cube
}

pn::BoundingAABB::BoundingAABB(pn::BoundingContainer* boundingContainer, float scale) :
pn::BoundingContainer(pn::BoundingContainerType::BOUNDING_AABB) {
	if (boundingContainer->getContainerType() == pn::BoundingContainerType::BOUNDING_AABB) {
		m_center = ((BoundingAABB*)boundingContainer)->m_center;
		m_original_radius = ((BoundingAABB*)boundingContainer)->m_original_radius * scale;
		m_scale = scale;
		m_transform = glm::translate(mat4(), m_center) * glm::scale(mat4(), 2.0f * vec3(m_radius[0], m_radius[1], m_radius[2]));
	}
	else {
		assert(false);
	}
}

void pn::BoundingAABB::update(const mat4& worldMatrix) {
	// From Real-Time Collision Detection p. 87
	m_center = worldMatrix[3].xyz;
	m_radius = vec3(0);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			m_radius[i] += glm::abs(worldMatrix[i][j]) * m_original_radius[j];
		}
	}
	m_transform = glm::translate(mat4(), m_center) * glm::scale(mat4(), 2.0f * m_radius);
}

const vec3& pn::BoundingAABB::getCenter() const {
	return m_center;
}

const vec3& pn::BoundingAABB::getRadius() const {
	return m_radius;
}

