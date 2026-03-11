#pragma once

#include <physics.h>
#include <raylib.h>
#include <random>
#include <entityAnimation.h>

struct AssetManager;

struct Slime
{
	Slime()
	{
		physics.transform.w = 0.8f;
		physics.transform.h = 0.8f;
	}

	EntityAnimation animation;

	PhysicalEntity physics;

	Vector2 &getPosition()
	{
		return physics.transform.pos;
	}

	void render(AssetManager& assetManager);

	void update(float deltaTime, std::ranlux24_base rng, Vector2 playerPosition);

	enum
	{
		STATE_WONDERING = 0,
		STATE_CHASING,
	};

	int currentState = STATE_WONDERING;
	float changeStateTimer = 1;
	float jumpTimer = 5;
	float moveSpeed = 0;
};