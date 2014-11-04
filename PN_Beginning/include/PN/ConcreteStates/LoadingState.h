#ifndef LOADING_STATE
#define LOADING_STATE

#include "PN/GameState/GameState.h"

#include "PN/Render/Image.h"
#include "PN/Render/Mesh.h"
#include "PN/Render/Renderable.h"

#include "PN/Window/WindowManager.h"

namespace pn {
	class LoadingState : public pn::GameState {
	public:
		LoadingState(pn::PString stateFilename);

		void update(double dt) override;
		void render() override;

	protected:
		void startUpAssist() override;

	private:
		pn::Renderable m_ls_r;

	};
}

#endif