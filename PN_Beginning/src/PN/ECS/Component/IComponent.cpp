#include "PN/ECS/Component/IComponent.h"
#include "PN/ECS/Component/TransformComponent.h"
#include "PN/ECS/Component/RenderComponent.h"
#include "PN/ECS/Component/LightComponent.h"
#include "PN/ECS/Component/MoveComponent.h"

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

	else if (type == "move" || type == "Move") {
		return pn::ComponentType::MOVE;
	}

	else {
		return pn::ComponentType::NONE;
	}
}

std::shared_ptr<pn::IComponent> pn::IComponent::make(const ComponentData& componentData, std::string componentName, pn::ResourceManager& resources) {
	switch (textToType(componentName)) {
	case pn::ComponentType::TRANSFORM:
		return pn::TransformComponent::make(componentData, resources);
	case pn::ComponentType::RENDER:
		return pn::RenderComponent::make(componentData, resources);
	case pn::ComponentType::LIGHT:
		return pn::LightComponent::make(componentData, resources);
	case pn::ComponentType::MOVE:
		return pn::MoveComponent::make(componentData, resources);
	default:
		std::cout << "Could not create component " + componentName + " -- couldn't recognize type (check name in entity definition)" << std::endl;
		return nullptr;
	}

}