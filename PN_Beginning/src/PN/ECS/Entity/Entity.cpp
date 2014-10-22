#include "PN/ECS/Entity/Entity.h"

pn::Entity::Entity(ID id) : m_ID(id)
{}

ID pn::Entity::getID() const {
	return m_ID;
}