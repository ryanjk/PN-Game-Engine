#include "PN/ECS/Entity/Entity.h"
#include "PN/ECS/Component/TransformComponent.h"

#include <iostream>
#include <bitset>

std::shared_ptr<pn::Entity>  pn::Entity::makeEntity(const EntityData& entityData, pn::PString name, EntityID parent) {
	EntityID entity_id = name.getHash();

	auto new_entity = std::make_shared<pn::Entity>(name);
	auto components = entityData["components"];
	
	for (auto& component : components.getMemberNames()) {
		auto new_component = pn::IComponent::make(entityData["components"][component], component);
		new_entity->addComponent(new_component);
	}

	new_entity->m_parent = parent;

	return new_entity;
}

pn::Entity::Entity(pn::PString name) : m_ID(name.getHash()), m_name(name), m_key(0), alive(true)
{}

pn::Entity::~Entity() {
	std::cout << "Entity " << m_name.getText() << " destroyed" << std::endl;
}

EntityID pn::Entity::getID() const {
	return m_ID;
}

EntityKey pn::Entity::getKey() const {
	return m_key;
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

EntityID pn::Entity::getParent() const {
	return m_parent;
}