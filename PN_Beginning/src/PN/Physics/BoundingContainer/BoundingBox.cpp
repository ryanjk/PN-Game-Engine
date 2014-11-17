#include "PN/Physics/BoundingContainer/BoundingBox.h"

pn::BoundingBox::BoundingBox(float length, float width, float height) : 
pn::BoundingContainer(pn::BoundingContainerType::BOUNDING_BOX),
m_height(height), m_length(length), m_width(width)
{}

float pn::BoundingBox::getHeight() const {
	return m_height;
}

float pn::BoundingBox::getWidth() const {
	return m_width;
}

float pn::BoundingBox::getLength() const {
	return m_length;
}

