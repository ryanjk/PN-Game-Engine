#include "PN/ECS/Component/TransformComponent.h"

#include "PN/Util/PString.h"

#include <iostream>

#include "glm/gtx/euler_angles.hpp"

std::shared_ptr<pn::TransformComponent> pn::TransformComponent::make(const ComponentData& data, pn::ResourceManager& resources) {

	auto component = std::make_shared<TransformComponent>();

	auto translationData = data["translation"];
	if (!translationData.isNull()) {
		component->m_translation = vec3(translationData[0].asFloat(), translationData[1].asFloat(), translationData[2].asFloat());
	}
	
	auto scaleData = data["scale"];
	if (!scaleData.isNull()) {
		component->m_scale = vec3(scaleData[0].asFloat(), scaleData[1].asFloat(), scaleData[2].asFloat());
	}

	auto rotationData = data["rotation"];
	if (!rotationData.isNull()) {
		component->m_rotation = vec3(rotationData[0].asFloat(), rotationData[1].asFloat(), rotationData[2].asFloat());
	}

	component->m_transformMatrix =
		glm::translate(mat4(), component->m_translation) *
		glm::eulerAngleYXZ(component->m_rotation[1], component->m_rotation[0], component->m_rotation[2]) *
		glm::scale(mat4(), component->m_scale);

	return component;
}

pn::TransformComponent::TransformComponent() {}

pn::ComponentType pn::TransformComponent::getType() const {
	return pn::ComponentType::TRANSFORM;
}

const mat4& pn::TransformComponent::getTransformMatrix() const {
	return m_transformMatrix;
}

const vec3& pn::TransformComponent::getTranslation() const {
	return m_translation;
}

const vec3& pn::TransformComponent::getScale() const {
	return m_scale;
}

const vec3& pn::TransformComponent::getRotation() const {
	return m_rotation;
}

