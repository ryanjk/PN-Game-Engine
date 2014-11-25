#ifndef PN_BOUNDING_CONTAINER_H
#define PN_BOUNDING_CONTAINER_H

#include "PN/Util/Math.h"

namespace pn {

	enum BoundingContainerType {
		BOUNDING_BOX = 1
	};

	class BoundingContainer {
	public:
		BoundingContainer(BoundingContainerType type) : m_type(type) {}
		virtual ~BoundingContainer() {}

		BoundingContainerType getContainerType() const { return m_type; }

		virtual void update(const mat4& worldMatrix) = 0;
	private:
		BoundingContainerType m_type;
	};
}

#endif