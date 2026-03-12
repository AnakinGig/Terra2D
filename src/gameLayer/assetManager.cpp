#include "assetManager.h"
#include <items.h>

void AssetManager::loadAll()
{
	dirt = LoadTexture(RESOURCES_PATH "dirt.png");
	textures = LoadTexture(RESOURCES_PATH "textures.png");
	frame = LoadTexture(RESOURCES_PATH "frame.png");
	player = LoadTexture(RESOURCES_PATH "player.png");
	slime = LoadTexture(RESOURCES_PATH "slime.png");
	items = LoadTexture(RESOURCES_PATH "items.png");

	forestBG = LoadTexture(RESOURCES_PATH "forestBG.png");
	desertBg = LoadTexture(RESOURCES_PATH "desertBG.png");
	snowBG = LoadTexture(RESOURCES_PATH "snowBG.png");
	caveBG = LoadTexture(RESOURCES_PATH "caveBG.png");

	//0 = default
	feetArmour[0] = LoadTexture(RESOURCES_PATH "body/player_feet.png");
	headArmour[0] = LoadTexture(RESOURCES_PATH "body/player_head.png");
	frontArmour[0] = LoadTexture(RESOURCES_PATH "body/player_front.png");
	backArmour[0] = LoadTexture(RESOURCES_PATH "body/player_back.png");

	headArmour[Item::partyHat] = LoadTexture(RESOURCES_PATH "body/party_hat.png");
	headArmour[Item::sunGlasses] = LoadTexture(RESOURCES_PATH "body/sunglasses.png");

	feetArmour[Item::copperBoots] = LoadTexture(RESOURCES_PATH "body/copper_armour_feet.png");
	headArmour[Item::copperHelmet] = LoadTexture(RESOURCES_PATH "body/copper_armour_head.png");
	frontArmour[Item::copperChestPlate] = LoadTexture(RESOURCES_PATH "body/copper_armour_front.png");
	backArmour[Item::copperChestPlate] = LoadTexture(RESOURCES_PATH "body/copper_armour_back.png");

	feetArmour[Item::ironBoots] = LoadTexture(RESOURCES_PATH "body/iron_armour_feet.png");
	headArmour[Item::ironHelmet] = LoadTexture(RESOURCES_PATH "body/iron_armour_head.png");
	frontArmour[Item::ironChestPlate] = LoadTexture(RESOURCES_PATH "body/iron_armour_front.png");
	backArmour[Item::ironChestPlate] = LoadTexture(RESOURCES_PATH "body/iron_armour_back.png");

	feetArmour[Item::goldBoots] = LoadTexture(RESOURCES_PATH "body/gold_armour_feet.png");
	headArmour[Item::goldHelmet] = LoadTexture(RESOURCES_PATH "body/gold_armour_head.png");
	frontArmour[Item::goldChestPlate] = LoadTexture(RESOURCES_PATH "body/gold_armour_front.png");
	backArmour[Item::goldChestPlate] = LoadTexture(RESOURCES_PATH "body/gold_armour_back.png");

	feetArmour[Item::iceBoots] = LoadTexture(RESOURCES_PATH "body/ice_armour_feet.png");
	headArmour[Item::iceHelmet] = LoadTexture(RESOURCES_PATH "body/ice_armour_head.png");
	frontArmour[Item::iceChestPlate] = LoadTexture(RESOURCES_PATH "body/ice_armour_front.png");
	backArmour[Item::iceChestPlate] = LoadTexture(RESOURCES_PATH "body/ice_armour_back.png");
}

Texture2D AssetManager::getHeadTexture(int item)
{
	auto found = headArmour.find(item);
	if (found != headArmour.end())
	{
		if (found->second.id == 0)
		{
			return headArmour[0];
		}
		return found->second;
	}
	return headArmour[0];
}

Texture2D AssetManager::getBackTexture(int item)
{
	auto found = backArmour.find(item);
	if (found != backArmour.end())
	{
		if (found->second.id == 0)
		{
			return backArmour[0];
		}
		return found->second;
	}
	return backArmour[0];
}

Texture2D AssetManager::getFeetTexture(int item)
{
	auto found = feetArmour.find(item);
	if (found != feetArmour.end())
	{
		if (found->second.id == 0)
		{
			return feetArmour[0];
		}
		return found->second;
	}
	return feetArmour[0];
}

Texture2D AssetManager::getFrontTexture(int item)
{
	auto found = frontArmour.find(item);
	if (found != frontArmour.end())
	{
		if (found->second.id == 0)
		{
			return frontArmour[0];
		}
		return found->second;
	}
	return frontArmour[0];
}
