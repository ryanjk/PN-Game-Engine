#include "PN/GameState/GameState.h"

#include "PN/Command/ResourceLoadCommand.h"
#include "PN/Command/EntityLoadCommand.h"

#include "PN/Render/DrawCall.h"

#include "PN/ECS/Entity/Entity.h"
#include "PN/ECS/Component/TransformComponent.h"
#include "PN/ECS/Component/RenderComponent.h"

#include "PN/Util/Math.h"
#include "PN/Util/MatrixStack.h"

#include "PN/Settings/SettingsManager.h"

#include <iostream>
#include <cassert>
#include <algorithm>
#include <fstream>

static const pn::PString STATE_RESOURCE_FILEPATH = "resource/state/";

static auto& settings = pn::SettingsManager::g_SettingsManager;

pn::GameState::GameState(pn::PString stateFileName) : m_stateFilename(stateFileName), m_activeCamera(), 
m_entities(), m_resources(), m_renderables(), m_lights(), m_root(), m_loaded(false)
{}

/*
startUp() and shutdown() use template method pattern -- concrete classes may implement _Assist() method if needed
*/

void pn::GameState::startUp() {
	loadResources();
	loadEntities();
	startUpAssist();

	m_loaded = true;
}

void pn::GameState::shutdown() {
	shutdownAssist();
	releaseEntities();
	releaseResources();

	m_loaded = false;
}

bool pn::GameState::isLoaded() const {
	return m_loaded;
}

/*
Parse the *.state file and load resources (typically ones not initially used by any state entities) via ResourceManager singleton 
*/
void pn::GameState::loadResources() {
	Json::Reader reader;

	std::ifstream state_file;
	state_file.open((STATE_RESOURCE_FILEPATH + m_stateFilename).getText());

	if (!state_file.is_open()) {
		std::cout << "State Load: could not load " << m_stateFilename.getText() << std::endl;
		getchar();
		exit(-1);
	}

	bool success = reader.parse(state_file, m_root);
	if (!success) {
		std::cout << "State Load: could not parse state file: " << reader.getFormattedErrorMessages();
		state_file.close();
		getchar();
		exit(-1);
	}

	state_file.close();

	auto& resourceTree = m_root["resource"];

	for (auto& resource : resourceTree) {
		for (auto& resourceFilename : resource) {
			m_resources.load(resourceFilename.asString());
		}
	}
}

void pn::GameState::loadResourcesCommand(CommandQueue& commandQueue) {
	Json::Reader reader;

	std::ifstream state_file;
	state_file.open((STATE_RESOURCE_FILEPATH + m_stateFilename).getText());

	if (!state_file.is_open()) {
		std::cout << "State Load: could not load " << m_stateFilename.getText() << std::endl;
		getchar();
		exit(-1);
	}

	bool success = reader.parse(state_file, m_root);
	if (!success) {
		std::cout << "State Load: could not parse state file: " << reader.getFormattedErrorMessages();
		state_file.close();
		getchar();
		exit(-1);
	}

	state_file.close();

	auto& resourceTree = m_root["resource"];

	for (auto& resource : resourceTree) {
		for (auto& resourceFilename : resource) {
			commandQueue.push_back(std::make_shared<pn::ResourceLoadCommand>(&m_resources, resourceFilename.asString()));
		}
	}

}

void pn::GameState::releaseResources() {
	m_resources.removeAll();

	// delete gl objects used by renderables
	for (auto& i : m_renderables) {
		auto& renderable = i.second;

		glDeleteVertexArrays(1, &renderable.VAO);

		glDeleteBuffers(1, &renderable.VBO_v);
		glDeleteBuffers(1, &renderable.VBO_vn);
		glDeleteBuffers(1, &renderable.VBO_vt);

		glDeleteTextures(1, &renderable.TBO_diffuse);
		glDeleteSamplers(1, &renderable.sampler_diffuse);
	}

}

static void dfs(pn::Entity& entity, pn::GameState* state) {
	std::cout << entity.getName() << std::endl;
	for (auto& e : entity.getChildrenID()) {
		std::cout << "Child of " << entity.getName() << ": ";
		dfs(state->getEntity(e), state);
	}
}

