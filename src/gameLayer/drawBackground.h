#pragma once

#include <raylib.h>

struct AssetManager;

struct DrawBackground
{
	enum
	{
		none = 0,
		forest,
		desert,
		snow,
		cave,
	};

	int currentBackgroundType = forest;

	int currentTransitionType = none;
	float transitionTime = 0;

	void draw(float deltaTime, AssetManager& assetManager, Camera2D camera, Vector2 mapSize);

	void setBackground(int background);
};