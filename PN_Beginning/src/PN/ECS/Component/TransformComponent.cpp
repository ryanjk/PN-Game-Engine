#include "PN/ECS/Component/TransformComponent.h"

#include "glm/gtx/euler_angles.hpp"

std::shared_ptr<pn::TransformComponent> pn::TransformComponent::make(const ComponentData& data) {
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

	component->updateTransformMatrix();

	return component;
}

pn::TransformComponent::TransformComponent() {}

pn::ComponentType pn::TransformComponent::getType() const {
	return pn::ComponentType::TRANSFORM;
}

mat4 pn::TransformComponent::getTransformMatrix() const {
	return m_transformMatrix;
}

void pn::TransformComponent::updateTransformMatrix() {
	m_transformMatrix =
		glm::translate(mat4(), m_translation) *
		glm::eulerAngleYXZ(m_rotation[1], m_rotation[0], m_rotation[2]) *
		glm::scale(mat4(), m_scale);
	
}

vec3 pn::TransformComponent::getTranslation() const {
	return m_translation;
}

vec3 pn::TransformComponent::getScale() const {
	return m_scale;
}

vec3 pn::TransformComponent::getRotation() const {
	return m_rotation;
}

void pn::TransformComponent::translate(vec3 by) {
	m_translation += by;
	m_transformMatrix[3] = vec4(m_translation.x, m_translation.y, m_translation.z, 1.0f);
}

void pn::TransformComponent::setTranslation(vec3 translation) {
	m_translation = translation;
	m_transformMatrix[3] = vec4(translation.x, translation.y, translation.z, 1.0f);
}

void pn::TransformComponent::scale(vec3 by) {
	m_scale *= by;
	updateTransformMatrix();
}

void pn::TransformComponent::setScale(vec3 scale) {
	m_scale = scale;
	updateTransformMatrix();
}

void pn::TransformComponent::rotate(vec3 angles) {
	m_rotation += angles;
	updateTransformMatrix();
}

void pn::TransformComponent::setRotation(vec3 rotation) {
	m_rotation = rotation;
	updateTransformMatrix();
}