void pn::GameState::loadEntities() {
	auto& entityTree = m_root["entities"];
	if (entityTree.isNull()) return; // Leave if there are no entities in this state

	// This is only an estimate for the amount of entities -- doesn't include children
	auto num_entities = entityTree.size();
	m_entities.reserve(num_entities + 1);

	// Create a root entity who will contain all entities in the state as children
	auto root = std::make_shared<pn::Entity>(this->m_stateFilename);

	loadEntitiesRec(entityTree, root);
	m_entities.push_back(root);

	#ifdef _DEBUG
	for (size_t i = 0; i < m_entities.size(); i++) {
		for (size_t j = i + 1; j < m_entities.size(); j++) {
			assert(m_entities[i]->getID() != m_entities[j]->getID() && "Two entities share an ID\n");
		}
	}
	#endif

	// set up renderable for each entity that needs one
	for (auto& entity_i : m_entities) {
		auto& entity = *entity_i;
		bool hasRenderComponent = (entity.getKey() & pn::ComponentType::RENDER) == pn::ComponentType::RENDER;
		if (hasRenderComponent) {
			auto& renderComponent = std::dynamic_pointer_cast<pn::RenderComponent>(entity.getComponent(pn::ComponentType::RENDER));
			auto& entity_material = m_resources.getMaterial(renderComponent->getMaterialFilename());

			Renderable entity_renderable;

			entity_material.setUpRenderable(entity_renderable, *renderComponent, m_resources);

			m_renderables.insert({ entity.getID(), entity_renderable });
		}

		// add to light entities if applicable
		bool hasLightComponent = (entity.getKey() & pn::ComponentType::LIGHT) == pn::ComponentType::LIGHT;
		if (hasLightComponent) {
			m_lights.push_back(entity.getID());
		}
	}

	// set the state's camera
	auto& camera = getEntity("camera");
	m_activeCamera = &camera;
}

void pn::GameState::loadEntitiesRec(const Json::Value& current_entity_tree_root, EntityPointer& parent_p) {
	if (current_entity_tree_root.isNull()) {
		return;
	}

	auto& parent = *parent_p;

	// Add children entities to the entity group
	for (std::string& entity : current_entity_tree_root.getMemberNames()) {

		auto new_entity = std::make_shared<pn::Entity>(entity);
		new_entity->setParent(parent.getID());

		EntityID new_entity_id = new_entity->getID();

		parent.addChild(new_entity_id);

		auto& components = current_entity_tree_root[entity]["components"];
		for (std::string& component : components.getMemberNames()) {
			auto new_component = pn::IComponent::make(components[component], component, m_resources);
			new_entity->addComponent(new_component);
		}

		if (!current_entity_tree_root[entity]["children"].isNull()) {
			loadEntitiesRec(current_entity_tree_root[entity]["children"], new_entity);
		}

		m_entities.push_back(new_entity);
	}
}

