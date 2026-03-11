#pragma once

#include <physics.h>
#include <raylib.h>
#include <entity.h>

struct AssetManager;

struct Player : public Entity
{
	Player()
	{
		physics.transform.w = 0.8f;
		physics.transform.h = 1.6f;

		life = getMaxLife();
	}

	Vector2 &getPosition()
	{
		return physics.transform.pos;
	}

	void render(AssetManager& assetManager);

	int getEntityType() { return EntityType_Player; };

	bool update(float deltaTime, EntityUpdateData entityUpdateData);

	float getMaxLife() { return 50; }
};