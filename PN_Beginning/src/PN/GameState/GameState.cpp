#include "PN/GameState/GameState.h"

#include "PN/ECS/Component/TransformComponent.h"

#include <iostream>
#include <cassert>
#include <algorithm>

static const pn::PString STATE_RESOURCE_FILEPATH = "resource/state/";

pn::GameState::GameState(pn::PString stateFileName) : m_stateFilename(stateFileName)
{}

/*
startUp() and shutdown() use template method pattern -- concrete classes may implement _Assist() method if needed
*/

void pn::GameState::startUp() {
	loadResources();
	loadEntities();
	startUpAssist();
}

void pn::GameState::shutdown() {
	shutdownAssist();
	releaseEntities();
	releaseResources();
}

/*
Parse the *.state file and load resources (typically ones not initially used by any state entities) via ResourceManager singleton 
*/
void pn::GameState::loadResources() {
	Json::Reader reader;

	std::ifstream state_file;
	state_file.open((STATE_RESOURCE_FILEPATH + m_stateFilename).getText());

	if (!state_file.is_open()) {
		std::cout << "State Load: could not load " << m_stateFilename.getText() << std::endl;
		getchar();
		exit(-1);
	}

	bool success = reader.parse(state_file, m_root);
	if (!success) {
		std::cout << "State Load: could not parse state file: " << reader.getFormattedErrorMessages();
		state_file.close();
		getchar();
		exit(-1);
	}

	state_file.close();

	auto& resourceTree = m_root["resource"];

	for (auto& resource : resourceTree) {
		for (auto& resourceFilename : resource) {
//			pn::ResourceManager::g_resourceManager.load(resourceFilename.asString());
			m_resources.load(resourceFilename.asString());
		}
	}
}

void pn::GameState::releaseResources() {
	m_resources.removeAll();
}

static void dfs(pn::Entity& entity, pn::GameState* state) {
	std::cout << entity.getName() << std::endl;
	for (auto& e : entity.getChildrenID()) {
		std::cout << "Child of " << entity.getName() << ": ";
		dfs(state->getEntity(e), state);
	}
}

void pn::GameState::loadEntities() {
	auto& entityTree = m_root["entities"];
	if (entityTree.isNull()) return; // Leave if there are no entities in this state

	// This is only an estimate for the amount of entities -- doesn't include children
	auto num_entities = entityTree.size();
	m_entities.reserve(num_entities + 1);

	// Create a root entity who will contain all entities in the state as children
	pn::Entity root(this->m_stateFilename);

	loadEntitiesRec(entityTree, root);
	m_entities.push_back(root);

	#ifdef _DEBUG
	for (size_t i = 0; i < m_entities.size(); i++) {
		for (size_t j = i + 1; j < m_entities.size(); j++) {
			assert(m_entities[i].getID() != m_entities[j].getID() && "Two entities share an ID\n");
		}
	}
	#endif

//	dfs(root, this);
}

void pn::GameState::loadEntitiesRec(const Json::Value& current_entity_tree_root, pn::Entity& parent) {
	if (current_entity_tree_root.isNull()) {
		return;
	}

	// Add children entities to the entity group
	for (std::string& entity : current_entity_tree_root.getMemberNames()) {

		pn::Entity new_entity(entity);
		new_entity.setParent(parent.getID());

		EntityID new_entity_id = new_entity.getID();

		parent.addChild(new_entity_id);

		auto& components = current_entity_tree_root[entity]["components"];
		for (std::string& component : components.getMemberNames()) {
			auto new_component = pn::IComponent::make(components[component], component, m_resources);
			new_entity.addComponent(new_component);
		}

		if (!current_entity_tree_root[entity]["children"].isNull()) {
			loadEntitiesRec(current_entity_tree_root[entity]["children"], new_entity);
		}

		m_entities.push_back(new_entity);
	}
}

void pn::GameState::releaseEntities() {
	m_entities.clear();
}

pn::Entity& pn::GameState::getRootEntity() {
	return getEntity(this->m_stateFilename.getHash());
}

pn::Entity& pn::GameState::getEntity(const pn::PString& entity_name) {
	return getEntity(entity_name.getHash());
}

pn::Entity& pn::GameState::getEntity(EntityID entity_id) {
	for (auto& e : m_entities) {
		if (e.getID() == entity_id) {
			return e;
		}
	}

	assert(false && "No entity found");

	return m_entities[0];
}

mat4 pn::GameState::getEntityWorldTransform(EntityID entity_id) {
	static EntityID root = this->m_stateFilename.getHash();

	auto& entity = getEntity(entity_id);

	auto transformComponent = std::dynamic_pointer_cast<pn::TransformComponent>(entity.getComponent(pn::ComponentType::TRANSFORM));
	mat4 world_matrix = transformComponent->getTransformMatrix();

	auto parentID = entity.getParent();
	while (parentID != root) {
		auto& parent = getEntity(parentID);
		auto parentTransform = std::dynamic_pointer_cast<pn::TransformComponent>(parent.getComponent(pn::ComponentType::TRANSFORM));
		mat4 parent_world_matrix = parentTransform->getTransformMatrix();
		world_matrix = parent_world_matrix * world_matrix;

		parentID = parent.getParent();
	}

	return world_matrix;
}

mat4 pn::GameState::getEntityWorldTransform(const pn::PString& entity_name) {
	return getEntityWorldTransform(entity_name.getHash());
}


void pn::GameState::render() {}

void pn::GameState::startUpAssist() {}
void pn::GameState::shutdownAssist() {}