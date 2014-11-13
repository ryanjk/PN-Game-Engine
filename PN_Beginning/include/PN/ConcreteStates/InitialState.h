#ifndef INITIAL_STATE_H
#define INITIAL_STATE_H

#include "PN/GameState/GameState.h"

#include "PN/Util/PString.h"

#include "PN/Input/FirstPersonListener.h"

#include <memory>

namespace pn {

	class InitialState : public pn::GameState {
	public:
		InitialState(const pn::PString& stateFilename);

		void update(double dt) override;

	protected:
		void startUpAssist() override;
		void shutdownAssist() override;

	private:
		std::shared_ptr<pn::FirstPersonListener> firstPersonListener;

	};
}

#endif