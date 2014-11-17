#ifndef BOUNDING_BOX_H
#define BOUNDING_BOX_H

#include "PN/Physics/BoundingContainer/BoundingContainer.h"

namespace pn {
	class BoundingBox : public pn::BoundingContainer {
	public:
		BoundingBox(float length, float width, float height);

		float getHeight() const;
		float getWidth() const;
		float getLength() const;

	private:
		float m_length;
		float m_width;
		float m_height;
	};
}

#endif