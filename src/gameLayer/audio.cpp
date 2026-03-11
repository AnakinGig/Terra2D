#include "audio.h"
#include "audio.h"
#include "audio.h"
#include "audio.h"
#include <raylib.h>
#include <vector>
#include <cmath>
#include <asserts.h>
#include <settings.h>
#include <raymath.h>

namespace Audio
{
	void init()
	{
		InitAudioDevice();
		SetMasterVolume(0.9);

		loadAllMusicAndSounds();
	}

	std::vector<Music> allMusics;
	int currentMusicPlaying = 0;

	std::vector<Sound> allSounds;

	void loadAllMusicAndSounds()
	{
		allMusics.push_back({}); //noneMusic

		auto loadMusic = [&](const char* path)
			{
				Music m;

				m = LoadMusicStream(path);

				if (m.stream.buffer)
				{
					m.looping = true;
					allMusics.push_back(m);
				}
				else
				{
					allMusics.push_back({}); //push empty sound
				}
			};

		loadMusic(RESOURCES_PATH "music/forest.ogg");
		loadMusic(RESOURCES_PATH "music/desert.ogg");
		loadMusic(RESOURCES_PATH "music/snow.ogg");
		loadMusic(RESOURCES_PATH "music/cave.ogg");

		permaAssertComment(allMusics.size() == MUSICS_COUNT, "Forgot to add a music here!");

		auto loadSound = [&](const char* path)
		{
				Sound s;

			s = LoadSound(path);

			if (s.stream.buffer)
			{
				allSounds.push_back(s);
			}
			else
			{
				allSounds.push_back({}); //push empty sound
			}
		};

		allSounds.push_back({}); //noneSound

		loadSound(RESOURCES_PATH "sounds/place.ogg");
		loadSound(RESOURCES_PATH "sounds/break.ogg");

		permaAssertComment(allSounds.size() == SOUNDS_COUNT, "Forgot to add a sound here!");
	}

	void playMusic(int index)
	{
		if (allMusics.size() <= index) { return; }
		if (currentMusicPlaying == index) { return; }

		StopMusicStream(allMusics[currentMusicPlaying]);

		allMusics[index].looping = true;
		PlayMusicStream(allMusics[index]);
		SetMusicVolume(allMusics[index], std::pow(getSettings().musicVolume * getSettings().masterVolume, 1.0));

		currentMusicPlaying = index;
	}

	void update()
	{
		if (!isMusicPlaying())
		{
			currentMusicPlaying = 0;
			return;
		}

		SetMusicVolume(allMusics[currentMusicPlaying], std::pow(getSettings().musicVolume * getSettings().masterVolume, 1.0));
		UpdateMusicStream(allMusics[currentMusicPlaying]);
	}

	void stopAllMusic()
	{
		StopMusicStream(allMusics[currentMusicPlaying]);
		currentMusicPlaying = 0;
	}

	void playSound(int sound, float volume)
	{
		if (sound <= noneSound || sound >= SOUNDS_COUNT) { return; }

		volume = Clamp(volume, 0, 1);

		volume *= getSettings().masterVolume * getSettings().masterVolume;
		volume *= getSettings().soundsVolume * getSettings().soundsVolume;

		SetSoundVolume(allSounds[sound], volume);
		PlaySound(allSounds[sound]);
	}
	bool Audio::isMusicPlaying()
	{
		if (!currentMusicPlaying) { return 0; }
		return IsAudioStreamPlaying(allMusics[currentMusicPlaying].stream);
	}
}
