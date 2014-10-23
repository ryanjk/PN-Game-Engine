#ifndef RENDER_COMPONENT_H
#define RENDER_COMPONENT_H

#include "PN/ECS/Component/IComponent.h"

#include "PN/Util/Math.h"
#include "PN/Util/PString.h"

namespace pn {
	class RenderComponent : public pn::IComponent {
	public:
		static RenderComponent make(const ComponentData& data);
		ComponentType getType() const override;

		vec4 getAmbient() const;
		void setAmbient(vec4 ambient);

		vec4 getDiffuse() const;
		void setDiffuse(vec4 diffuse);

		float getGloss() const;
		void setGloss(float gloss);

		pn::PString getMesh() const;
		void setMesh(pn::PString mesh);

		vec4 getSpecular() const;
		void setSpecular(vec4 specular);

	private:
		vec4 m_ambient = { 0.0f, 0.0f, 0.0f, 0.0f };
		vec4 m_diffuse = { 0.0f, 0.0f, 0.0f, 0.0f };
		float m_gloss = 0;
		pn::PString m_mesh = "this_model_is_not_set.obj";
		vec4 m_specular = { 0.0f, 0.0f, 0.0f, 0.0f };
	};
}

#endif