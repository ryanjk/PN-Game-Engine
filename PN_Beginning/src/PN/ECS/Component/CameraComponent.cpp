#include "PN/ECS/Component/CameraComponent.h"

std::shared_ptr<pn::CameraComponent> pn::CameraComponent::make(const ComponentData& data) {
	std::shared_ptr<CameraComponent> component = std::make_shared<CameraComponent>();

	auto rotationData = data["rotation"];
	if (!rotationData.isNull()) {
		component->m_rotation = vec3(rotationData[0].asFloat(), rotationData[1].asFloat(), rotationData[2].asFloat());
	}

	auto translationData = data["translation"];
	if (!translationData.isNull()) {
		component->m_translation = vec3(translationData[0].asFloat(), translationData[1].asFloat(), translationData[2].asFloat());
	}

	return component;
}

pn::CameraComponent::CameraComponent() {};

pn::ComponentType pn::CameraComponent::getType() const {
	return pn::ComponentType::CAMERA;
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

