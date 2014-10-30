#ifndef CAMERA_COMPONENT_H
#define CAMERA_COMPONENT_H

#include "PN/ECS/Component/IComponent.h"

#include "PN/Util/Math.h"
#include "PN/Util/PString.h"

namespace pn {
	class CameraComponent : public pn::IComponent {
	public:
		static std::shared_ptr<CameraComponent> make(const ComponentData& data);
		ComponentType getType() const override;

		CameraComponent();

		vec3 getAt() const;
		void setAt(vec3 at);

		vec3 getOrigin() const;
		void setOrigin(vec3 origin);

		double getPitch() const;
		void setPitch(double pitch);

		vec3 getUp() const;
		void setUp(vec3 up);

		double getYaw() const;
		void setYaw(double yaw);

	private:
		vec3 m_at = { 0.0f, 0.0f, -1.0f };
		vec3 m_origin = { 0.0f, 0.0f, 0.0f };
		double m_pitch = 0;
		vec3 m_up = { 0.0f, 1.0f, 0.0f };
		double m_yaw = 0;
	};
}

#endif