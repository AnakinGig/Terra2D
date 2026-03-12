#include "settings.h"
#include <fstream>
#include <filesystem>
#include <raymath.h>
#include <nlohmann/json.hpp>

Settings settings;

Settings settingsOld;

Settings& getSettings()
{
	return settings;
}

using Json = nlohmann::json;

void saveSettings()
{
	Json j;

	j["masterVolume"] = settings.masterVolume;
	j["musicVolume"] = settings.musicVolume;
	j["soundsVolume"] = settings.soundsVolume;

	std::error_code error;
	std::filesystem::create_directory(RESOURCES_PATH "../settings", error);

	std::ofstream f(RESOURCES_PATH "../settings/settings.txt");
	f << j.dump(2);
	f.close();
}

void loadSettings()
{
	settings = {};

	std::ifstream f(RESOURCES_PATH "../settings/settings.txt");
	if (!f.is_open()) { return; }

	Json j;
	j = Json::parse(f, nullptr, /*allow_exceptions=*/false);

	if (j["masterVolume"].is_number())
		settings.masterVolume = j["masterVolume"].get<float>();
	
	if (j["musicVolume"].is_number())
		settings.musicVolume = j["musicVolume"].get<float>();

	if (j["soundsVolume"].is_number())
		settings.soundsVolume = j["soundsVolume"].get<float>();

	settings.sanitize();

	f.close();
}

void updateSettings()
{
	if (settings != settingsOld)
	{
		saveSettings();
	}
	settingsOld = settings;
}