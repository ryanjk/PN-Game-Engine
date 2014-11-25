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

	GLfloat buffer_data[] = {
		-length / 2, -height / 2, width / 2, 
		length / 2, -height / 2, width / 2,
		length / 2, height / 2, width / 2,
		-length / 2, height / 2, width / 2,
		-length / 2, -height / 2, -width / 2,
		length / 2, -height / 2, -width / 2,
		length / 2, height / 2, -width / 2,
		-length / 2, height / 2, -width / 2
	};

	GLushort buffer_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// top
		3, 2, 6,
		6, 7, 3,
		// back
		7, 6, 5,
		5, 4, 7,
		// bottom
		4, 5, 1,
		1, 0, 4,
		// left
		4, 0, 3,
		3, 7, 4,
		// right
		1, 5, 6,
		6, 2, 1,
	};

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(GLfloat), buffer_data, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &m_IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(GLushort), buffer_indices, GL_STATIC_DRAW);	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

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

