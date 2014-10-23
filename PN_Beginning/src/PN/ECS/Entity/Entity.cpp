#include "PN/ECS/Entity/Entity.h"

pn::Entity::Entity(ID id) : m_ID(id) {
	alive = true;
}

ID pn::Entity::getID() const {
	return m_ID;
}

void pn::Entity::addComponent(Component component) {
	m_components.insert({component->getType(), component});
}

void pn::Entity::removeComponent(Component component) {
	auto component_pos = m_components.find(component->getType());
	if (component_pos != m_components.end()) {
		m_components.erase(component_pos);
	}
}