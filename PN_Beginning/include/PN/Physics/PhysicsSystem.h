#ifndef PN_PHYSICS_SYS_H
#define PN_PHYSICS_SYS_H

namespace pn {
	class GameState;
	class PhysicsSystem {
	public:
		PhysicsSystem();
		void startUp(GameState* state);
		void shutdown();

		void run(double dt);

	private:
		GameState* m_gameState;

		void integrateWorld(double dt);

	};
}

#endif