#ifndef WINDOW_INIT_H
#define WINDOW_INIT_H

#define GLEW_STATIC

/*
#define GL_VERSION_4_0
#define GL_VERSION_4_1
#define GL_VERSION_4_2
#define GL_VERSION_4_3
#define GL_VERSION_4_4
#define GL_VERSION_4_5
*/

#include "GL/glew.h"
#include "GLFW/glfw3.h"

using Window = GLFWwindow;

namespace pn {
	class WindowManager {
	public:
		static WindowManager g_windowManager;
		
		void startUp(bool fullscreen, unsigned width, unsigned height);
		void shutdown();

		void resize(bool fullscreen, unsigned width, unsigned height);

		Window* getWindow();
		Window* getLoaderWindow();

		bool isFullscreen() const;
		unsigned int getWidth() const;
		unsigned int getHeight() const;

	private:
		WindowManager();
		void updateSize(bool fullscreen, unsigned width, unsigned height);
		
		Window* m_window;
		Window* m_loader_window;

		bool m_fullscreen;
		unsigned int m_width;
		unsigned int m_height;

		bool m_started;
	};
}

#endif