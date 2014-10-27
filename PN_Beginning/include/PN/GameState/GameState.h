#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "PN/Util/PString.h"
#include "PN/Util/ResourceManager.h"

#include "PN/ECS/Entity/Entity.h"

#include "json/json.h"

#include <fstream>

using EntityPointer = std::shared_ptr < pn::Entity > ;
using Entities = std::vector < std::shared_ptr<pn::Entity> >;

namespace pn {
	class GameState {
	public:
		GameState(pn::PString stateFileName);

		virtual void update(double dt) = 0;
		virtual void render();  // call rendering functions

		void startUp();  // set up the state (load resources, set rendering state, etc)
		void shutdown();   // shutdown state

		Entities& getEntities();

	protected:
		virtual void startUpAssist();
		virtual void shutdownAssist();

		Entities m_entities;

	private:
		pn::PString m_stateFilename;
		Json::Value m_root;

		void loadResources();
		void releaseResources();

		void loadEntities();
		void releaseEntities();
	};
}

#endif

