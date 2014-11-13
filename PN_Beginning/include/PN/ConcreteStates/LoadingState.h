#ifndef LOADING_STATE
#define LOADING_STATE

#include "PN/Command/Command.h"

#include "PN/GameState/GameState.h"

#include "PN/Render/Image.h"
#include "PN/Render/Mesh.h"
#include "PN/Render/Renderable.h"

#include "PN/Window/WindowManager.h"

#include <vector>

namespace pn {
	class LoadingState : public pn::GameState {
	public:
		LoadingState(pn::PString stateFilename);

		void setStateToLoad(std::shared_ptr<pn::GameState>);

		void update(double dt) override;

	protected:
		void startUpAssist() override;

	private:
		double m_startLoadTime;
		
		int m_totalCommands;
		int m_commandsLoadedSoFar;

		std::vector<std::shared_ptr<pn::Command>> m_loadingCommands;
		std::shared_ptr<pn::GameState> m_stateBeingLoaded;

	};
}

#endif