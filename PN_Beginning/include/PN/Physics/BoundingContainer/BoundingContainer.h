#ifndef PN_BOUNDING_CONTAINER_H
#define PN_BOUNDING_CONTAINER_H

#include "PN/Util/Math.h"

#include "PN/Window/WindowManager.h"

namespace pn {

	enum BoundingContainerType {
		BOUNDING_BOX = 1
	};

	class BoundingContainer {
	public:
		BoundingContainer(BoundingContainerType type) : m_type(type) {}
		virtual ~BoundingContainer() {}

		BoundingContainerType getContainerType() const { return m_type; }

		const GLuint& getVAO() const { return m_VAO; }
		const GLuint& getVBO() const { return m_VBO; }
		const GLuint& getIBO() const { return m_IBO; }

		virtual void update(const mat4& worldMatrix) = 0;
	protected:
		GLuint m_VAO;
		GLuint m_VBO;
		GLuint m_IBO;
	private:
		BoundingContainerType m_type;
	};
}

#endif