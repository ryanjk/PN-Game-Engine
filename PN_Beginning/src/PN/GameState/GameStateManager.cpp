#include "PN/GameState/GameStateManager.h"
#include "PN/ConcreteStates/LoadingState.h"

#include "PN/Window/WindowManager.h"

#include <assert.h>
#include <iostream>
#include <thread>

pn::GameStateManager pn::GameStateManager::g_gameStateManager;

pn::GameStateManager::GameStateManager() {

}

GameStatePointer pn::GameStateManager::getCurrentState() {
	return m_currentState;
}

void pn::GameStateManager::startUp() {
	// When gamestate manager starts, load the loading screen state first
	// for now, it will exist as long as the game is being played (same screen will be used for loading always)
	m_loadingState = std::make_shared<pn::LoadingState>("loading.state");

	// time the loading of the loading screen resources
	auto t1 = mm::getTime();
	m_loadingState->startUp();
	std::cout << "Time to initialize loading state: " << mm::getTime() - t1 << std::endl;

	// set the state to the loading screen as well
	m_currentState = m_loadingState;
}

void pn::GameStateManager::shutdown() {
	m_currentState->shutdown();
	m_loadingState->shutdown();

	m_currentState = nullptr;
	m_loadingState = nullptr;
}

void pn::GameStateManager::notifyState(double dt) {
	assert(m_currentState);

	m_currentState->update(dt);
}

void pn::GameStateManager::renderState() {
	assert(m_currentState);

	m_currentState->render();
}

void pn::GameStateManager::setState(GameStatePointer state) {
	assert(state);
	
	// if new state isn't currently loaded, let the loading state load it.
	if (!state->isLoaded()) {
		m_loadingState->setStateToLoad(state);
		m_currentState = m_loadingState;
	}
	else {
		m_currentState = state;
	}

}