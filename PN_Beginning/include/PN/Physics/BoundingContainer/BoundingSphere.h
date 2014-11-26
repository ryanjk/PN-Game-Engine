#ifndef PN_SPHERE_H
#define PN_SPHERE_H

#include "PN/Physics/BoundingContainer/BoundingContainer.h"

namespace pn {
	class BoundingSphere : public pn::BoundingContainer {
	public:
		BoundingSphere(float radius);
		BoundingSphere(BoundingContainer* boundingContainer, float scaleFactor);

		void update(const mat4& worldMatrix) override;

		const vec3& getPosition() const;
		float getRadius() const;

	private:
		vec3 m_world_position;
		float m_radius;

		float m_scaleFactor;
	};
}

#endif