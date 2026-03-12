#pragma once

namespace Audio
{
	void init();

	void loadAllMusicAndSounds();

	void playMusic(int index);

	void update();

	void stopAllMusic();

	void playSound(int sound, float volume = 1);

	bool isMusicPlaying();

	enum Sounds
	{
		noneSound = 0,
		placeBlock,
		breakBlock,

		SOUNDS_COUNT
	};

	enum Musics
	{
		noneMusic = 0,
		musicForest,
		musicDesert,
		musicSnow,
		musicCave,

		MUSICS_COUNT
	};

	void setMusic(int music);
};