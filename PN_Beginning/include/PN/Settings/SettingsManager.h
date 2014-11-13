#ifndef SETTINGS_MANAGER_H
#define SETTINGS_MANAGER_H

#include "PN/Util/Math.h"

#include <string>

namespace pn {
	class SettingsManager {
	public:
		static SettingsManager g_SettingsManager;

		void startUp(std::string configDataPath);
		void shutdown(std::string configDataPath);

		unsigned int getWindowHeight() const;
		unsigned int getWindowWidth() const;
		float getAspectRatio() const;

		const mat4& getProjectionMatrix() const;
		const mat4& getHUDMatrix() const;

		bool isWindowFullscreen() const;

		bool isVsync() const;


	private:
		SettingsManager();

		unsigned int m_window_width;
		unsigned int m_window_height;
		float m_aspect_ratio;

		mat4 m_projection_matrix;
		mat4 m_hud_matrix;

		bool m_fullscreen;
		bool m_vsync;

		void save_settings(std::string configDataPath);


	};
}

#endif