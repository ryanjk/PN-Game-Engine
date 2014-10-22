#ifndef SETTINGS_MANAGER_H
#define SETTINGS_MANAGER_H

#include <string>

namespace pn {
	class SettingsManager {
	public:
		static SettingsManager g_SettingsManager;

		void startUp(std::string configDataPath);
		void shutdown(std::string configDataPath);

		int getWindowHeight();
		int getWindowWidth();
		bool isWindowFullscreen();

		bool isVsync();


	private:
		SettingsManager();

		int m_window_width;
		int m_window_height;
		bool m_fullscreen;

		bool m_vsync;

		void save_settings(std::string configDataPath);


	};
}

#endif