#include "PN/GameState/GameState.h"

#include <iostream>

static const pn::PString STATE_RESOURCE_FILEPATH = "resource/state/";

pn::GameState::GameState(pn::PString stateFileName) : m_stateFilename(stateFileName)
{}

/*
startUp() and shutdown() use template method pattern -- concrete classes may implement _Assist() method if needed
*/

void pn::GameState::startUp() {
	loadResources();
	startUpAssist();
}

void pn::GameState::shutdown() {
	shutdownAssist();
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

void pn::GameState::render() {}

void pn::GameState::startUpAssist() {}
void pn::GameState::shutdownAssist() {}