#ifndef PN_ICOMPONENT_H
#define PN_ICOMPONENT_H

#include "PN/ECS/Entity/Entity.h"

using ComponentKey = int;

namespace pn {
	class IComponent {
	public:

		void setOwner(ID entity) {
			m_owner = entity;
		}

		ID getOwner() const {
			return m_owner;
		}

		virtual ComponentKey getComponentKey() const = 0;

	private:
		ID m_owner;
	};
}

#endif