#ifndef INITIAL_STATE_H
#define INITIAL_STATE_H

#include "PN/Window/WindowManager.h"

#include "PN/GameState/GameState.h"

#include "PN/Render/Image.h"
#include "PN/Render/Mesh.h"
#include "PN/Render/Renderable.h"

#include "LodePNG/lodepng.h"

#include <map>

namespace pn {
	class InitialState : public pn::GameState {
	public:
		InitialState(pn::PString stateFilename);

		void update(double dt) override;
		void render() override;

	protected:
		void startUpAssist() override;

	private:
		std::map<EntityID, pn::Renderable> m_renderables;

		std::vector<EntityID> m_lights;

		EntityPointer m_activeCamera;

		vec3 m_light_pos;
		vec3 m_light2_pos;

		
	};
}

#endif