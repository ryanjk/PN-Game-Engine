#include "PN/ECS/Component/TransformComponent.h"

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
		glm::rotate(mat4(), m_rotation[0], vec3(1.0f, 0.0f, 0.0f)) *
		glm::rotate(mat4(), m_rotation[1], vec3(0.0f, 1.0f, 0.0f)) *
		glm::rotate(mat4(), m_rotation[2], vec3(0.0f, 0.0f, 1.0f)) *
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

void pn::TransformComponent::setTranslation(vec3 translation) {
	m_translation = translation;
	m_transformMatrix[3] = vec4(translation.x, translation.y, translation.z, 1.0f);
}

void pn::TransformComponent::setScale(vec3 scale) {
	m_scale = scale;
	updateTransformMatrix();
}

void pn::TransformComponent::setRotation(vec3 rotation) {
	m_rotation = rotation;
	updateTransformMatrix();
}