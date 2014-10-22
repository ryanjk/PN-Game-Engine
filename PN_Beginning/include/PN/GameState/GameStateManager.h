#ifndef GAME_STATE_MANAGER_H
#define GAME_STATE_MANAGER_H

#include <vector>
#include <memory>
#include "PN/GameState/GameState.h"

using GameStatePointer = std::shared_ptr < pn::GameState > ;
using GameStates = std::vector < GameStatePointer >;

namespace pn {
	class GameStateManager {
	public:
		static GameStateManager g_gameStateManager;

		void startUp();
		void shutdown();

		void setState(GameStatePointer state);

		void notifyState(double dt);
		void renderState();
	private:
		GameStateManager();
		void beginLoad();

		GameStatePointer m_currentState;
		GameStatePointer m_loadingState;
	};
}

#endif