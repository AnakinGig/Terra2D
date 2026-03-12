#pragma once
#include <gameMap.h>
#include <raylib.h>
#include <entityIdHolder.h>
#include <background.h>
#include <player.h>
#include <structure.h>
#include <assetManager.h>

struct Gameplay
{
	GameMap gameMap;
	Camera2D camera = {};
	Background background;

	int creativeSelectedBlock = Block::dirt;

	Vector2 selectionStart = {};
	Vector2 selectionEnd = {};
	Structure copyStructure;

	char saveName[100] = {};

	Player player;
	EntityHolder entities;

	bool insideInventory = false;

	bool showImgui = false;

	// Debug info for biome detection
	struct BiomeDebugInfo {
		int totalBlocks = 0;
		int sandCount = 0;
		int stoneCount = 0;
		int snowCount = 0;
		int iceCount = 0;
		int blocksAbove = 0;
		bool hasCeiling = false;
		bool inSky = false;
	};
	BiomeDebugInfo biomeDebug;

	Rectangle getInventoryRectangle(float w, float h);

	int detectBiome(Vector2 position);

	void spawnSlime(Vector2 position);

	void spawnDroppedItem(Vector2 position, int type);

	bool init();

	bool update(AssetManager &assetManager);
};