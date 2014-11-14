#include "PN/ConcreteStates/LoadingState.h"
#include "PN/ConcreteStates/InitialState.h"

#include "PN/GameState/GameStateManager.h"

#include "PN/Render/RenderFactory.h"

#include "PN/Util/ResourceManager.h"
#include "PN/Util/FileType.h"

#include <iostream>
#include <thread>

pn::LoadingState::LoadingState(pn::PString stateFilename) : pn::GameState(stateFilename) {
	
}

void pn::LoadingState::startUpAssist() {

}

void pn::LoadingState::setStateToLoad(std::shared_ptr<pn::GameState> stateToLoad) {
	m_stateBeingLoaded = stateToLoad;

	m_startLoadTime = mm::getTime();

	stateToLoad->loadResourcesCommand(m_loadingCommands);
	stateToLoad->loadEntitiesCommand(m_loadingCommands);

	m_totalCommands = m_loadingCommands.size();
	m_commandsLoadedSoFar = 0;

}

void pn::LoadingState::update(double dt) {
	static double time = 0.0;
	time += dt;

	if (m_loadingCommands.size() > 0) {
		auto command = m_loadingCommands[0];
		command->execute();
		m_loadingCommands.erase(m_loadingCommands.begin());

		m_commandsLoadedSoFar += 1;

		auto& loading_icon = getEntity("loading_indicator");
		auto& transform = std::dynamic_pointer_cast<pn::TransformComponent>(loading_icon.getComponent(pn::ComponentType::TRANSFORM));
		float s = (float)m_commandsLoadedSoFar / m_totalCommands;
		transform->setScale({ s, s, s });
	}

	else {
		m_stateBeingLoaded->startUpSystems();
		m_stateBeingLoaded->startUpAssist();
		m_stateBeingLoaded->m_loaded = true;

		std::cout << "Time to load initial state " << mm::getTime() - m_startLoadTime << std::endl;

		pn::GameStateManager::g_gameStateManager.m_currentState = m_stateBeingLoaded;

		m_stateBeingLoaded = nullptr;
	}
}
