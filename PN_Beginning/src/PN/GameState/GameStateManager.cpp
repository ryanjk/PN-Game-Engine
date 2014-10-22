#include "PN/GameState/GameStateManager.h"
#include "PN/ConcreteStates/LoadingState.h"

#include "PN/Window/WindowManager.h"

#include <assert.h>
#include <iostream>
#include <thread>

pn::GameStateManager pn::GameStateManager::g_gameStateManager;

pn::GameStateManager::GameStateManager() {

}

void pn::GameStateManager::startUp() {
	m_loadingState = std::make_shared<pn::LoadingState>("loading.state");
	auto t1 = glfwGetTime();
	m_loadingState->startUp();
	std::cout << "Time to initialize loading state: " << glfwGetTime() - t1 << std::endl;
	m_currentState = m_loadingState;
}

void pn::GameStateManager::shutdown() {
	m_currentState->shutdown();
}

void pn::GameStateManager::beginLoad() {
	
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
	
	auto t1 = glfwGetTime();
	beginLoad();
	state->startUp();
	m_currentState = state;
	std::cout << "Time to initialize state: " << glfwGetTime() - t1 << std::endl;


}