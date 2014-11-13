#include "PN/ConcreteStates/SecondState.h"
#include "PN/ConcreteStates/InitialState.h"

#include "PN/GameState/GameStateManager.h"


pn::SecondState::SecondState(const pn::PString& stateFilename) : pn::GameState(stateFilename) {

}

void pn::SecondState::update(double dt) {
	static double time = 0.0;
	time += dt;

	/*if (time > 5) {
		time = 0;
		this->shutdown();
		pn::GameStateManager::g_gameStateManager.setState(std::make_shared<pn::InitialState>("initial.state"));
	} */

}