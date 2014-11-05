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

		Entity();
		Entity(const pn::PString& name);
		~Entity();

		Entity& operator=(const Entity& entity) = delete;

		pn::PString getName() const;
		EntityID getID() const;
		EntityKey getKey() const;

		void setParent(EntityID parentID);
		EntityID getParent() const;

		const std::vector<EntityID>& getChildrenID() const;

		Component getComponent(ComponentType componentType) const;

		void addComponent(Component component);
		void removeComponent(Component component);

		void addChild(EntityID child);
		void removeChild(EntityID child);

		bool alive;

	private:
		EntityID m_ID;
		pn::PString m_name;

		EntityID m_parent;
		std::vector<EntityID> m_children;

		Components m_components;
		EntityKey m_key;
	};
}

#endif