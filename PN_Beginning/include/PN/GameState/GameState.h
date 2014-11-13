#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "PN/Command/ResourceLoadCommand.h"

#include "PN/Util/PString.h"
#include "PN/Util/ResourceManager.h"

#include "PN/ECS/Entity/Entity.h"

#include "PN/Render/Renderable.h"

#include "json/json.h"

#include <map>

using EntityPointer = std::shared_ptr < pn::Entity >;
using Entities = std::vector < EntityPointer >;

namespace pn {

	class MatrixStack;
	struct DrawCall;
	using DrawCallContainer = std::multimap < int, DrawCall >;

	class GameState {

		friend class LoadingState;

	public:
		GameState(pn::PString stateFileName);

		virtual void update(double dt) = 0;
		void render();  // call rendering functions

		void startUp();  // set up the state (load resources, set rendering state, etc)
		void shutdown();   // shutdown state

		bool isLoaded() const;

		pn::Entity& getRootEntity();

		pn::Entity& getEntity(const pn::PString& entity_name);
		pn::Entity& getEntity(EntityID entity_id);

		mat4 getEntityWorldTransform(const pn::PString& entity_name);
		mat4 getEntityWorldTransform(EntityID entity_id);

	protected:
		virtual void startUpAssist();
		virtual void shutdownAssist();

		pn::Entity* m_activeCamera;

		Entities m_entities;
		pn::ResourceManager m_resources;

	private:
		void buildDrawCalls(EntityID start, pn::MatrixStack& matrixStack, DrawCallContainer& m_drawCalls);
		void renderDrawCalls(DrawCallContainer& drawCalls);

		std::map<EntityID, pn::Renderable> m_renderables;
		std::vector<EntityID> m_lights;
		
		pn::PString m_stateFilename;
		Json::Value m_root;

		void loadResources();
		void releaseResources();

		void loadEntities();
		void loadEntitiesRec(const Json::Value& entity_tree_root, EntityPointer& parent);
		void releaseEntities();

		// only called by loading state, these contribute to the command queue used by the state to load sequentially
		void loadResourcesCommand(CommandQueue& commandQueue);
		void loadEntitiesCommand(CommandQueue& commandQueue);
		void loadEntitiesRecCommand(const Json::Value& entity_tree_root, EntityPointer& parent, CommandQueue& commandQueue);

		bool m_loaded; // is the state already loaded in memory

	};
}

#endif

