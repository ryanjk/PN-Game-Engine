#ifndef PN_ENTITY_H
#define PN_ENTITY_H

#include "json/json.h"

#include "PN/ECS/Component/IComponent.h"

#include <memory>

using ID = int;
using EntityData = Json::Value;

using Component = std::shared_ptr < pn::IComponent >;
using Components = std::map < pn::ComponentType, Component >;

namespace pn {
	class Entity {
	public:
		static Entity makeEntity(const EntityData& entityData);
		
		ID getID() const;

		Component getComponent(Component component);
		void addComponent(Component component);
		void removeComponent(Component component);

		bool alive;

	private:
		Entity(ID id);
		ID m_ID;
		Components m_components;

	};
}

#endif