#ifndef PN_ENTITY_H
#define PN_ENTITY_H

#include "json/json.h"

#include "PN/ECS/Component/IComponent.h"

#include "PN/Util/PString.h"
#include "PN/Util/Math.h"

#include <memory>

using EntityID = unsigned int;
using EntityKey = unsigned int;
using EntityData = Json::Value;

using Component = std::shared_ptr < pn::IComponent >;
using Components = std::map < pn::ComponentType, Component >;

namespace pn {
	class Entity {
	public:
		static std::shared_ptr<Entity> makeEntity(const EntityData& entityData, pn::PString name, EntityID parent);

		Entity(pn::PString name);
		~Entity();

		EntityID getID() const;
		EntityKey getKey() const;

		EntityID getParent() const;

		Component getComponent(ComponentType componentType) const;

		void addComponent(Component component);
		void removeComponent(Component component);

		bool alive;

	private:
		EntityID m_ID;
		EntityID m_parent;

		pn::PString m_name;
		EntityKey m_key;

		Components m_components;

	};
}

#endif