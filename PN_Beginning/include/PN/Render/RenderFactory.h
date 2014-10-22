#ifndef PN_IMAGE_FACTORY_H
#define PN_IMAGE_FACTORY_H

#include "PN/Render/Image.h"
#include "PN/Render/Mesh.h"


namespace pn {
	class RenderFactory {
	public:
		static pn::Image makeFromPNG(const char* filename);
		static pn::Mesh loadMeshFromObj(const char* filename);
	};
}

#endif