#ifndef FIRST_PERSON_LISTENER_H
#define FIRST_PERSON_LISTENER_H

#include "PN/Input/InputEventListener.h"

#include "PN/Render/Camera.h"

#include "PN/Window/WindowManager.h"

#include "PN/Settings/SettingsManager.h"

namespace pn {
	class FirstPersonListener : public pn::InputEventListener {
	public:
		FirstPersonListener(pn::Camera* camera);

		void onMouseMove(double from_x, double from_y, double to_x, double to_y) override;
		
		void onKeyPress(int key) override;
		void onKeyRelease(int key) override;

		void update(double dt) override;

	private:
		pn::Camera* m_camera;
		double m_scrollSpeed;
		double m_moveSpeed;
	};
}

#endif