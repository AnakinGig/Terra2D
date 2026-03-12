#pragma once

#include <raymath.h>

struct Settings
{
	float musicVolume = 0.75;
	float masterVolume = 0.75;
	float soundsVolume = 0.75;

	void sanitize()
	{
		musicVolume = Clamp(musicVolume, 0.f, 1.f);
		masterVolume = Clamp(masterVolume, 0.f, 1.f);
		soundsVolume = Clamp(soundsVolume, 0.f, 1.f);
	}

	bool operator==(const Settings& other) const
	{
		return musicVolume == other.musicVolume &&
			masterVolume == other.masterVolume &&
			soundsVolume == other.soundsVolume;
	}

	bool operator!=(const Settings& other) const
	{
		return !(*this == other);
	}
};

Settings& getSettings();

void saveSettings();

void loadSettings();

void updateSettings();