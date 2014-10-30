#include "PN/GameState/GameState.h"

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
Parse the *.state file and load resources via ResourceManager singleton 
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
			pn::ResourceManager::g_resourceManager.load(resourceFilename.asString());
		}
	}

}

void pn::GameState::releaseResources() {
	auto& resourceTree = m_root["resource"];

	for (auto& resource : resourceTree) {
		for (auto& resourceFilename : resource) {
			pn::ResourceManager::g_resourceManager.remove(resourceFilename.asString());
		}
	}
}

void pn::GameState::loadEntities() {
	auto& entityTree = m_root["entities"];

	auto num_entities = entityTree.size();
	m_entities.reserve(num_entities);

	for (auto& entity : entityTree.getMemberNames()) {
		auto new_entity = pn::Entity::makeEntity(entityTree[entity], entity);
		m_entities.push_back(new_entity);
	}

	#ifdef _DEBUG
	for (size_t i = 0; i < m_entities.size(); i++) {
		for (size_t j = i + 1; j < m_entities.size(); j++) {
			assert(m_entities[i]->getID() != m_entities[j]->getID() && "Two entities share an ID\n");
		}
	}
	#endif
}

void pn::GameState::releaseEntities() {

}

Entities& pn::GameState::getEntities() {
	return m_entities;
}

EntityPointer pn::GameState::getEntity(const pn::PString& entity_name) {
	auto e_itr = std::find_if(m_entities.begin(), m_entities.end(), [&](EntityPointer e) -> bool {return e->getID() == entity_name.getHash(); });
	assert((e_itr != m_entities.end()) && "No entity found\n");
	return (*e_itr);
}

void pn::GameState::render() {}

void pn::GameState::startUpAssist() {}
void pn::GameState::shutdownAssist() {}