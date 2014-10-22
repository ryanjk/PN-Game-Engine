#ifndef PN_ENTITY_H
#define PN_ENTITY_H

using ID = int;

namespace pn {
	class Entity {
	public:
		Entity(ID id);

		ID getID() const;

	private:
		ID m_ID;

	};
}

#endif