#ifndef COLLIDE_COMPONENT_H
#define COLLIDE_COMPONENT_H

#include "PN/ECS/Component/IComponent.h"

#include "PN/Physics/BoundingContainer/BoundingContainer.h"

#include "PN/Util/Math.h"
#include "PN/Util/PString.h"

namespace pn {
	class CollideComponent : public pn::IComponent {
	public:
		static std::shared_ptr<CollideComponent> make(const ComponentData& data, pn::ResourceManager& resources);
		ComponentType getType() const override;

		CollideComponent();

		BoundingContainerType getContainerType() const;
		void setContainerType(BoundingContainerType containerType);

		float getScale() const;

	private:
		BoundingContainerType m_containerType = BOUNDING_OBB;
		float m_scale = 1.0f;
	};
}

#endif