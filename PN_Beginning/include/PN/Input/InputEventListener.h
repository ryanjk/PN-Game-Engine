#ifndef INPUT_EVENT_LISTENER_H
#define INPUT_EVENT_LISTENER_H

using Key = int;

namespace pn{
	class InputEventListener {
	public:
		InputEventListener() {}
		virtual ~InputEventListener() {}
		
		virtual void update(double dt) {}

		virtual void onLeftMousePress(double x, double y) {}
		virtual void onLeftMouseRelease(double x, double y) {}
		
		virtual void onRightMousePress(double x, double y) {}
		virtual void onRightMouseRelease(double x, double y) {}

		virtual void onMiddleMousePress(double x, double y) {}
		virtual void onMiddleMouseRelease(double x, double y) {}

		virtual void onKeyPress(Key key) {}
		virtual void onKeyRelease(Key key) {}

		virtual void onMouseMove(double from_x, double from_y, double to_x, double to_y) {}

		virtual void onCursorEnter() {}
		virtual void onCursorExit() {}
	};
}

#endif