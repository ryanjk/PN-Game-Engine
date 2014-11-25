#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "PN/Command/ResourceLoadCommand.h"

#include "PN/Util/PString.h"
#include "PN/Util/ResourceManager.h"

#include "PN/ECS/Entity/Entity.h"

#include "PN/Render/RenderSystem.h"

#include "PN/Physics/PhysicsSystem.h"

#include "json/json.h"

#include <map>
#include <functional>

using EntityPointer = std::shared_ptr < pn::Entity >;
using Entities = std::vector < EntityPointer >;

namespace pn {

	class MatrixStack;

	class GameState {

		friend class LoadingState;
		friend class RenderSystem;
		friend class PhysicsSystem;

	public:
		GameState(pn::PString stateFileName);
		virtual ~GameState() {}

		virtual void update(double dt) = 0;
		void updatePhysics(double dt);
		void render();  // call rendering functions

		void startUp();  // set up the state (load resources, set rendering state, etc)
		void shutdown();   // shutdown state

		bool isLoaded() const;

		/*
		Entity Access Methods
		*/
		pn::Entity& getRootEntity();

		pn::Entity& getEntity(const pn::PString& entity_name);
		pn::Entity& getEntity(EntityID entity_id);

		const mat4& getEntityWorldTransform(const pn::PString& entity_name, std::function<mat4(void)> calcMatrix);
		const mat4& getEntityWorldTransform(EntityID entity_id, std::function<mat4(void)> calcMatrix);

	protected:
		virtual void startUpAssist();
		virtual void shutdownAssist();

		RenderSystem m_renderSystem;
		PhysicsSystem m_physicsSystem;

		Entities m_entities;
		pn::ResourceManager m_resources;

	private:
		pn::PString m_stateFilename;
		Json::Value m_root;

		std::map<EntityID, mat4> m_worldMatrix_cache;
		std::map<EntityID, bool> m_worldMatrix_needUpdate;

		void startUpSystems();
		void shutdownSystems();

		void loadResources();
		void loadEntities();
		void loadEntitiesRec(const Json::Value& entity_tree_root, EntityPointer& parent);

		// only called by loading state, these contribute to the command queue used by the state to load sequentially
		void loadResourcesCommand(CommandQueue& commandQueue);
		void loadEntitiesCommand(CommandQueue& commandQueue);
		void loadEntitiesRecCommand(const Json::Value& entity_tree_root, EntityPointer& parent, CommandQueue& commandQueue);

		void releaseResources();
		void releaseEntities();

		bool m_loaded; // is the state already loaded in memory

	};
}

#endif

