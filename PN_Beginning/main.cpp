#include "PN/Window/WindowManager.h"

#include "PN/Settings/SettingsManager.h"

#include "PN/Input/InputManager.h"
#include "PN/Input/InputEventListener.h"
#include "PN/Input/InputEventHandler.h"

#include "PN/GameState/GameStateManager.h"
#include "PN/GameState/GameState.h"

#include "PN/ConcreteStates/InitialState.h"

#include "PN/Util/PString.h"
#include "PN/Util/ResourceManager.h"

#include <iostream>
#include <memory>
#include <thread>
#include <tuple>

void error_callback(int error, const char * description) {
	std::cerr << "GLFW: " << description << std::endl;
}

int main()
{
	// Initialize settings (load them into the manager from config file)
	pn::SettingsManager::g_SettingsManager.startUp("config/config.ini");
	
	bool fullscreen = pn::SettingsManager::g_SettingsManager.isWindowFullscreen();
	int width = pn::SettingsManager::g_SettingsManager.getWindowWidth();
	int height = pn::SettingsManager::g_SettingsManager.getWindowHeight();

	// Initialize GLFW and GLEW
	glfwSetErrorCallback(error_callback);
	pn::WindowManager::g_windowManager.startUp(fullscreen, width, height);

	Window* window = pn::WindowManager::g_windowManager.getWindow();

	// Initialize input handling
	// Sets callbacks for, e.g., mouse click, cursor move, etc which forward to the InputHandler
	auto handler = std::make_shared<pn::InputEventHandler>();
	pn::InputManager::g_inputManager.startUp(window, handler);

	class loud_listener : public pn::InputEventListener {
		void onLeftMousePress(double x, double y) { std::cout << "Left Press: " << x << " " << y << std::endl; }
		void onLeftMouseRelease(double x, double y) { std::cout << "Left Release: " << x << " " << y << std::endl; }

		void onRightMousePress(double x, double y) { std::cout << "Right Press: " << x << " " << y << std::endl; }
		void onRightMouseRelease(double x, double y) { std::cout << "Right Release: " << x << " " << y << std::endl; }

		void onMiddleMousePress(double x, double y) { std::cout << "Middle Press: " << x << " " << y << std::endl; }
		void onMiddleMouseRelease(double x, double y) { std::cout << "Middle Release: " << x << " " << y << std::endl; }

		void onMouseMove(double from_x, double from_y, double to_x, double to_y) {
			std::cout << "From: " << from_x << ", " << from_y <<
				" To: " << to_x << ", " << to_y << std::endl;
		}

		void onKeyPress(Key key) { std::cout << "Key Press: " << (char)key << std::endl; }
		void onKeyRelease(Key key) { std::cout << "Key Release: " << (char)key << std::endl; }

	};
	class exit_listener : public pn::InputEventListener {
	public:
		exit_listener(Window* window) : m_window(window){};
		void onKeyPress(Key key) { 
			if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(pn::WindowManager::g_windowManager.getWindow(), GL_TRUE);
			else if (key == GLFW_KEY_SPACE) {
				if (cursor) {
					glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
					cursor = false;
				}
				else {
					glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
					cursor = true;
				}
			}
		}
	private:
		Window* m_window;
		bool cursor = true;
	};

	auto loudListener = std::make_shared<loud_listener>();
	auto exitListener = std::make_shared<exit_listener>(window);

//	handler->addListener(loudListener);
	handler->addListener(exitListener);

	pn::ResourceManager::g_resourceManager.startUp();

	double current_time = glfwGetTime();
	std::cout << "Time to initialize engine: " << current_time << std::endl;
	
	pn::GameStateManager::g_gameStateManager.startUp();
	auto initState = std::make_shared<pn::InitialState>("initial.state");
	pn::GameStateManager::g_gameStateManager.setState(initState);

	const double dt = 0.01667;
	double past_time = 0;
	double accumulator = 0;

	/*
	mat4 m = glm::rotate(mat4(), glm::radians(90.0f), vec3(1.0f, 0.0f, 0.0f));

	for (int i = 0; i < 4; i++) {
		std::cout << std::endl;
		for (int j = 0; j < 4; j++) {
			std::cout << m[i][j] << " ";
		}
	}
	*/
	while (!glfwWindowShouldClose(window))
	{
		double frame_time = current_time - past_time;
		past_time = glfwGetTime();

		accumulator += frame_time;

		while (accumulator >= dt) {

			// Process input
			glfwPollEvents();
			pn::InputManager::g_inputManager.update(dt);

			// Update game data
			pn::GameStateManager::g_gameStateManager.notifyState(dt);

			accumulator -= dt;
		}

		// Render game world
		pn::GameStateManager::g_gameStateManager.renderState();

		// Swap buffers
		glfwSwapBuffers(window);

		current_time = glfwGetTime();
	}

	pn::GameStateManager::g_gameStateManager.shutdown();
	pn::ResourceManager::g_resourceManager.shutdown();
	pn::InputManager::g_inputManager.shutdown();
	pn::WindowManager::g_windowManager.shutdown();

}