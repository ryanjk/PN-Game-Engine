#ifndef PN_TRANSFORM_COMPONENT
#define PN_TRANSFORM_COMPONENT

#include "PN/ECS/Component/IComponent.h"

#include "PN/Util/Math.h"


namespace pn {
	class TransformComponent : public pn::IComponent {
	public:
		friend class PhysicsSystem;

		static std::shared_ptr<TransformComponent> make(const ComponentData& data, pn::ResourceManager& resources);
		ComponentType getType() const override;

		TransformComponent();

		const mat4& getTransformMatrix() const;

		const vec3& getTranslation() const;
		const vec3& getScale() const;
		const vec3& getRotation() const;

	private:

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