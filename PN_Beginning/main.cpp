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

void error_callback(int error, const char * description) {
	std::cerr << "GLFW: " << description << std::endl;
}

int main(int argc, char* args[])
{
	// Initialize settings (load them into the manager from config file)
	pn::SettingsManager::g_SettingsManager.startUp("config/config.ini");
	
	bool fullscreen = pn::SettingsManager::g_SettingsManager.isWindowFullscreen();
	auto width = pn::SettingsManager::g_SettingsManager.getWindowWidth();
	auto height = pn::SettingsManager::g_SettingsManager.getWindowHeight();

	// Initialize SDL and GLEW
//	SDL_SetMainReady();
	pn::WindowManager::g_windowManager.startUp(fullscreen, width, height);

	pn::mm::Window* window = pn::WindowManager::g_windowManager.getWindow();

	// Initialize input handling
	// Sets callbacks for, e.g., mouse click, cursor move, etc which forward to the InputHandler
	auto handler = std::make_shared<pn::InputEventHandler>();
	pn::InputManager::g_inputManager.startUp(handler);

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
		exit_listener(pn::mm::Window* window) : m_window(window){ e.type = pn::mm::QUIT_EVENT; };
		void onKeyPress(Key key) { 
			if (key == pn::mm::KEY_ESC) SDL_PushEvent(&e);
			else if (key == pn::mm::KEY_SPACEBAR) {
				if (cursor) {
					pn::mm::setCursorOff(false);
					cursor = false;
				}
				else {
					pn::mm::setCursorOff(true);
					cursor = true;
				}
			}
		}
	private:
		SDL_Event e;
		pn::mm::Window* m_window;
		bool cursor = true;
	};

	auto loudListener = std::make_shared<loud_listener>();
	auto exitListener = std::make_shared<exit_listener>(window);

//	handler->addListener(loudListener);
	handler->addListener(exitListener);

	double current_time = pn::mm::getTime();
	std::cout << "Time to initialize engine: " << current_time << std::endl;
	
	pn::GameStateManager::g_gameStateManager.startUp();

	pn::GameStateManager::g_gameStateManager.setState(std::make_shared<pn::InitialState>("initial.state"));

	const double dt = 0.01667;
	const double MAX_DT = dt * 10;
	double past_time = 0;
	double accumulator = 0;

	double FPS_counter = -current_time;
	int frames_passed = 0;

	pn::mm::InputEvent e;
	bool shouldClose = false;
	while (!shouldClose)
	{
		double frame_time = current_time - past_time;
		past_time = pn::mm::getTime();

		FPS_counter += frame_time;
		if (frames_passed >= 1000) {
			std::cout << "FPS: " <<  frames_passed / FPS_counter << std::endl;
			frames_passed = 0;
			FPS_counter = 0;
		}

		accumulator += frame_time;
		
		if (accumulator > MAX_DT) {
			accumulator = MAX_DT;
		}

		while (accumulator >= dt) {

			// Process input
			while (pn::mm::pollEvent(&e)) {
				if (e.type == pn::mm::QUIT_EVENT) {
					shouldClose = true;
				}
				pn::InputManager::g_inputManager.sendEvent(e);
			}

			pn::InputManager::g_inputManager.update(dt);

			// Update game data
			pn::GameStateManager::g_gameStateManager.notifyState(dt);

			accumulator -= dt;
		}

		// Render game world
		pn::GameStateManager::g_gameStateManager.renderState();

		// Swap buffers
		pn::mm::swapBuffers(window);

		current_time = pn::mm::getTime();

		frames_passed++;
	}

	pn::GameStateManager::g_gameStateManager.shutdown();
	pn::InputManager::g_inputManager.shutdown();
	pn::WindowManager::g_windowManager.shutdown();

}