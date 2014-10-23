#ifndef CAMERA_COMPONENT_H
#define CAMERA_COMPONENT_H

#include "PN/ECS/Component/IComponent.h"

#include "PN/Util/Math.h"
#include "PN/Util/PString.h"

namespace pn {
	class CameraComponent : public pn::IComponent {
	public:
		static CameraComponent make(const ComponentData& data);
		ComponentType getType() const override;

		bool getFov() const;
		void setFov(bool fov);

		vec3 getRotation() const;
		void setRotation(vec3 rotation);

		vec3 getTranslation() const;
		void setTranslation(vec3 translation);

	private:
		bool m_fov = true;
		vec3 m_rotation = { 0.0f, 0.0f, 0.0f };
		vec3 m_translation = { 0.0f, 0.0f, 0.0f };
	};
}

#endif