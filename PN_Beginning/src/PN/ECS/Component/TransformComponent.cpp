#include "PN/GameState/GameStateManager.h"

#include "PN/ECS/Component/TransformComponent.h"

#include "PN/Util/PString.h"

#include "glm/gtx/euler_angles.hpp"

#include <iostream>

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

	component->updateTransformMatrix();

	return component;
}

pn::TransformComponent::TransformComponent() {}

pn::ComponentType pn::TransformComponent::getType() const {
	return pn::ComponentType::TRANSFORM;
}

const mat4& pn::TransformComponent::getTransformMatrix() const {
	return m_transformMatrix;
}

const mat4& pn::TransformComponent::getWorldTransformMatrix() const {
	return pn::GameStateManager::g_gameStateManager.getCurrentState()->getEntityWorldTransform(getOwner());
}

void pn::TransformComponent::updateTransformMatrix() {
	m_transformMatrix =
		glm::translate(mat4(), m_translation) *
		glm::eulerAngleYXZ(m_rotation[1], m_rotation[0], m_rotation[2]) *
		glm::scale(mat4(), m_scale);
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

void pn::TransformComponent::translateWorld(const vec3& by) {
	m_translation += by;
	m_transformMatrix[3] = vec4(m_translation.x, m_translation.y, m_translation.z, 1.0f);
}

void pn::TransformComponent::translateLocal(const vec3& by) {
	auto pitch = make_quat({ 1.0f, 0.0f, 0.0f }, m_rotation[0]);
	auto yaw = make_quat({ 0.0f, 1.0f, 0.0f }, m_rotation[1]);
	auto roll = make_quat({ 0.0f, 0.0f, 1.0f }, m_rotation[2]);
	
	vec3 rotatedDirection = yaw * pitch * roll * by;
	translateWorld(rotatedDirection);
}

void pn::TransformComponent::setTranslation(const vec3& translation) {
	m_translation = translation;
	m_transformMatrix[3] = vec4(translation.x, translation.y, translation.z, 1.0f);
}

void pn::TransformComponent::scale(const vec3& by) {
	m_scale *= by;
	updateTransformMatrix();
}

void pn::TransformComponent::setScale(const vec3& scale) {
	m_scale = scale;
	updateTransformMatrix();
}

void pn::TransformComponent::rotateLocal(const vec3& eulerAngles) {
	m_rotation += eulerAngles;

	rotateParent(eulerAngles, m_translation);
}

void pn::TransformComponent::rotateParent(const vec3& eulerAngles, const vec3& point) {
	m_transformMatrix[3].xyz = m_transformMatrix[3].xyz - point;
	m_transformMatrix = glm::eulerAngleYXZ(eulerAngles[1], eulerAngles[0], eulerAngles[2]) * m_transformMatrix;
	m_transformMatrix[3].xyz = m_transformMatrix[3].xyz + point;

	m_translation = m_transformMatrix[3].xyz;
}

void pn::TransformComponent::rotatePitchLocal(float pitch) {
	m_rotation[0] += pitch;

	rotatePitchParent(pitch, m_transformMatrix[3].xyz);
}

void pn::TransformComponent::rotatePitchParent(float pitch, const vec3& point) {
	m_transformMatrix[3].xyz = m_transformMatrix[3].xyz - point;
	m_transformMatrix = glm::rotate(mat4(), pitch, vec3(1.0f, 0.0f, 0.0f)) * m_transformMatrix;
	m_transformMatrix[3].xyz = m_transformMatrix[3].xyz + point;

	m_translation = m_transformMatrix[3].xyz; 

	updateTransformMatrix();
}

void pn::TransformComponent::rotateYawLocal(float yaw) {
	m_rotation[1] += yaw;

	rotateYawParent(yaw, m_transformMatrix[3].xyz);
}

void pn::TransformComponent::rotateYawParent(float yaw, const vec3& point) {
	m_transformMatrix[3].xyz = m_transformMatrix[3].xyz - point;
	m_transformMatrix = glm::rotate(mat4(), yaw, vec3(0.0f, 1.0f, 0.0f)) * m_transformMatrix;
	m_transformMatrix[3].xyz = m_transformMatrix[3].xyz + point;

	m_translation = m_transformMatrix[3].xyz; 
	updateTransformMatrix();
}

void pn::TransformComponent::rotateRollLocal(float roll) {
	m_rotation[2] += roll;

	rotateRollParent(roll, m_translation);
}

void pn::TransformComponent::rotateRollParent(float roll, const vec3& point) {
	m_transformMatrix[3].xyz = m_transformMatrix[3].xyz - point;
	m_transformMatrix = glm::rotate(mat4(), roll, vec3(0.0f, 0.0f, 1.0f)) * m_transformMatrix;
	m_transformMatrix[3].xyz = m_transformMatrix[3].xyz + point;

	m_translation = m_transformMatrix[3].xyz;
	updateTransformMatrix();
}

void pn::TransformComponent::setRotation(const vec3& rotation) {
	m_rotation = rotation;

	updateTransformMatrix();
}