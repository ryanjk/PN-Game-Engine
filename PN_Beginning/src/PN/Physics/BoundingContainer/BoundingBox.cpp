#include "PN/Physics/BoundingContainer/BoundingBox.h"

pn::BoundingBox::BoundingBox(float length, float width, float height) :
pn::BoundingContainer(pn::BoundingContainerType::BOUNDING_BOX),
m_height(height), m_length(length), m_width(width),
m_front_quad(), m_back_quad()
{
	vec4 front_left_bottom = vec4(-length / 2, -height / 2, width / 2, 1.0f);
	vec4 front_right_bottom = vec4(length / 2, -height / 2, width / 2, 1.0f);
	vec4 front_right_top = vec4(length / 2, height / 2, width / 2, 1.0f);
	vec4 front_left_top = vec4(-length / 2, height / 2, width / 2, 1.0f);

	vec4 back_left_bottom = vec4(-length / 2, -height / 2, -width / 2, 1.0f);
	vec4 back_right_bottom = vec4(length / 2, -height / 2, -width / 2, 1.0f);
	vec4 back_right_top = vec4(length / 2, height / 2, -width / 2, 1.0f);
	vec4 back_left_top = vec4(-length / 2, height / 2, -width / 2, 1.0f);

	m_front_quad = mat4(front_left_bottom, front_right_bottom, front_right_top, front_left_top);
	m_back_quad = mat4(back_left_bottom, back_right_bottom, back_right_top, back_left_top);

	m_world_back_quad = m_back_quad;
	m_world_front_quad = m_front_quad;
}

void pn::BoundingBox::update(const mat4& worldMatrix) {
	m_world_front_quad = worldMatrix * m_front_quad;
	m_world_back_quad = worldMatrix * m_back_quad;
}

float pn::BoundingBox::getHeight() const {
	return m_height;
}

float pn::BoundingBox::getWidth() const {
	return m_width;
}

float pn::BoundingBox::getLength() const {
	return m_length;
}

const mat4& pn::BoundingBox::getFrontQuad() const {
	return m_world_front_quad;
}

const mat4& pn::BoundingBox::getBackQuad() const {
	return m_world_back_quad;
}

