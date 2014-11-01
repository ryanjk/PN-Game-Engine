#ifndef PN_ICOMPONENT_H
#define PN_ICOMPONENT_H

#include "json/json.h"

#include <memory>

using EntityID = unsigned int;
using ComponentData = Json::Value;

namespace pn {
	enum ComponentType {
		NONE = 0,
		TRANSFORM = 1,
		RENDER = 2
	};

	class IComponent {
	public:
		virtual ComponentType getType() const = 0;
		
		static ComponentType textToType(std::string type);
		static std::shared_ptr<pn::IComponent> make(const ComponentData& data, std::string componentName);

		EntityID getOwner() const {
			return m_owner;
		}

		void setOwner(EntityID entity) {
			m_owner = entity;
		}

	private:
		EntityID m_owner;
	};
}

#endif