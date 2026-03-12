#pragma once

#include <physics.h>
#include <raylib.h>
#include <entity.h>
#include <entityAnimation.h>
#include <items.h>

struct AssetManager;

struct Player : public Entity
{
	Player()
	{
		setColliderSize();

		life = getMaxLife();
	}

	EntityAnimation animations;

	Vector2 &getPosition()
	{
		return physics.transform.pos;
	}

	void render(AssetManager& assetManager);

	int getEntityType() { return EntityType_Player; };

	bool update(float deltaTime, EntityUpdateData entityUpdateData);

	Json formatToJson();

	bool loadFromJson(Json& j);

	void setColliderSize()
	{
		physics.transform.w = 0.8f;
		physics.transform.h = 1.6f;
	}

	float getMaxLife() { return 50; }

	int armourHead = Item::partyHat;
	int armourChest = Item::goldChestPlate;
	int armourLegs = Item::iceBoots;

	int heldItem = Block::goldBlock;
};