void pn::GameState::loadEntitiesCommand(CommandQueue& commandQueue) {
	auto& entityTree = m_root["entities"];
	if (entityTree.isNull()) return; // Leave if there are no entities in this state

	// This is only an estimate for the amount of entities -- doesn't include children
	auto num_entities = entityTree.size();
	m_entities.reserve(num_entities + 1);

	// Create a root entity who will contain all entities in the state as children
	auto root = std::make_shared<pn::Entity>(this->m_stateFilename);

	loadEntitiesRecCommand(entityTree, root, commandQueue);

	m_entities.push_back(root);

	class RenderableSetUpCommand : public pn::Command {
	public:
		RenderableSetUpCommand(pn::ResourceManager* resources,
			std::map<EntityID, pn::Renderable>* state_renderables,
			pn::Entity* entity) :
			m_resources(resources), m_renderables(state_renderables), m_entity(entity)
		{}

		void execute() override {
			bool hasRenderComponent = (m_entity->getKey() & pn::ComponentType::RENDER) == pn::ComponentType::RENDER;
			if (hasRenderComponent) {
				auto& renderComponent = std::dynamic_pointer_cast<pn::RenderComponent>(m_entity->getComponent(pn::ComponentType::RENDER));
				auto& entity_material = m_resources->getMaterial(renderComponent->getMaterialFilename());

				Renderable entity_renderable;

				entity_material.setUpRenderable(entity_renderable, *renderComponent, *m_resources);

				m_renderables->insert({ m_entity->getID(), entity_renderable });
			}
		}

	private:
		pn::ResourceManager* m_resources;
		std::map<EntityID, pn::Renderable>* m_renderables;
		pn::Entity* m_entity;
	};

	class LightSetUpCommand : public pn::Command {
	public:
		LightSetUpCommand(pn::Entity* entity, std::vector<EntityID>* state_lights) : m_entity(entity), m_state_lights(state_lights)
		{}

		void execute() override {
			bool hasLightComponent = (m_entity->getKey() & pn::ComponentType::LIGHT) == pn::ComponentType::LIGHT;
			if (hasLightComponent) {
				m_state_lights->push_back(m_entity->getID());
			}
		}

	private:
		pn::Entity* m_entity;
		std::vector<EntityID>* m_state_lights;
	};

	// set up renderable for each entity that needs one
	for (auto& entity : m_entities) {
		commandQueue.push_back(std::make_shared<RenderableSetUpCommand>(&m_resources, &m_renderables, entity.get()));

		// add to light entities if applicable
		commandQueue.push_back(std::make_shared<LightSetUpCommand>(entity.get(), &m_lights));
	}

	class CameraSetUpCommand : public pn::Command {
	public:
		CameraSetUpCommand(pn::Entity** state_camera_entity, pn::Entity* camera_to_use) :
			m_state_camera_entity(state_camera_entity), m_camera_to_use(camera_to_use)
		{}

		void execute() override {
			*m_state_camera_entity = m_camera_to_use;
		}

	private:
		pn::Entity** m_state_camera_entity;
		pn::Entity* m_camera_to_use;
	};

	// set the state's camera
	auto& camera = getEntity("camera");
	commandQueue.push_back(std::make_shared<CameraSetUpCommand>(&m_activeCamera, &camera));
}

void pn::GameState::loadEntitiesRecCommand(const Json::Value& current_entity_tree_root, EntityPointer& parent_p, CommandQueue& commandQueue) {
	if (current_entity_tree_root.isNull()) {
		return;
	}

	auto& parent = *parent_p;

	// Add children entities to the entity group
	for (std::string& entity : current_entity_tree_root.getMemberNames()) {

		auto new_entity = std::make_shared<pn::Entity>(entity);
		new_entity->setParent(parent.getID());

		EntityID new_entity_id = new_entity->getID();

		parent.addChild(new_entity_id);

		auto& components = current_entity_tree_root[entity]["components"];
		commandQueue.push_back(std::make_shared<pn::EntityLoadCommand>(&components, new_entity.get(), &m_resources));

		if (!current_entity_tree_root[entity]["children"].isNull()) {
			loadEntitiesRecCommand(current_entity_tree_root[entity]["children"], new_entity, commandQueue);
		}

		m_entities.push_back(new_entity);
	}
}

void pn::GameState::releaseEntities() {
	m_entities.clear();
}

pn::Entity& pn::GameState::getRootEntity() {
	return getEntity(this->m_stateFilename.getHash());
}

pn::Entity& pn::GameState::getEntity(const pn::PString& entity_name) {
	return getEntity(entity_name.getHash());
}

pn::Entity& pn::GameState::getEntity(EntityID entity_id) {
	for (auto& e_p : m_entities) {
		auto& e = *e_p;
		if (e.getID() == entity_id) {
			return e;
		}
	}

	assert(false && "No entity found");

	return *m_entities[0];
}

mat4 pn::GameState::getEntityWorldTransform(EntityID entity_id) {
	const EntityID& root = this->m_stateFilename.getHash();

	auto& entity = getEntity(entity_id);

	auto transformComponent = std::dynamic_pointer_cast<pn::TransformComponent>(entity.getComponent(pn::ComponentType::TRANSFORM));
	mat4 world_matrix = transformComponent->getTransformMatrix();

	auto parentID = entity.getParent();
	while (parentID != root) {
		auto& parent = getEntity(parentID);
		auto parentTransform = std::dynamic_pointer_cast<pn::TransformComponent>(parent.getComponent(pn::ComponentType::TRANSFORM));
		mat4 parent_world_matrix = parentTransform->getTransformMatrix();
		world_matrix = parent_world_matrix * world_matrix;

		parentID = parent.getParent();
	}

	return world_matrix;
}

