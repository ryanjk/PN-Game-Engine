#ifndef BOUNDING_OBB_H
#define BOUNDING_OBB_H

#include "PN/Physics/BoundingContainer/BoundingContainer.h"

namespace pn {
	class BoundingOBB : public pn::BoundingContainer {
	public:
		BoundingOBB(float length, float width, float height); 
		BoundingOBB(BoundingContainer* boundingContainer, float scaleFactor);

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