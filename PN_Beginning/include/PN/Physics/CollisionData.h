#ifndef COLLISION_DATA_H
#define COLLISION_DATA_H

#include "PN/Util/Math.h"
#include "PN/ECS/Entity/Entity.h"

namespace pn {
	struct CollisionData {
		EntityID e1;
		EntityID e2;
		vec3 separatingVector;
	};
}

#endif