#ifndef PN_MM_H
#define PN_MM_H

#include "SDL/SDL.h"
#undef main

namespace pn {
	namespace mm {
		using Window = SDL_Window;
		using InputEvent = SDL_Event;

		int pollEvent(InputEvent* e);

		int initMultimediaLibrary();
		void shutdownMultimediaLibrary();
		
		Window* createWindow(bool fullscreen, unsigned width, unsigned height);
		void destroyWindow(Window* window);

		void setVsync(bool on);
		void swapBuffers(Window* window);

		void setWindowSize(Window* window, int x, int y);

		void setCursorOff(bool on);
		void getCursorPos(int* x, int* y);
		void moveCursorToPos(Window* window, int x, int y);

		double getTime();

		enum EventEnum {
			QUIT_EVENT = SDL_QUIT,
			WINDOW_EVENT = SDL_WINDOWEVENT,

			KEY_DOWN_EVENT = SDL_KEYDOWN,
			KEY_UP_EVENT = SDL_KEYUP,

			MOUSE_MOVE_EVENT = SDL_MOUSEMOTION,

			MOUSE_BUTTON_DOWN_EVENT = SDL_MOUSEBUTTONDOWN,
			MOUSE_BUTTON_UP_EVENT = SDL_MOUSEBUTTONUP,

			MOUSE_ENTER_WINDOW_EVENT = SDL_WINDOWEVENT_ENTER,
			MOUSE_EXIT_WINDOW_EVENT = SDL_WINDOWEVENT_LEAVE
		};

		enum KeyEnum {
			KEY_A = SDLK_a,
			KEY_S = SDLK_s,
			KEY_D = SDLK_d,
			KEY_W = SDLK_w,

			KEY_ESC = SDLK_ESCAPE,
			KEY_SPACEBAR = SDLK_SPACE,
			KEY_LEFT_SHIFT = SDLK_LSHIFT,

			KEY_PRESS = SDL_PRESSED,
			KEY_RELEASE = SDL_RELEASED
		};

		enum MouseEnum {
			MOUSE_LEFT = SDL_BUTTON_LEFT,
			MOUSE_RIGHT = SDL_BUTTON_RIGHT,
			MOUSE_MIDDLE = SDL_BUTTON_MIDDLE,

			MOUSE_PRESS = SDL_PRESSED,
			MOUSE_RELEASE = SDL_RELEASED
		};
	}

}

#endif