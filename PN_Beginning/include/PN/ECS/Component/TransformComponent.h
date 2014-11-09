#ifndef PN_TRANSFORM_COMPONENT
#define PN_TRANSFORM_COMPONENT

#include "PN/ECS/Component/IComponent.h"

#include "PN/Util/Math.h"

/*
Transforms:
	Translate{Space}
	Rotate[Axis]{Space}
*/

namespace pn {
	class TransformComponent : public pn::IComponent {
	public:
		static std::shared_ptr<TransformComponent> make(const ComponentData& data, pn::ResourceManager& resources);
		ComponentType getType() const override;

		TransformComponent();

		const mat4& getTransformMatrix() const;
		const mat4& getWorldTransformMatrix() const;

		const vec3& getTranslation() const;
		const vec3& getScale() const;
		const vec3& getRotation() const;

		void translateWorld(const vec3& by);
		void translateLocal(const vec3& by);

		void scale(const vec3& by);

		void rotateLocal(const vec3& eulerAngles);
		void rotatePitchLocal(float pitch);
		void rotateYawLocal(float yaw);
		void rotateRollLocal(float roll);

		void rotateParent(const vec3& eulerAngles, const vec3& point = {0.0f, 0.0f, 0.0f});
		void rotatePitchParent(float pitch, const vec3& point = { 0.0f, 0.0f, 0.0f });
		void rotateYawParent(float yaw, const vec3& point = { 0.0f, 0.0f, 0.0f });
		void rotateRollParent(float roll, const vec3& point = { 0.0f, 0.0f, 0.0f });

		void setTranslation(const vec3& translation);
		void setScale(const vec3& scale);
		void setRotation(const vec3& rotation);

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

	};
}

#endif