#include "PN/ECS/Component/RenderComponent.h"

#include "PN/Util/ResourceManager.h"

std::shared_ptr<pn::RenderComponent> pn::RenderComponent::make(const ComponentData& data, pn::ResourceManager& resources) {
	std::shared_ptr<RenderComponent> component = std::make_shared<RenderComponent>();

	auto ambientData = data["ambient"];
	if (!ambientData.isNull()) {
		component->m_ambient = vec4(ambientData[0].asFloat(), ambientData[1].asFloat(), ambientData[2].asFloat(), ambientData[3].asFloat());
	}

	auto diffuseData = data["diffuse"];
	if (!diffuseData.isNull()) {
		resources.load(diffuseData.asString());
		component->m_diffuse = diffuseData.asString();
	}

	auto glossData = data["gloss"];
	if (!glossData.isNull()) {
		component->m_gloss = glossData.asFloat();
	}

	auto meshData = data["mesh"];
	if (!meshData.isNull()) {
		resources.load(meshData.asString());
		component->m_mesh = meshData.asString();
	}
	else {
	//	pn::ResourceManager::g_resourceManager.load("default.obj");
	}

	auto shaderProgramData = data["shaderProgram"];
	if (!shaderProgramData.isNull()) {
		resources.load(shaderProgramData.asString());
		component->m_shaderProgram = shaderProgramData.asString();
	}
	else {
		resources.load("default.sp");
	}

	auto specularData = data["specular"];
	if (!specularData.isNull()) {
		component->m_specular = vec4(specularData[0].asFloat(), specularData[1].asFloat(), specularData[2].asFloat(), specularData[3].asFloat());
	}

	return component;
}

pn::RenderComponent::RenderComponent() {};

pn::ComponentType pn::RenderComponent::getType() const {
	return pn::ComponentType::RENDER;
}

const vec4& pn::RenderComponent::getAmbient() const {
	return m_ambient;
}

void pn::RenderComponent::setAmbient(const vec4& ambient) {
	m_ambient = ambient;
}

const pn::PString& pn::RenderComponent::getDiffuse() const {
	return m_diffuse;
}

void pn::RenderComponent::setDiffuse(const pn::PString& diffuse) {
	m_diffuse = diffuse;
}

float pn::RenderComponent::getGloss() const {
	return m_gloss;
}

void pn::RenderComponent::setGloss(float gloss) {
	m_gloss = gloss;
}

const pn::PString& pn::RenderComponent::getMesh() const {
	return m_mesh;
}

void pn::RenderComponent::setMesh(const pn::PString& mesh) {
	m_mesh = mesh;
}

const pn::PString& pn::RenderComponent::getShaderProgram() const {
	return m_shaderProgram;
}

void pn::RenderComponent::setShaderProgram(const pn::PString& shaderProgram) {
	m_shaderProgram = shaderProgram;
}

const vec4& pn::RenderComponent::getSpecular() const {
	return m_specular;
}

void pn::RenderComponent::setSpecular(const vec4& specular) {
	m_specular = specular;
}

