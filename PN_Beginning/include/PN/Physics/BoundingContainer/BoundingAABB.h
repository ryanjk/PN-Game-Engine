#ifndef BOUNDING_AABB_H
#define BOUNDING_AABB_H

#include "PN/Physics/BoundingContainer/BoundingContainer.h"

#include <vector>

namespace pn {
	class BoundingAABB : public pn::BoundingContainer {
	public:
		BoundingAABB(float length, float width, float height);
		BoundingAABB(pn::BoundingContainer* boundingContainer, float scale);

		void update(const mat4& worldMatrix) override;

		const vec3& getCenter() const;
		const vec3& getRadius() const;

	private:
		vec3 m_center;
		vec3 m_original_radius;
		vec3 m_radius; // radius for x, y and z axes
		float m_scale;
	};
}

#endif