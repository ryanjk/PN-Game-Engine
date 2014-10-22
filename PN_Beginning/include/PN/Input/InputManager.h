#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include "PN/Window/WindowManager.h"
#include "PN/Input/InputEventHandler.h"
#include <memory>

using HandlerPointer = std::shared_ptr < pn::InputEventHandler > ;
using ListenerPointer = std::shared_ptr < pn::InputEventListener >;

namespace pn {
	class InputManager {
	public:
		static InputManager g_inputManager;

		void startUp(Window* window, HandlerPointer handler);
		void shutdown();

		void update(double dt);

		void setInputEventHandler(HandlerPointer handler);
		const HandlerPointer getInputHandler();

	private:
		InputManager();
		HandlerPointer m_handler;

		static void mouse_move_callback(Window* window, double x, double y);
		static void curson_enter_callback(Window* window, int enter);
		static void mouse_button_callback(Window* window, int button, int action, int mods);
		static void keyboard_key_callback(Window* window, int key, int scancode, int action, int mods);
	};
}

#endif