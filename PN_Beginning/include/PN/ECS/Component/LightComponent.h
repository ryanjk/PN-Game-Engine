#ifndef LIGHT_COMPONENT_H
#define LIGHT_COMPONENT_H

#include "PN/ECS/Component/IComponent.h"

#include "PN/Util/Math.h"
#include "PN/Util/PString.h"

namespace pn {
	
	enum LightType {
		POINT_LIGHT = 0,
		DIRECTIONAL_LIGHT = 1,
		SPOTLIGHT = 2
	};
	
	class LightComponent : public pn::IComponent {
	public:
		static std::shared_ptr<LightComponent> make(const ComponentData& data, pn::ResourceManager& resources);
		ComponentType getType() const override;

		LightComponent();

		vec3 getColour() const;
		void setColour(vec3 colour);

		float getInnerRadians() const;
		void setInnerRadians(float innerRadians);

		float getIntensity() const;
		void setIntensity(float intensity);

		int getLightType() const;
		void setLightType(int lightType);

		float getMaxRadius() const;
		void setMaxRadius(float maxRadius);

		float getOuterRadians() const;
		void setOuterRadians(float outerRadians);

	private:
		vec3 m_colour = { 0.0f, 0.0f, 1.0f };
		float m_intensity = 1.0f;
		int m_lightType = POINT_LIGHT;
		float m_innerRadians = 0.7f;
		float m_outerRadians = 1.57f;
		float m_maxRadius = 1.0f;
	};
}

#endif