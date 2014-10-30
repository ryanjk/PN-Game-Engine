#ifndef PN_TRANSFORM_COMPONENT
#define PN_TRANSFORM_COMPONENT

#include "PN/ECS/Component/IComponent.h"

#include "PN/Util/Math.h"

namespace pn {
	class TransformComponent : public pn::IComponent {
	public:
		static std::shared_ptr<TransformComponent> make(const ComponentData& data);
		ComponentType getType() const override;

		TransformComponent();

		mat4 getTransformMatrix() const;
		vec3 getTranslation() const;
		vec3 getScale() const;
		vec3 getRotation() const;

		void translateWorld(vec3 by);
		void translateLocal(vec3 by);

		void scale(vec3 by);

		void rotate(vec3 eulerAngles);
		void rotatePitch(float pitch);
		void rotateYaw(float yaw);
		void rotateRoll(float roll);

		void setTranslation(vec3 translation);
		void setScale(vec3 scale);
		void setRotation(vec3 rotation);

	private:
		void updateTransformMatrix();

		mat4 m_transformMatrix = {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};

		vec3 m_translation = { 0.0f, 0.0f, 0.0f };
		vec3 m_scale = { 1.0f, 1.0f, 1.0f };
		vec3 m_rotation = { 0.0f, 0.0f, 0.0f };  // pitch, yaw, roll

		glm::fquat m_pitch;
		glm::fquat m_yaw;
		glm::fquat m_roll;

	};
}

#endif