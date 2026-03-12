#pragma once
#include <gameMap.h>
#include <raylib.h>
#include <entityIdHolder.h>
#include <drawBackground.h>
#include <player.h>
#include <structure.h>
#include <assetManager.h>

struct Gameplay
{
	GameMap gameMap;
	Camera2D camera = {};
	DrawBackground background;

	int creativeSelectedBlock = Block::dirt;

	Vector2 selectionStart = {};
	Vector2 selectionEnd = {};
	Structure copyStructure;

	char saveName[100] = {};

	Player player;
	EntityHolder entities;

	bool insideInventory = false;

	bool showImgui = false;

	Rectangle getInventoryRectangle(float w, float h);

	void spawnSlime(Vector2 position);

	void spawnDroppedItem(Vector2 position, int type);

	bool init();

	bool update(AssetManager &assetManager);
};