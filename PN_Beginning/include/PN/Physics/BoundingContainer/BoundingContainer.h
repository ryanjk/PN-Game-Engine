#ifndef PN_BOUNDING_CONTAINER_H
#define PN_BOUNDING_CONTAINER_H

namespace pn {

	enum BoundingContainerType {
		BOUNDING_BOX = 1
	};

	class BoundingContainer {
	public:
		BoundingContainer(BoundingContainerType type) : m_type(type) {}
		BoundingContainerType getContainerType() const { return m_type; }
	private:
		BoundingContainerType m_type;
	};
}

#endif