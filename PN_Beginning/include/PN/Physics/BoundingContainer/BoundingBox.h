#ifndef BOUNDING_BOX_H
#define BOUNDING_BOX_H

#include "PN/Physics/BoundingContainer/BoundingContainer.h"

namespace pn {
	class BoundingBox : public pn::BoundingContainer {
	public:
		BoundingBox(float length, float width, float height); 
		BoundingBox(BoundingContainer* boundingContainer, float scaleFactor);

		void update(const mat4& worldMatrix) override;

		const mat4& getFrontQuad() const;
		const mat4& getBackQuad() const;
		
	private:
		mat4 m_box_scale;

		static const mat4 m_front_quad;
		static const mat4 m_back_quad;

		mat4 m_world_front_quad;
		mat4 m_world_back_quad;
	};
}

#endif