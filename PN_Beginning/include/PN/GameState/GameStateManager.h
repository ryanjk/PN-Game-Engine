#ifndef GAME_STATE_MANAGER_H
#define GAME_STATE_MANAGER_H

#include <vector>
#include <memory>

#include "PN/ConcreteStates/LoadingState.h"
#include "PN/GameState/GameState.h"

using GameStatePointer = std::shared_ptr < pn::GameState > ;
using GameStates = std::vector < GameStatePointer >;

namespace pn {
	class GameStateManager {

		friend class LoadingState;

	public:
		static GameStateManager g_gameStateManager;

		GameStatePointer getCurrentState();

		void startUp();
		void shutdown();

		void setState(GameStatePointer state);

		void notifyState(double dt);
		void renderState();
	private:
		GameStateManager();

		GameStatePointer m_currentState;
		std::shared_ptr<pn::LoadingState> m_loadingState;
	};
}

#endif