#ifndef CAMERA_H
#define CAMERA_H

#include "PN/Util/Math.h"

namespace pn {
	class Camera {
	public:
		Camera();

		void translate(vec3 amount);

		void applyPitch(float radians);
		void applyYaw(float radians);

		mat4 getView();

		vec3 getPosition();
	private:
		vec3 m_up = vec3(0.0f, 1.0f, 0.0f);
		vec3 m_origin = vec3(0.0f, 0.0f, 5.0f);
		vec3 m_at = vec3(0.0f, 0.0f, -1.0f);

		float m_pitch = 0.0f;
		float m_yaw = 0.0f;
	};
}

#endif