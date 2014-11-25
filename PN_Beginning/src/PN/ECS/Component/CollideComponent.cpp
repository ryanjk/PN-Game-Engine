#include "PN/ECS/Component/CollideComponent.h"

std::shared_ptr<pn::CollideComponent> pn::CollideComponent::make(const ComponentData& data, pn::ResourceManager& resources) {
	std::shared_ptr<CollideComponent> component = std::make_shared<CollideComponent>();

	auto containerTypeData = data["containerType"];
	if (!containerTypeData.isNull()) {
		int containerType = containerTypeData.asInt();
		component->m_containerType = (BoundingContainerType) containerType;
	}

	auto scaleData = data["scale"];
	if (!scaleData.isNull()) {
		component->m_scale = scaleData.asFloat();
	}

	return component;
}

pn::CollideComponent::CollideComponent() {};

pn::ComponentType pn::CollideComponent::getType() const {
	return pn::ComponentType::COLLIDE;
}

pn::BoundingContainerType pn::CollideComponent::getContainerType() const {
	return m_containerType;
}

void pn::CollideComponent::setContainerType(BoundingContainerType containerType) {
	m_containerType = containerType;
}

float pn::CollideComponent::getScale() const {
	return m_scale;
}

