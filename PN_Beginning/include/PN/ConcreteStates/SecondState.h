#ifndef SECOND_STATE_H
#define SECOND_STATE_H

#include "PN/GameState/GameState.h"

#include "PN/Input/FirstPersonListener.h"

namespace pn {
	class SecondState : public pn::GameState {
	public:
		SecondState(const pn::PString& stateFilename);

		void update(double dt) override;

	protected:
		void startUpAssist() override;
//		void shutdownAssist() override;

	private:
		std::shared_ptr<pn::FirstPersonListener> firstPersonListener;

	};
}

#endif