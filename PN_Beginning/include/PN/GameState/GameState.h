#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "PN/Util/PString.h"
#include "PN/Util/ResourceManager.h"

#include "json/json.h"

#include <fstream>

namespace pn {
	class GameState {
	public:
		GameState(pn::PString stateFileName);

		virtual void update(double dt) = 0;
		virtual void render();  // call rendering functions

		virtual void startUp() final;  // set up the state (load resources, set rendering state, etc)

		virtual void shutdown() final;   // shutdown state

	protected:
		virtual void startUpAssist();
		virtual void shutdownAssist();

	private:
		pn::PString m_stateFilename;
		Json::Value m_root;
		void loadResources();
		void releaseResources();
	};
}

#endif

