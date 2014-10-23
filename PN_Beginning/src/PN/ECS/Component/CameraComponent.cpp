#include "PN/ECS/Component/CameraComponent.h"

pn::CameraComponent pn::CameraComponent::make(const ComponentData& data) {
	CameraComponent component;

	auto fovData = data["fov"];
	if (!fovData.isNull()) {
		component.m_fov = fovData.asBool();
	}

	auto rotationData = data["rotation"];
	if (!rotationData.isNull()) {
		component.m_rotation = vec3(rotationData[0].asFloat(), rotationData[1].asFloat(), rotationData[2].asFloat());
	}

	auto translationData = data["translation"];
	if (!translationData.isNull()) {
		component.m_translation = vec3(translationData[0].asFloat(), translationData[1].asFloat(), translationData[2].asFloat());
	}

	return component;
}

pn::ComponentType pn::CameraComponent::getType() const {
	return pn::ComponentType::Camera;
}

bool pn::CameraComponent::getFov() const {
	return m_fov;
}

void pn::CameraComponent::setFov(bool fov) {
	m_fov = fov;
}

vec3 pn::CameraComponent::getRotation() const {
	return m_rotation;
}

void pn::CameraComponent::setRotation(vec3 rotation) {
	m_rotation = rotation;
}

vec3 pn::CameraComponent::getTranslation() const {
	return m_translation;
}

void pn::CameraComponent::setTranslation(vec3 translation) {
	m_translation = translation;
}

