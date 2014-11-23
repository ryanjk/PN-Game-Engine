#ifndef PN_DRAW_CALL_H
#define PN_DRAW_CALL_H

#include "PN/Util/Math.h"

#include <memory>
#include <map>

class Material;
class RenderComponent;
struct Renderable;

namespace pn {
	struct DrawCall {
		DrawCall() : world_transform(), renderComponent() {}
		DrawCall(const mat4& p_world_transform, std::shared_ptr<RenderComponent> p_renderComponent) : 
			world_transform(p_world_transform), renderComponent(p_renderComponent) {}

		mat4 world_transform;
		std::shared_ptr<RenderComponent> renderComponent;
	};
	using DrawCallContainer = std::multimap < int, pn::DrawCall > ;
}

#endif 