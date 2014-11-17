#include "PN/ECS/Entity/Entity.h"
#include "PN/ECS/Component/TransformComponent.h"

#include <iostream>
#include <bitset>
#include <algorithm>

pn::Entity::Entity() : m_ID(), m_name(), m_children(), m_components(), m_key(), m_parent(), alive(false)
{}

pn::Entity::Entity(const pn::PString& name) : m_ID(name.getHash()), m_name(name), m_key(0), alive(true), m_children(), m_components(), m_parent()
{}

pn::Entity::~Entity() 
{}

pn::PString pn::Entity::getName() const {
	return m_name;
}

EntityID pn::Entity::getID() const {
	return m_ID;
}

EntityKey pn::Entity::getKey() const {
	return m_key;
}

bool pn::Entity::hasComponents(unsigned int types) const {
	return ((m_key & types) == types);
}

Component pn::Entity::getComponent(ComponentType componentType) const {
	auto findComponent = m_components.find(componentType);
	if (findComponent != m_components.end()) {
		return findComponent->second;
	}
	else {
		return nullptr;
	}
}

void pn::Entity::addComponent(Component component) {
	m_components.insert({component->getType(), component});
	component->setOwner(m_ID);
	m_key |= component->getType();
}

void pn::Entity::removeComponent(Component component) {
	auto component_pos = m_components.find(component->getType());
	if (component_pos != m_components.end()) {
		m_components.erase(component_pos);
	}
	m_key &= ~component->getType();
}

void pn::Entity::setParent(EntityID parentID) {
	m_parent = parentID;
}

EntityID pn::Entity::getParent() const {
	return m_parent;
}

const std::vector<EntityID>& pn::Entity::getChildrenID() const {
	return m_children;
}

void pn::Entity::addChild(EntityID child) {
	m_children.push_back(child);
}

void pn::Entity::removeChild(EntityID child) {
	std::remove_if(m_children.begin(), m_children.end(), [=](EntityID e) -> bool {return e == child; });
}