mat4 pn::GameState::getEntityWorldTransform(const pn::PString& entity_name) {
	return getEntityWorldTransform(entity_name.getHash());
}

void pn::GameState::render() {
	glEnable(GL_DEPTH_TEST);
	
	DrawCallContainer drawCalls;

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	pn::MatrixStack matrixStack;

	for (auto root_children : getRootEntity().getChildrenID()) {
		buildDrawCalls(root_children, matrixStack, drawCalls);
	}

	renderDrawCalls(drawCalls);

	glUseProgram(0);
}

void pn::GameState::buildDrawCalls(EntityID current_entity_ID, pn::MatrixStack& matrixStack, DrawCallContainer& drawCalls) {
	const auto& current_entity = getEntity(current_entity_ID);

	// If the entity does not have a position in space, then its children do not, so stop travelling down this branch
	bool hasTransform = (current_entity.getKey() & (pn::ComponentType::TRANSFORM)) == (pn::ComponentType::TRANSFORM);
	if (!hasTransform) {
		return;
	}

	// Push the entity's matrix onto the matrix stack
	const auto& transformComponent = std::dynamic_pointer_cast<pn::TransformComponent>(current_entity.getComponent(pn::ComponentType::TRANSFORM));
	matrixStack.push(transformComponent->getTransformMatrix());

	// If the entity is renderable, create a draw call and add it to the draw call container
	bool hasRender = (current_entity.getKey() & (pn::ComponentType::RENDER)) == (pn::ComponentType::RENDER);
	if (hasRender) {
		auto& renderComponent = std::dynamic_pointer_cast<pn::RenderComponent>(current_entity.getComponent(pn::ComponentType::RENDER));
		auto& material = m_resources.getMaterial(renderComponent->getMaterialFilename());

		auto& entityRenderable = m_renderables[current_entity_ID];

		// world position of entity
		mat4 worldTransform(matrixStack.multiply());

		// amount of vertices to render
		unsigned int num_vertices = m_resources.getMesh(entityRenderable.mesh).getVertices().size();

		// put draw call into container
		pn::DrawCall drawCall({ worldTransform, entityRenderable, material, num_vertices, renderComponent });
		drawCalls.insert({ material.getMaterialID(), drawCall });
	}

	// Continue visiting the entity's children
	for (auto childID : current_entity.getChildrenID()) {
		buildDrawCalls(childID, matrixStack, drawCalls);
	}

	matrixStack.pop();
}

void pn::GameState::renderDrawCalls(DrawCallContainer& drawCalls) {
	static const auto dynamic_light_material = pn::PString("dynamic_light.sp").getHash();
	static const auto static_light_material = pn::PString("static_light.sp").getHash();

	// Camera position
	const mat4& camera_world_transform = getEntityWorldTransform(m_activeCamera->getID());

	// Get view transform from camera
	const auto view_transform(glm::inverse(camera_world_transform));

	HashValue last_material_rendered = 0;
	// render each draw call -- it goes in order of material ID because the map is sorted by this
	for (auto& drawCallIter : drawCalls) {
		auto& drawCall = drawCallIter.second;
		auto current_material_hash = drawCall.material.getMaterialFilename().getHash();
		auto& current_material = drawCall.material;

		// If the current program is not the same as the last one used, set new global uniforms
		if (!(current_material_hash == last_material_rendered)) {
			current_material.setGlobalUniforms(*this, m_lights, camera_world_transform, view_transform);
			last_material_rendered = current_material_hash;
		}

		current_material.setInstanceUniforms(drawCall);
		glDrawArraysInstanced(GL_TRIANGLES, 0, drawCall.num_vertices, 1);

	}
}

void pn::GameState::startUpAssist() {}
void pn::GameState::shutdownAssist() {}