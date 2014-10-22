#ifndef INPUT_EVENT_HANDLER_H
#define INPUT_EVENT_HANDLER_H

#include <vector>
#include <memory>
#include "PN/Input/InputEventListener.h"

using ListenerPointer = std::shared_ptr < pn::InputEventListener > ;
using ListenerContainer = std::vector<ListenerPointer>;
using Key = int;

namespace pn {
	class InputEventHandler {
	public:
		InputEventHandler();

		void addListener(ListenerPointer listener);
		void removeListener(ListenerPointer listener);

		void update(double dt);

		void handleLeftMousePress(double x, double y);
		void handleLeftMouseRelease(double x, double y);

		void handleRightMousePress(double x, double y);
		void handleRightMouseRelease(double x, double y);

		void handleMiddleMousePress(double x, double y);
		void handleMiddleMouseRelease(double x, double y);

		void handleKeyPress(Key key);
		void handleKeyRelease(Key key);

		void handleMouseMove(double from_x, double from_y, double to_x, double to_y);

		void handleCursorEnter();
		void handleCursorExit();

	private:
		ListenerContainer m_listeners;
	};
}

#endif