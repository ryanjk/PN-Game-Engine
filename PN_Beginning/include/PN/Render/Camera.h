#ifndef CAMERA_H
#define CAMERA_H

#include "PN/Util/Math.h"

#include "PN/ECS/Component/CameraComponent.h"

using CameraComponentPointer = std::shared_ptr < pn::CameraComponent > ;

namespace pn {
	class Camera {
	public:
		Camera();

		void setCamera(CameraComponentPointer cameraComponent);

		void translate(vec3 amount);

		void applyPitch(double radians);
		void applyYaw(double radians);

		mat4 getView();

		vec3 getPosition();
	private:
		CameraComponentPointer m_currentCamera;
		
//		vec3 m_up = vec3(0.0f, 1.0f, 0.0f);
	//	vec3 m_origin = vec3(0.0f, 0.0f, 5.0f);
	//	vec3 m_at = vec3(0.0f, 0.0f, -1.0f);

	//	double m_pitch = 0.0;
	//	double m_yaw = 0.0;
	};
}

#endif