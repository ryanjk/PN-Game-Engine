#include "PN/ECS/Component/CameraComponent.h"

std::shared_ptr<pn::CameraComponent> pn::CameraComponent::make(const ComponentData& data) {
	std::shared_ptr<CameraComponent> component = std::make_shared<CameraComponent>();

	auto atData = data["at"];
	if (!atData.isNull()) {
		component->m_at = vec3(atData[0].asFloat(), atData[1].asFloat(), atData[2].asFloat());
	}

	auto originData = data["origin"];
	if (!originData.isNull()) {
		component->m_origin = vec3(originData[0].asFloat(), originData[1].asFloat(), originData[2].asFloat());
	}

	auto pitchData = data["pitch"];
	if (!pitchData.isNull()) {
		component->m_pitch = pitchData.asDouble();
	}

	auto upData = data["up"];
	if (!upData.isNull()) {
		component->m_up = vec3(upData[0].asFloat(), upData[1].asFloat(), upData[2].asFloat());
	}

	auto yawData = data["yaw"];
	if (!yawData.isNull()) {
		component->m_yaw = yawData.asDouble();
	}

	return component;
}

pn::CameraComponent::CameraComponent() {};

pn::ComponentType pn::CameraComponent::getType() const {
	return pn::ComponentType::CAMERA;
}

vec3 pn::CameraComponent::getAt() const {
	return m_at;
}

void pn::CameraComponent::setAt(vec3 at) {
	m_at = at;
}

vec3 pn::CameraComponent::getOrigin() const {
	return m_origin;
}

void pn::CameraComponent::setOrigin(vec3 origin) {
	m_origin = origin;
}

double pn::CameraComponent::getPitch() const {
	return m_pitch;
}

void pn::CameraComponent::setPitch(double pitch) {
	m_pitch = pitch;
}

vec3 pn::CameraComponent::getUp() const {
	return m_up;
}

void pn::CameraComponent::setUp(vec3 up) {
	m_up = up;
}

double pn::CameraComponent::getYaw() const {
	return m_yaw;
}

void pn::CameraComponent::setYaw(double yaw) {
	m_yaw = yaw;
}

