#include "PN/ConcreteStates/LoadingState.h"

#include "PN/Render/RenderFactory.h"

#include "PN/Util/ShaderLoader.h"
#include "PN/Util/ResourceManager.h"
#include "PN/Util/FileType.h"

#include <iostream>
#include <thread>

static auto& resources = pn::ResourceManager::g_resourceManager;

void setUpRenderable(pn::Renderable& r) {
	r.SHADER_v = pn::ResourceManager::g_resourceManager.getVertexShader("vertex_shader.vglsl");
	r.SHADER_f = pn::ResourceManager::g_resourceManager.getFragmentShader("frag_shader.fglsl");
	r.SHADER_program = pn::ShaderLoader::loadProgram(r.SHADER_v, r.SHADER_f);

	r.image = pn::PString("loading_screen.png").getHash();
	r.mesh = pn::PString("loading_screen.obj").getHash();

	glGenVertexArrays(1, &r.VAO);

	glBindVertexArray(r.VAO);
	{
		glGenBuffers(1, &r.VBO_v);
		glBindBuffer(GL_ARRAY_BUFFER, r.VBO_v);
		{
			glBufferData(GL_ARRAY_BUFFER, resources.getMesh(r.mesh).getVertices().size() * sizeof(GLfloat), &resources.getMesh(r.mesh).getVertices()[0], GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &r.VBO_vt);
		glBindBuffer(GL_ARRAY_BUFFER, r.VBO_vt);
		{
			glBufferData(GL_ARRAY_BUFFER, resources.getMesh(r.mesh).getTexes().size() * sizeof(GLfloat), &resources.getMesh(r.mesh).getTexes()[0], GL_STATIC_DRAW);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	glBindVertexArray(0);

	glGenTextures(1, &r.TBO);
	glBindTexture(GL_TEXTURE_2D, r.TBO);
	{
		pn::Image& img = resources.getImage(r.image);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, img.getWidth(), img.getHeight());
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, img.getWidth(), img.getHeight(), GL_RGBA, GL_UNSIGNED_BYTE, &img.getPixels()[0]);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}

pn::LoadingState::LoadingState(pn::PString stateFilename) : pn::GameState(stateFilename) {
	
}

void pn::LoadingState::startUpAssist() {
	// Set state local variables
	setUpRenderable(m_ls_r);

}

void pn::LoadingState::update(double dt) {

}

void pn::LoadingState::render() {
	glClear(GL_COLOR_BUFFER_BIT);

	glBindVertexArray(m_ls_r.VAO);
	glBindTexture(GL_TEXTURE_2D, m_ls_r.TBO);
	glUseProgram(m_ls_r.SHADER_program);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	int alphaLocation = glGetUniformLocation(m_ls_r.SHADER_program, "alpha");
	glUniform1f(alphaLocation, 1.0f);

	glDrawArrays(GL_TRIANGLES, 0, resources.getMesh(m_ls_r.mesh).getVertices().size());

	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	glUseProgram(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}
