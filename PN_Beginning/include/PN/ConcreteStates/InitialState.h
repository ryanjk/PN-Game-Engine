#ifndef INITIAL_STATE_H
#define INITIAL_STATE_H

#include "PN/Window/WindowManager.h"

#include "PN/GameState/GameState.h"

#include "PN/Render/Camera.h"
#include "PN/Render/Image.h"
#include "PN/Render/Mesh.h"
#include "PN/Render/Renderable.h"

#include "LodePNG/lodepng.h"

namespace pn {
	class InitialState : public pn::GameState {
	public:
		InitialState(pn::PString stateFilename);

		void update(double dt) override;
		void render() override;

	protected:
		void startUpAssist() override;

	private:
		pn::Renderable m_r;
		pn::Renderable m_floor_r;

		pn::Camera m_camera;

		mat4 m_monkey_world_transform;
		mat4 m_floor_world_transform;

		vec3 m_light_pos;
		vec3 m_light2_pos;

		
	};
}

#endif