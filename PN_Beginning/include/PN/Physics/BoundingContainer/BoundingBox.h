#ifndef BOUNDING_BOX_H
#define BOUNDING_BOX_H

#include "PN/Physics/BoundingContainer/BoundingContainer.h"

namespace pn {
	class BoundingBox : public pn::BoundingContainer {
	public:
		BoundingBox(float length, float width, float height);

		void update(const mat4& worldMatrix) override;

		float getHeight() const;
		float getWidth() const;
		float getLength() const;

		const mat4& getFrontQuad() const;
		const mat4& getBackQuad() const;
		
	private:
		float m_length;
		float m_width;
		float m_height;

		mat4 m_front_quad;
		mat4 m_back_quad;

		mat4 m_world_front_quad;
		mat4 m_world_back_quad;
	};
}

#endif