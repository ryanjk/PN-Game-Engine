#include "PN/ECS/Component/MoveComponent.h"

std::shared_ptr<pn::MoveComponent> pn::MoveComponent::make(const ComponentData& data, pn::ResourceManager& resources) {
	std::shared_ptr<MoveComponent> component = std::make_shared<MoveComponent>();

	auto accelerationData = data["acceleration"];
	if (!accelerationData.isNull()) {
		component->m_acceleration = vec3(accelerationData[0].asFloat(), accelerationData[1].asFloat(), accelerationData[2].asFloat());
	}

	auto velocityData = data["velocity"];
	if (!velocityData.isNull()) {
		component->m_velocity = vec3(velocityData[0].asFloat(), velocityData[1].asFloat(), velocityData[2].asFloat());
	}

	return component;
}

pn::MoveComponent::MoveComponent() {};

pn::ComponentType pn::MoveComponent::getType() const {
	return pn::ComponentType::MOVE;
}

vec3 pn::MoveComponent::getAcceleration() const {
	return m_acceleration;
}

void pn::MoveComponent::setAcceleration(vec3 acceleration) {
	m_acceleration = acceleration;
}

vec3 pn::MoveComponent::getVelocity() const {
	return m_velocity;
}

void pn::MoveComponent::setVelocity(vec3 velocity) {
	m_velocity = velocity;
}

