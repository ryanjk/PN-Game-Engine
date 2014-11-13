#include "PN/ConcreteStates/InitialState.h"
#include "PN/ConcreteStates/SecondState.h"

#include "PN/GameState/GameStateManager.h"

#include "PN/ECS/Component/TransformComponent.h"

#include "PN/Input/InputManager.h"

#include <iostream>

pn::InitialState::InitialState(const pn::PString& stateFilename) : pn::GameState(stateFilename) {

}

void pn::InitialState::update(double dt) {
	static double time = 0.0;
	time += dt;

	/*if (time > 5) {
		time = 0;
		this->shutdown();
		pn::GameStateManager::g_gameStateManager.setState(std::make_shared<pn::SecondState>("second.state"));
	} */
}

void pn::InitialState::startUpAssist() {
	
	auto& player = getEntity("player");
	auto playerBody = std::dynamic_pointer_cast<pn::TransformComponent>(player.getComponent(pn::ComponentType::TRANSFORM));
	auto& handler = pn::InputManager::g_inputManager.getInputHandler();
	firstPersonListener = std::make_shared<pn::FirstPersonListener>(playerBody);
	handler->addListener(firstPersonListener);

	mm::setCursor(false);
}

void pn::InitialState::shutdownAssist() {
	auto& handler = pn::InputManager::g_inputManager.getInputHandler();
	handler->removeListener(firstPersonListener);

	mm::setCursor(true);
}