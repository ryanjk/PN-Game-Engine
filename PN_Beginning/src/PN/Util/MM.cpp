#include "PN/Util/MM.h"

#include <iostream>

int pn::mm::pollEvent(pn::mm::InputEvent* e) {
	return SDL_PollEvent(e);
}

int pn::mm::initMultimediaLibrary() {
	auto error = SDL_Init(SDL_INIT_VIDEO);
	if (error < 0) {
		std::cout << "SDL couldn't initialize: " << SDL_GetError() << std::endl;
		exit(-1);
	}
	return error;
}

void pn::mm::shutdownMultimediaLibrary() {
	SDL_Quit();
}

pn::mm::Window* pn::mm::createWindow(bool fullscreen, unsigned width, unsigned height) {
	
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	
	pn::mm::Window* window = NULL;

	Uint32 window_flags = SDL_WINDOW_OPENGL;
	if (fullscreen) {
		window_flags = window_flags | SDL_WINDOW_FULLSCREEN;
	}

	window = SDL_CreateWindow("PN_Beginning", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, window_flags);

	if (window == NULL) {
		std::cout << "SDL Could not create window: " << std::endl;
		exit(-1);
	}

	SDL_GLContext glContext = SDL_GL_CreateContext(window);

	if (glContext == NULL) {
		std::cout << "SDL couldn't create GL context: " << SDL_GetError() << std::endl;
		exit(-1);
	}

	setCursorOff(false);

	return window;
}

void pn::mm::destroyWindow(pn::mm::Window* window) {
	SDL_DestroyWindow(window);
}

void pn::mm::setVsync(bool on) {
	if (on) {
		if (SDL_GL_SetSwapInterval(1) < 0) {
			std::cout << "SDL couldn't set Vsync: " << SDL_GetError() << std::endl;
		}
	}
	else {
		SDL_GL_SetSwapInterval(0);
	}
}

void pn::mm::swapBuffers(pn::mm::Window* window) {
	SDL_GL_SwapWindow(window);
}

void pn::mm::setWindowSize(pn::mm::Window* window, int x, int y) {
	SDL_SetWindowSize(window, x, y);
}

void pn::mm::setCursorOff(bool on) {
	int isOn = on ? 1 : 0;
	SDL_ShowCursor(isOn);
}

void pn::mm::getCursorPos(int* x, int* y) {
	SDL_GetMouseState(x, y);
}

void pn::mm::moveCursorToPos(pn::mm::Window* window, int x, int y) {
	SDL_WarpMouseInWindow(window, x, y);
}

double pn::mm::getTime() {
	return SDL_GetTicks() / 1000.0f;
}