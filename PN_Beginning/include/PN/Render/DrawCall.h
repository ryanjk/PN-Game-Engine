#ifndef PN_DRAW_CALL_H
#define PN_DRAW_CALL_H

#include "PN/Util/Math.h"

#include <memory>

class Material;
class RenderComponent;
struct Renderable;

namespace pn {
	struct DrawCall {
		mat4 world_transform;
		Renderable gl_objects;
		Material material;
		unsigned int num_vertices;
		std::shared_ptr<RenderComponent> renderComponent;
	};
}

#endif 