#include "PN/ECS/Component/IComponent.h"
#include "PN/ECS/Component/TransformComponent.h"
#include "PN/ECS/Component/RenderComponent.h"
#include "PN/ECS/Component/LightComponent.h"

#include <iostream>

pn::ComponentType pn::IComponent::textToType(std::string type) {
	if (type == "transform" || type == "Transform") {
		return pn::ComponentType::TRANSFORM;
	}

	else if (type == "render" || type == "Render") {
		return pn::ComponentType::RENDER;
	}

	else if (type == "light" || type == "Light") {
		return pn::ComponentType::LIGHT;
	}

	else {
		return pn::ComponentType::NONE;
	}
}

std::shared_ptr<pn::IComponent> pn::IComponent::make(const ComponentData& componentData, std::string componentName) {
	switch (textToType(componentName)) {
	case pn::ComponentType::TRANSFORM:
		return pn::TransformComponent::make(componentData);
	case pn::ComponentType::RENDER:
		return pn::RenderComponent::make(componentData);
	case pn::ComponentType::LIGHT:
		return pn::LightComponent::make(componentData);
	default:
		std::cout << "Could not create component " + componentName + " -- couldn't recognize type (check name in entity definition)" << std::endl;
		return nullptr;
	}

}