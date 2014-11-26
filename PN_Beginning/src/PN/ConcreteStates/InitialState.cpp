#include "PN/ConcreteStates/InitialState.h"
#include "PN/ConcreteStates/SecondState.h"

#include "PN/GameState/GameStateManager.h"

#include "PN/ECS/Component/MoveComponent.h"

#include "PN/Input/InputManager.h"

#include <iostream>

pn::InitialState::InitialState(const pn::PString& stateFilename) : pn::GameState(stateFilename) {

}

void pn::InitialState::update(double dt) {
	static double time = 0.0;
	time += dt;

	// update player position
	auto& player = getEntity("player");

	m_physicsSystem.updateEntity(player, dt);
	auto& transformComponent = std::dynamic_pointer_cast<pn::TransformComponent>(player.getComponent(pn::ComponentType::TRANSFORM));
	m_physicsSystem.setTranslation(*transformComponent, transformComponent->getTranslation() * vec3(1.0f, 0.0f, 1.0f));

//	auto& monkey = getEntity("monkey2");

//	auto& monkey_transform = std::dynamic_pointer_cast<pn::TransformComponent>(monkey.getComponent(ComponentType::TRANSFORM));
//	m_physicsSystem.scale(*monkey_transform, vec3(1.0001f, 1.0001f, 1.0001f));

/*	if (time > 5.0f) {
		time = 0;
		this->shutdown();
		pn::GameStateManager::g_gameStateManager.setState(std::make_shared<pn::SecondState>("second.state"));
	}
	*/
}

void pn::InitialState::startUpAssist() {
	
	auto& player = getEntity("player");
	auto& handler = pn::InputManager::g_inputManager.getInputHandler();
	firstPersonListener = std::make_shared<pn::FirstPersonListener>(&player, &m_physicsSystem);
	handler->addListener(firstPersonListener);

	mm::setCursor(false);
}

void pn::InitialState::shutdownAssist() {
	auto& handler = pn::InputManager::g_inputManager.getInputHandler();
	handler->removeListener(firstPersonListener);

	mm::setCursor(true);
}