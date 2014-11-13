#ifndef ENTITY_LOAD_COMMAND_H
#define ENTITY_LOAD_COMMAND_H

#include "PN/ECS/Entity/Entity.h"
#include "PN/ECS/Component/IComponent.h"

#include "PN/Command/Command.h"

#include "PN/Util/ResourceManager.h"

#include "json/json.h"

#include <string>
#include <iostream>

namespace pn {
	class EntityLoadCommand : public pn::Command {
	public:
		EntityLoadCommand(const Json::Value* componentTree, pn::Entity* entity, pn::ResourceManager* state_resources) :
			m_componentTree(componentTree), m_entity_to_load(entity), m_state_resources(state_resources) {}
		
		void execute() override {
			for (std::string& component : m_componentTree->getMemberNames()) {
				auto new_component = pn::IComponent::make((*m_componentTree)[component], component, *m_state_resources);
				m_entity_to_load->addComponent(new_component);
			}
		}

	private:
		const Json::Value* m_componentTree;
		pn::Entity* m_entity_to_load;
		pn::ResourceManager* m_state_resources;
	};
}

#endif