#include "PN/ConcreteStates/SecondState.h"
#include "PN/ConcreteStates/InitialState.h"

#include "PN/GameState/GameStateManager.h"

#include "PN/Input/InputManager.h"


pn::SecondState::SecondState(const pn::PString& stateFilename) : pn::GameState(stateFilename) {

}

void pn::SecondState::update(double dt) {
	static double time = 0.0;
	time += dt;

	// update player position
	auto& player = getEntity("player");

	m_physicsSystem.updateEntity(player, dt);
	auto& transformComponent = std::dynamic_pointer_cast<pn::TransformComponent>(player.getComponent(pn::ComponentType::TRANSFORM));
	m_physicsSystem.setTranslation(*transformComponent, transformComponent->getTranslation() * vec3(1.0f, 0.0f, 1.0f));

/*	if (time > 5) {
		time = 0;
		this->shutdown();
		pn::GameStateManager::g_gameStateManager.setState(std::make_shared<pn::InitialState>("initial.state"));
	}
*/

}

void pn::SecondState::startUpAssist() {
	auto& player = getEntity("player");
	auto& handler = pn::InputManager::g_inputManager.getInputHandler();
	firstPersonListener = std::make_shared<pn::FirstPersonListener>(&player, &m_physicsSystem);
	handler->addListener(firstPersonListener);

	mm::setCursor(false);
}