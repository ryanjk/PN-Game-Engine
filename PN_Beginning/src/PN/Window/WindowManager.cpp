#include "PN/Window/WindowManager.h"
#include "PN/Settings/SettingsManager.h"
#include "iostream"

pn::WindowManager pn::WindowManager::g_windowManager;

pn::WindowManager::WindowManager() {
	m_started = false;
}

void pn::WindowManager::startUp(bool fullscreen, unsigned int width, unsigned int height) {
	
	if (m_started) return;

	m_started = true;
	
	mm::initMultimediaLibrary();
	
	mm::Window* window;
	window = mm::createWindow(fullscreen, width, height);
	
	// Disable vsync
	if (!pn::SettingsManager::g_SettingsManager.isVsync()) {
		mm::setVsync(false);
	}

	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK) {
		std::cout << "Could not initialize GLEW: " << glewGetErrorString(glewError) << std::endl;
		exit(-1);
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_MULTISAMPLE);
	glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	pn::mm::swapBuffers(window);

	m_window = window;
	updateSize(fullscreen, width, height);
	
	pn::mm::moveCursorToPos(m_window, m_width / 2, m_height / 2);

	std::cout << glGetString(GL_RENDERER) << std::endl;
	std::cout << glGetString(GL_VERSION) << std::endl;
	std::cout << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	GLint iMultiSample, iNumSamples;
	glGetIntegerv(GL_SAMPLE_BUFFERS, &iMultiSample);
	glGetIntegerv(GL_SAMPLES, &iNumSamples);

	std::cout << iMultiSample << " " << iNumSamples << std::endl;

}

void pn::WindowManager::shutdown() {
	mm::destroyWindow(m_window);
	mm::shutdownMultimediaLibrary();
}

void pn::WindowManager::resize(bool fullscreen, unsigned width, unsigned height) {
	mm::setWindowSize(m_window, width, height);
	updateSize(fullscreen, width, height);
}

void pn::WindowManager::updateSize(bool fullscreen, unsigned width, unsigned height) {
	m_fullscreen = fullscreen;
	m_width = width;
	m_height = height;
}

pn::mm::Window* pn::WindowManager::getWindow() {
	return m_window;
}

bool pn::WindowManager::isFullscreen() const {
	return m_fullscreen;
}

unsigned int pn::WindowManager::getHeight() const {
	return m_height;
}

unsigned int pn::WindowManager::getWidth() const {
	return m_width;
}
