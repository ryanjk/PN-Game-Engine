#ifndef SETTINGS_MANAGER_H
#define SETTINGS_MANAGER_H

#include <string>

namespace pn {
	class SettingsManager {
	public:
		static SettingsManager g_SettingsManager;

		void startUp(std::string configDataPath);
		void shutdown(std::string configDataPath);

		unsigned int getWindowHeight() const;
		unsigned int getWindowWidth() const;
		bool isWindowFullscreen() const;

		bool isVsync() const;


	private:
		SettingsManager();

		unsigned int m_window_width;
		unsigned int m_window_height;
		bool m_fullscreen;

		bool m_vsync;

		void save_settings(std::string configDataPath);


	};
}

#endif