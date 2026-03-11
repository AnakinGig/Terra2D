#include "slime.h"
#include "slime.h"
#include <assetManager.h>
#include "helpers.h"
#include <randomStuff.h>

void Slime::render(AssetManager& assetManager)
{
	auto aabb = getRectangleForEntity(physics.transform, 1, 1);

	DrawTexturePro(
		assetManager.slime,
		getTextureAtlas(animation.positionX, animation.positionY, 32, 32),
		aabb, //dest
		{ 0, 0 }, //origin
		0.0f, // rotation
		WHITE // tint
	);
}

void Slime::update(float deltaTime, std::ranlux24_base rng, Vector2 playerPosition)
{
	changeStateTimer -= deltaTime;

	if (changeStateTimer < 0)
	{
		changeStateTimer = getRandomFloat(rng, 1, 7);

		float distanceToPlayer = Vector2Distance(playerPosition, getPosition());

		if (distanceToPlayer < 20)
		{
			if (getRandomChance(rng, 0.8))
			{
				currentState = STATE_CHASING;
			}
			else
			{
				currentState = STATE_WONDERING;
			}
		}
		else
		{
			currentState = STATE_WONDERING;
		}
	}

	if (physics.downTouch)
	{
		moveSpeed = 0;
		animation.setAnimation(0); //staying
	}
	else 
	{
		animation.setAnimation(1); //in air
	}

	jumpTimer -= deltaTime;

	switch (currentState)
	{
		case STATE_WONDERING:
		{
			if (jumpTimer < 0)
			{
				jumpTimer = getRandomFloat(rng, 3, 12);

				physics.jump(10);
				moveSpeed = getRandomFloat(rng, -7, 7);
			}
		}
		break;

		case STATE_CHASING:
		{
			if (jumpTimer < 0)
			{
				jumpTimer = getRandomFloat(rng, 2, 7);

				physics.jump(10);

				if (playerPosition.x > getPosition().x)
				{
					moveSpeed = getRandomFloat(rng, 1, 7);

				}
				else
				{
					moveSpeed = -getRandomFloat(rng, 1, 7);
				}
			}
		}
		break;
	}

	if (moveSpeed)
	{
		getPosition().x += moveSpeed * deltaTime;
	}

	animation.update(deltaTime, 0.08, 7);
}