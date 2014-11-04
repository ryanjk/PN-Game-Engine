#include "PN/ECS/Component/LightComponent.h"

std::shared_ptr<pn::LightComponent> pn::LightComponent::make(const ComponentData& data, pn::ResourceManager& resources) {
	std::shared_ptr<LightComponent> component = std::make_shared<LightComponent>();

	auto colourData = data["colour"];
	if (!colourData.isNull()) {
		component->m_colour = vec3(colourData[0].asFloat(), colourData[1].asFloat(), colourData[2].asFloat());
	}

	auto innerRadiansData = data["innerRadians"];
	if (!innerRadiansData.isNull()) {
		component->m_innerRadians = innerRadiansData.asFloat();
	}

	auto intensityData = data["intensity"];
	if (!intensityData.isNull()) {
		component->m_intensity = intensityData.asFloat();
	}

	auto lightTypeData = data["lightType"];
	if (!lightTypeData.isNull()) {
		component->m_lightType = lightTypeData.asInt();
	}

	auto maxRadiusData = data["maxRadius"];
	if (!maxRadiusData.isNull()) {
		component->m_maxRadius = maxRadiusData.asFloat();
	}

	auto outerRadiansData = data["outerRadians"];
	if (!outerRadiansData.isNull()) {
		component->m_outerRadians = outerRadiansData.asFloat();
	}

	return component;
}

pn::LightComponent::LightComponent() {};

pn::ComponentType pn::LightComponent::getType() const {
	return pn::ComponentType::LIGHT;
}

vec3 pn::LightComponent::getColour() const {
	return m_colour;
}

void pn::LightComponent::setColour(vec3 colour) {
	m_colour = colour;
}

float pn::LightComponent::getInnerRadians() const {
	return m_innerRadians;
}

void pn::LightComponent::setInnerRadians(float innerRadians) {
	m_innerRadians = innerRadians;
}

float pn::LightComponent::getIntensity() const {
	return m_intensity;
}

void pn::LightComponent::setIntensity(float intensity) {
	m_intensity = intensity;
}

int pn::LightComponent::getLightType() const {
	return m_lightType;
}

void pn::LightComponent::setLightType(int lightType) {
	m_lightType = lightType;
}

float pn::LightComponent::getMaxRadius() const {
	return m_maxRadius;
}

void pn::LightComponent::setMaxRadius(float maxRadius) {
	m_maxRadius = maxRadius;
}

float pn::LightComponent::getOuterRadians() const {
	return m_outerRadians;
}

void pn::LightComponent::setOuterRadians(float outerRadians) {
	m_outerRadians = outerRadians;
}

