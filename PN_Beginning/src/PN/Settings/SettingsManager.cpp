#include "PN/Settings/SettingsManager.h"

#include <fstream>
#include <iostream>

#define RESOLUTION_SETTING "[resolution]"
#define RENDERING_SETTING "[rendering]"

static void get_setting(std::string& line, std::string& name, std::string& value) {
	std::string delimiter = "=";

	int index = line.find(delimiter);
	name = line.substr(0, index);
	line.erase(0, index + 1);
	value = std::move(line);
}

static inline bool sToBool(const std::string& string) {
	if (string == "false") {
		return false;
	}
	else if (string == "true") {
		return true;
	}
	else {
		return false;
	}
}

pn::SettingsManager pn::SettingsManager::g_SettingsManager;

pn::SettingsManager::SettingsManager() {

}

void pn::SettingsManager::startUp(std::string configDataPath) {
	std::ifstream config;
	config.open(configDataPath.c_str());

	std::string r_string;
	std::string name, value;

	if (config.good()) {
		while (!config.eof()) {
			std::getline(config, r_string);
			if (r_string == RESOLUTION_SETTING) {
				// Set width setting
				std::getline(config, r_string);
				get_setting(r_string, name, value);
				m_window_width = atoi(value.c_str());

				// Set height setting
				std::getline(config, r_string);
				get_setting(r_string, name, value);
				m_window_height = atoi(value.c_str());

				// Set fullscreen setting
				std::getline(config, r_string);
				get_setting(r_string, name, value);
				m_fullscreen = sToBool(value);
			}
			else if (r_string == RENDERING_SETTING) {
				// set vsync setting
				std::getline(config, r_string);
				get_setting(r_string, name, value);
				m_vsync = sToBool(value);
			}
		}
	}

	config.close();
}

void pn::SettingsManager::save_settings(std::string configDataPath) {

}

unsigned int pn::SettingsManager::getWindowWidth() const {
	return m_window_width;
}

unsigned int pn::SettingsManager::getWindowHeight() const {
	return m_window_height;
}

bool pn::SettingsManager::isWindowFullscreen() const {
	return m_fullscreen;
}

bool pn::SettingsManager::isVsync() const {
	return m_vsync;
}