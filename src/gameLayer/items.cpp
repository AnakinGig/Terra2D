#include "items.h"
#include <assetManager.h>
#include <helpers.h>

static const int ITEMS_START = 10000;

bool isItem(int type)
{
	return type >= ITEMS_START;
}

bool isBlock(int type)
{
	return !isItem(type);
}

Texture getTextureForItemType(int itemType, AssetManager& assetManager)
{
	if (itemType < ITEMS_START)
	{
		//block item
		return assetManager.textures;
	}
	else
	{
		//item
		return assetManager.items;
	}
}

Rectangle getTextureCoordonatesForItemType(int itemType)
{
	if (itemType < ITEMS_START)
	{
		//block item
		return getTextureAtlas(itemType, 4, 32, 32);
	}
	else
	{
		//item
		return getTextureAtlas(itemType - ITEMS_START, 0, 32, 32);
	}
}

