#include "PN/Render/RenderSystem.h"

#include "PN/GameState/GameState.h"

#include "PN/ECS/Component/RenderComponent.h"
#include "PN/ECS/Component/TransformComponent.h"

#include "PN/Physics/BoundingContainer/BoundingOBB.h"

#include "PN/Settings/SettingsManager.h"

static auto& settings = pn::SettingsManager::g_SettingsManager;

pn::RenderSystem::RenderSystem() {}

void pn::RenderSystem::startUp(pn::GameState* state) {
	m_state = state;

	// Initialize GL data for drawing collision primitives
	createCollisionPrimitiveGLObjects();

	for (auto& entity_i : m_state->m_entities) {
		auto& entity = *entity_i;

		// add to light entities if applicable
		bool hasLightComponent = entity.hasComponents(pn::ComponentType::LIGHT);
		if (hasLightComponent) {
			m_lights.push_back(entity.getID());
		}
	}

	// set the state's camera
	auto& camera = m_state->getEntity("camera");
	m_activeCamera = &camera;
}

void pn::RenderSystem::createCollisionPrimitiveGLObjects() {
	pn::RenderSystem::glObjects bounding_box;
	GLfloat buffer_data[] = {
		-0.5, -0.5, 0.5,
		0.5, -0.5, 0.5,
		0.5, 0.5, 0.5,
		-0.5, 0.5, 0.5,
		-0.5, -0.5, -0.5,
		0.5, -0.5, -0.5,
		0.5, 0.5, -0.5,
		-0.5, 0.5, -0.5
	};

	GLushort buffer_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// top
		3, 2, 6,
		6, 7, 3,
		// back
		7, 6, 5,
		5, 4, 7,
		// bottom
		4, 5, 1,
		1, 0, 4,
		// left
		4, 0, 3,
		3, 7, 4,
		// right
		1, 5, 6,
		6, 2, 1,
	};

	glGenVertexArrays(1, &bounding_box.VAO);
	glBindVertexArray(bounding_box.VAO);

	glGenBuffers(1, &bounding_box.VBO);
	glBindBuffer(GL_ARRAY_BUFFER, bounding_box.VBO);
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(GLfloat), buffer_data, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &bounding_box.IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bounding_box.IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(GLushort), buffer_indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	bounding_box.num_tris = 36;
	m_primitiveGLObjects.insert(std::make_pair(pn::BoundingContainerType::BOUNDING_AABB, bounding_box));
	m_primitiveGLObjects.insert(std::make_pair(pn::BoundingContainerType::BOUNDING_OBB, bounding_box));

	GLfloat sphere_buffer_data[] = {
		0.000000f, -1.000000f, 0.000000f,
		0.723607f, -0.447220f, 0.525725f,
		-0.276388f, -0.447220f, 0.850649f,
		-0.894426f, -0.447216f, 0.000000f,
		-0.276388f, -0.447220f, -0.850649f,
		0.723607f, -0.447220f, -0.525725f,
		0.276388f, 0.447220f, 0.850649f,
		-0.723607f, 0.447220f, 0.525725f,
		-0.723607f, 0.447220f, -0.525725f,
		0.276388f, 0.447220f, -0.850649f,
		0.894426f, 0.447216f, 0.000000f,
		0.000000f, 1.000000f, 0.000000f,
		-0.162456f, -0.850654f, 0.499995f,
		0.425323f, -0.850654f, 0.309011f,
		0.262869f, -0.525738f, 0.809012f,
		0.850648f, -0.525736f, 0.000000f,
		0.425323f, -0.850654f, -0.309011f,
		-0.525730f, -0.850652f, 0.000000f,
		-0.688189f, -0.525736f, 0.499997f,
		-0.162456f, -0.850654f, -0.499995f,
		-0.688189f, -0.525736f, -0.499997f,
		0.262869f, -0.525738f, -0.809012f,
		0.951058f, 0.000000f, 0.309013f,
		0.951058f, 0.000000f, -0.309013f,
		0.000000f, 0.000000f, 1.000000f,
		0.587786f, 0.000000f, 0.809017f,
		-0.951058f, 0.000000f, 0.309013f,
		-0.587786f, 0.000000f, 0.809017f,
		-0.587786f, 0.000000f, -0.809017f,
		-0.951058f, 0.000000f, -0.309013f,
		0.587786f, 0.000000f, -0.809017f,
		0.000000f, 0.000000f, -1.000000f,
		0.688189f, 0.525736f, 0.499997f,
		-0.262869f, 0.525738f, 0.809012f,
		-0.850648f, 0.525736f, 0.000000f,
		-0.262869f, 0.525738f, -0.809012f,
		0.688189f, 0.525736f, -0.499997f,
		0.162456f, 0.850654f, 0.499995f,
		0.525730f, 0.850652f, 0.000000f,
		-0.425323f, 0.850654f, 0.309011f,
		-0.425323f, 0.850654f, -0.309011f,
		0.162456f, 0.850654f, -0.499995f
	};

	GLushort sphere_buffer_indices[] = {
		1, 14, 13,
		2, 14, 16,
		1, 13, 18,
		1, 18, 20,
		1, 20, 17,
		2, 16, 23,
		3, 15, 25,
		4, 19, 27,
		5, 21, 29,
		6, 22, 31,
		2, 23, 26,
		3, 25, 28,
		4, 27, 30,
		5, 29, 32,
		6, 31, 24,
		7, 33, 38,
		8, 34, 40,
		9, 35, 41,
		10, 36, 42,
		11, 37, 39,
		13, 15, 3,
		13, 14, 15,
		14, 2, 15,
		16, 17, 6,
		16, 14, 17,
		14, 1, 17,
		18, 19, 4,
		18, 13, 19,
		13, 3, 19,
		20, 21, 5,
		20, 18, 21,
		18, 4, 21,
		17, 22, 6,
		17, 20, 22,
		20, 5, 22,
		23, 24, 11,
		23, 16, 24,
		16, 6, 24,
		25, 26, 7,
		25, 15, 26,
		15, 2, 26,
		27, 28, 8,
		27, 19, 28,
		19, 3, 28,
		29, 30, 9,
		29, 21, 30,
		21, 4, 30,
		31, 32, 10,
		31, 22, 32,
		22, 5, 32,
		26, 33, 7,
		26, 23, 33,
		23, 11, 33,
		28, 34, 8,
		28, 25, 34,
		25, 7, 34,
		30, 35, 9,
		30, 27, 35,
		27, 8, 35,
		32, 36, 10,
		32, 29, 36,
		29, 9, 36,
		24, 37, 11,
		24, 31, 37,
		31, 10, 37,
		38, 39, 12,
		38, 33, 39,
		33, 11, 39,
		40, 38, 12,
		40, 34, 38,
		34, 7, 38,
		41, 40, 12,
		41, 35, 40,
		35, 8, 40,
		42, 41, 12,
		42, 36, 41,
		36, 9, 41,
		39, 42, 12,
		39, 37, 42,
		37, 10, 42
	};

	RenderSystem::glObjects sphere;
	sphere.num_tris = sizeof(sphere_buffer_indices) / sizeof(GLushort);

	for (unsigned int i = 0; i < sphere.num_tris; i++) {
		sphere_buffer_indices[i]--;
	}


	glGenVertexArrays(1, &sphere.VAO);
	glBindVertexArray(sphere.VAO);

	glGenBuffers(1, &sphere.VBO);
	glBindBuffer(GL_ARRAY_BUFFER, sphere.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_buffer_data), sphere_buffer_data, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &sphere.IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere.IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_buffer_indices), sphere_buffer_indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	
	m_primitiveGLObjects.insert(std::make_pair(pn::BoundingContainerType::BOUNDING_SPHERE, sphere));
}

