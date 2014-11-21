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
	auto& entity = getEntity("player");

	auto& moveComponent = *std::dynamic_pointer_cast<pn::MoveComponent>(entity.getComponent(pn::ComponentType::MOVE));
	auto& transformComponent = *std::dynamic_pointer_cast<pn::TransformComponent>(entity.getComponent(pn::ComponentType::TRANSFORM));

	const auto& current_velocity = moveComponent.getVelocity();
	const auto& current_acceleration = moveComponent.getAcceleration();
	const auto& current_position = transformComponent.getTranslation();

	vec3 new_velocity = current_velocity + current_acceleration * (float)dt;
	vec3 new_position = 0.5f * (glm::pow((float)dt, 2.0f)) * current_acceleration + current_velocity * (float)dt + current_position;
	new_position = new_position * vec3(1.0f, 0.0f, 1.0f);

	moveComponent.setVelocity(new_velocity);
	transformComponent.translateLocal((new_position - current_position));
}

void pn::InitialState::startUpAssist() {
	
	auto& player = getEntity("player");
	auto& handler = pn::InputManager::g_inputManager.getInputHandler();
	firstPersonListener = std::make_shared<pn::FirstPersonListener>(&player);
	handler->addListener(firstPersonListener);

	mm::setCursor(false);
}

void pn::InitialState::shutdownAssist() {
	auto& handler = pn::InputManager::g_inputManager.getInputHandler();
	handler->removeListener(firstPersonListener);

	mm::setCursor(true);
}