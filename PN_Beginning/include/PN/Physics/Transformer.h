#ifndef PN_TRANSFORMER_H
#define PN_TRANSFORMER_H

#include "PN/Util/Math.h"

namespace pn {
	class Transformer {
	public:
		static void translate(vec3 amount, mat4& transform);
		static void applyYaw(double yaw, mat4& transform);
		static void applyPitch(double pitch, mat4& transform);
	};
}

#endif