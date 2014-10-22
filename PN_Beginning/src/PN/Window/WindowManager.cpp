#include "PN/Window/WindowManager.h"
#include "PN/Settings/SettingsManager.h"
#include "iostream"

pn::WindowManager pn::WindowManager::g_windowManager;

pn::WindowManager::WindowManager() {
	m_started = false;
}

void pn::WindowManager::startUp(bool fullscreen, unsigned width, unsigned height) {
	
	if (m_started) return;

	m_started = true;
	
	auto err = glfwInit();
	if (err == GL_FALSE) {
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_VISIBLE, GL_TRUE);
	Window* window;

	if (!fullscreen) {
		window = glfwCreateWindow(width, height, "PN Beginning", nullptr, NULL);
	}
	else {
		window = glfwCreateWindow(width, height, "PN Beginning", glfwGetPrimaryMonitor(), NULL);
	}

	glfwMakeContextCurrent(window);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
	// Disable vsync
	if (!pn::SettingsManager::g_SettingsManager.isVsync()) {
		glfwSwapInterval(0);
	}

	glewExperimental = GL_TRUE;
	glewInit();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glfwSwapBuffers(window);

	m_window = window;
	updateSize(fullscreen, width, height);

	glfwSetCursorPos(m_window, m_width / 2, m_height / 2);

}

void pn::WindowManager::shutdown() {
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

void pn::WindowManager::resize(bool fullscreen, unsigned width, unsigned height) {
	glfwSetWindowSize(m_window, width, height);
	updateSize(fullscreen, width, height);
}

void pn::WindowManager::updateSize(bool fullscreen, unsigned width, unsigned height) {
	m_fullscreen = fullscreen;
	m_width = width;
	m_height = height;
}

Window* pn::WindowManager::getWindow() {
	return m_window;
}

Window* pn::WindowManager::getLoaderWindow() {
	return m_loader_window;
}

bool pn::WindowManager::isFullscreen() {
	return m_fullscreen;
}

unsigned pn::WindowManager::getHeight() {
	return m_height;
}

unsigned pn::WindowManager::getWidth() {
	return m_width;
}
