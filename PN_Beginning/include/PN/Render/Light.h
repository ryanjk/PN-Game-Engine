#ifndef PN_LIGHT_H
#define PN_LIGHT_H

#include "PN/Util/Math.h"

namespace pn {
	struct Light {
		vec3 position;
		vec3 direction;
		int type;
		vec3 colour;
		float intensity;
		float innerRadians;
		float outerRadians;
		float maxRadius;
	};
}

#endif