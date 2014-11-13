#ifndef RENDER_COMPONENT_H
#define RENDER_COMPONENT_H

#include "PN/ECS/Component/IComponent.h"

#include "PN/Util/Math.h"
#include "PN/Util/PString.h"

namespace pn {
	class RenderComponent : public pn::IComponent {
	public:
		static std::shared_ptr<RenderComponent> make(const ComponentData& data, pn::ResourceManager& resources);
		ComponentType getType() const override;

		RenderComponent();

		const vec4& getAmbient() const;
		void setAmbient(const vec4& ambient);

		const pn::PString& getDiffuse() const;
		void setDiffuse(const pn::PString& diffuse);

		float getGloss() const;
		void setGloss(float gloss);

		float getAlpha() const;
		void setAlpha(float alpha);

		const pn::PString& getMeshFilename() const;
		void setMeshFilename(const pn::PString& mesh);

		const pn::PString& getMaterialFilename() const;
		void setMaterialFilename(const pn::PString& material);

		const vec4& getSpecular() const;
		void setSpecular(const vec4& specular);

	private:
		vec4 m_ambient = { 0.0f, 0.0f, 0.0f, 1.0f };
		pn::PString m_diffuse = "red.png";
		float m_gloss = 1;
		float m_alpha = 1.0f;
		pn::PString m_mesh = "cube.obj";
		pn::PString m_material = "static_light.sp";
		vec4 m_specular = { 0.0f, 0.0f, 0.0f, 1.0f };
	};
}

#endif