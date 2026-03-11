#pragma once

#include <physics.h>
#include <raylib.h>
#include <random>
#include <entity.h>

struct AssetManager;

struct DroppedItem : Entity
{
	DroppedItem()
	{
		physics.transform.w = 0.8f;
		physics.transform.h = 0.8f;
	}

	int itemType = 0;
	int itemCounter = 1;

	void render(AssetManager& assetManager);

	bool update(float deltaTime, EntityUpdateData entityUpdateData);
	
	int getEntityType() { return EntityType_DroppedItem; }
};