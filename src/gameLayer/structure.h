#pragma once
#include <vector>
#include <blocks.h>

#include <gameMap.h>
#include <raylib.h>

struct Structure
{
	int w = 0;
	int h = 0;

	std::vector<Block> mapData;

	void create(int w, int h);

	Block& getBlocUnsafe(int x, int y);

	Block* getBlocSafe(int x, int y);

	void copyFromMap(GameMap& map, Vector2 start, Vector2 end);

	void pasteIntoMap(GameMap& map, Vector2 start);
};