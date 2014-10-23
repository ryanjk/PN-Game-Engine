#ifndef PN_ICOMPONENT_H
#define PN_ICOMPONENT_H

#include "json/json.h"

using ID = int;
using ComponentData = Json::Value;

namespace pn {
	enum ComponentType {
		Transform = 1,
		Camera = 2,
		Render = 4
	};

	class IComponent {
	public:
		virtual ComponentType getType() const = 0;
		
		ID getOwner() const {
			return m_owner;
		}

		void setOwner(ID entity) {
			m_owner = entity;
		}

	private:
		ID m_owner;
	};
}

#endif