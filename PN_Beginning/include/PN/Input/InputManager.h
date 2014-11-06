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

		void startUp(HandlerPointer handler);
		void shutdown();

		void sendEvent(mm::InputEvent inputEvent);
		void update(double dt);

		void setInputEventHandler(HandlerPointer handler);
		const HandlerPointer getInputHandler();

	private:
		InputManager();
		HandlerPointer m_handler;

		static void mouse_move_callback(double x, double y);
		static void cursor_enter_callback(int enter);
		static void mouse_button_callback(int button, int action);
		static void keyboard_key_callback(int key, int action);
	};
}

#endif