void pn::RenderSystem::shutdown() {
	for (auto i : m_primitiveGLObjects) {
		auto& obj = i.second;
		glDeleteVertexArrays(1, &obj.VAO);
		glDeleteBuffers(1, &obj.VBO);
		glDeleteBuffers(1, &obj.IBO);
	}
}

void pn::RenderSystem::run() {
	DrawCallContainer drawCalls{};

	pn::MatrixStack matrixStack;

	for (auto root_children : m_state->getRootEntity().getChildrenID()) {
		buildDrawCalls(root_children, matrixStack, drawCalls);
	}

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	renderDrawCalls(drawCalls);

	glUseProgram(0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glBindVertexArray(0);

#ifdef _DEBUG
	// Camera position
	const mat4& camera_world_transform = m_state->getEntityWorldTransform(m_activeCamera->getID(), nullptr);

	// Get view transform from camera
	const auto view_transform(glm::inverse(camera_world_transform));
	
	for (auto entity : m_state->m_entities) {
	//	if (entity->getName().getText() == "player") continue;
		auto map_itr = m_state->m_physicsSystem.getBoundingContainers().find(entity->getID());
		if (map_itr != m_state->m_physicsSystem.getBoundingContainers().end()) {
			auto& boundingContainer = map_itr->second;

			GLuint collision_draw_program = m_state->m_resources.getMaterial("collision_container_draw.sp").getGLProgramObject();
			glUseProgram(collision_draw_program);

			int worldIndex = glGetUniformLocation(collision_draw_program, "world");
			const auto& world_matrix = boundingContainer->getTransform();
			glUniformMatrix4fv(worldIndex, 1, GL_FALSE, glm::value_ptr(world_matrix));

			int viewIndex = glGetUniformLocation(collision_draw_program, "view");
			glUniformMatrix4fv(viewIndex, 1, GL_FALSE, glm::value_ptr(view_transform));

			int projIndex = glGetUniformLocation(collision_draw_program, "proj");
			glUniformMatrix4fv(projIndex, 1, GL_FALSE, glm::value_ptr(settings.getProjectionMatrix()));

			const auto& glObjects = m_primitiveGLObjects.find(boundingContainer->getContainerType())->second;

			glBindVertexArray(glObjects.VAO);
			glBindBuffer(GL_ARRAY_BUFFER, glObjects.VBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glObjects.IBO);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDrawElements(GL_TRIANGLES, glObjects.num_tris, GL_UNSIGNED_SHORT, NULL);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			glDisableVertexAttribArray(0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glUseProgram(0);
		}
	}

#endif
}

void pn::RenderSystem::buildDrawCalls(EntityID current_entity_ID, MatrixStack& matrixStack, DrawCallContainer& drawCalls) {
	const auto& current_entity = m_state->getEntity(current_entity_ID);

	// If the entity does not have a position in space, then its children do not, so stop travelling down this branch
	bool hasTransform = current_entity.hasComponents(pn::ComponentType::TRANSFORM);
	if (!hasTransform) {
		return;
	}

	// Push the entity's matrix onto the matrix stack
	const auto& transformComponent = std::dynamic_pointer_cast<pn::TransformComponent>(current_entity.getComponent(pn::ComponentType::TRANSFORM));
	matrixStack.push(transformComponent->getTransformMatrix());

	// If the entity is renderable, create a draw call and add it to the draw call container
	bool hasRender = current_entity.hasComponents(pn::ComponentType::RENDER);
	if (hasRender) {
		auto& renderComponent = std::dynamic_pointer_cast<pn::RenderComponent>(current_entity.getComponent(pn::ComponentType::RENDER));
		auto& material = m_state->m_resources.getMaterial(renderComponent->getMaterialFilename());
		auto& mesh = m_state->m_resources.getMesh(renderComponent->getMeshFilename());

		// world position of entity
		const mat4& worldTransform = m_state->getEntityWorldTransform(current_entity_ID, [&]()->mat4 { return matrixStack.multiply(); });

		// put draw call into container
		pn::DrawCall drawCall({ worldTransform, renderComponent });

		auto& camera = glm::inverse(m_state->getEntityWorldTransform("camera", nullptr));
		int distance = (int)(camera * worldTransform)[3].z;
		distance = glm::abs(distance);
	//	drawCalls.insert({ ( distance << 6) |  (mesh.getVAO() << 2) | material.getMaterialID(), std::move(drawCall) });
		drawCalls.insert({ distance, std::move(drawCall) });
	}

	// Continue visiting the entity's children
	for (auto childID : current_entity.getChildrenID()) {
		buildDrawCalls(childID, matrixStack, drawCalls);
	}

	matrixStack.pop();
}

void pn::RenderSystem::renderDrawCalls(DrawCallContainer& drawCalls) {

	// Camera position
	const mat4& camera_world_transform = m_state->getEntityWorldTransform(m_activeCamera->getID(), nullptr);

	// Get view transform from camera
	const auto view_transform(glm::inverse(camera_world_transform));

	HashValue last_material_rendered = 0;
	HashValue last_mesh_rendered = 0;

	// render each draw call -- it goes in order of material ID because the map is sorted by this
	for (auto& drawCallIter : drawCalls) {
		auto& drawCall = drawCallIter.second;

		auto current_material_hash = drawCall.renderComponent->getMaterialFilename().getHash();
		auto& current_material = m_state->m_resources.getMaterial(current_material_hash);

		auto current_mesh_hash = drawCall.renderComponent->getMeshFilename().getHash();
		auto& current_mesh = m_state->m_resources.getMesh(current_mesh_hash);

		auto current_texture_hash = drawCall.renderComponent->getDiffuse().getHash();
		auto& current_texture = m_state->m_resources.getImage(current_texture_hash);

		// If the current program is not the same as the last one used, set new global uniforms

		if (current_material_hash != last_material_rendered) {
			current_material.setGlobalUniforms(*m_state, m_lights, camera_world_transform, view_transform);
			last_material_rendered = current_material_hash;
		}

		// if rendering a different mesh, change VAO, otherwise keep from last time
		bool swapVAO = current_mesh_hash != last_mesh_rendered;
		last_mesh_rendered = current_mesh_hash;

		current_material.setInstanceUniforms(drawCall.world_transform, *drawCall.renderComponent, current_mesh, current_texture, swapVAO);

		glDrawArraysInstanced(GL_TRIANGLES, 0, current_mesh.getNumVertices(), 1);

		current_material.postRender();
	}
}