#ifndef PN_IMAGE_H
#define PN_IMAGE_H

#include <vector>

#include "PN/Window/WindowManager.h"

namespace pn {
	class Image {
	public:
		friend class RenderFactory;
		Image();
		~Image();

		const GLuint& getTBO() const;
		const GLuint& getSamplerObject() const;

	private:
		GLuint m_tbo;
		GLuint m_sampler;
	};
}

#endif