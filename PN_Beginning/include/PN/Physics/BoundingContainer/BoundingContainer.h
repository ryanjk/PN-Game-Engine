#ifndef PN_BOUNDING_CONTAINER_H
#define PN_BOUNDING_CONTAINER_H

#include "PN/Util/Math.h"

#include "PN/Window/WindowManager.h"

namespace pn {

	enum BoundingContainerType {
		BOUNDING_OBB = 1,
		BOUNDING_SPHERE = 2,
		BOUNDING_AABB = 3
	};

	class BoundingContainer {
	public:
		BoundingContainer(BoundingContainerType type) : m_type(type), m_transform() {}
		virtual ~BoundingContainer() {}

		BoundingContainerType getContainerType() const { return m_type; }

		virtual void update(const mat4& worldMatrix) = 0;
		const mat4& getTransform() const { return m_transform; }

	protected:
		mat4 m_transform;

	private:
		BoundingContainerType m_type;
	};
}

#endif