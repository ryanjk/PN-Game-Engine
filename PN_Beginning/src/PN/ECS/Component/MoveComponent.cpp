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

	auto angularVelocityData = data["angularVelocity"];
	if (!angularVelocityData.isNull()) {
		component->m_angularVelocity = vec3(angularVelocityData[0].asFloat(), angularVelocityData[1].asFloat(), angularVelocityData[2].asFloat());
	}

	auto angularAccelerationData = data["angularAcceleration"];
	if (!angularAccelerationData.isNull()) {
		component->m_angularAcceleration = vec3(angularAccelerationData[0].asFloat(), angularAccelerationData[1].asFloat(), angularAccelerationData[2].asFloat());
	}

	return component;
}

pn::MoveComponent::MoveComponent() {};

pn::ComponentType pn::MoveComponent::getType() const {
	return pn::ComponentType::MOVE;
}

const vec3& pn::MoveComponent::getAcceleration() const {
	return m_acceleration;
}

void pn::MoveComponent::setAcceleration(const vec3& acceleration) {
	m_acceleration = acceleration;
}

const vec3& pn::MoveComponent::getVelocity() const {
	return m_velocity;
}

void pn::MoveComponent::setVelocity(const vec3& velocity) {
	m_velocity = velocity;
}

const vec3& pn::MoveComponent::getAngularVelocity() const {
	return m_angularVelocity;
}

void pn::MoveComponent::setAngularVelocity(const vec3& angularVelocity) {
	m_angularVelocity = angularVelocity;
}

const vec3& pn::MoveComponent::getAngularAcceleration() const {
	return m_angularAcceleration;
}

void pn::MoveComponent::setAngularAcceleration(const vec3& angularAcceleration) {
	m_angularAcceleration = angularAcceleration;
}

