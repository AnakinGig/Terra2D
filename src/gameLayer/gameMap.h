#pragma once
#include <vector>
#include <blocks.h>

struct GameMap
{
	int w = 0;
	int h = 0;

	std::vector<Block> mapData;

	void create(int w, int h);

	Block &getBlocUnsafe(int x, int y);

	Block *getBlocSafe(int x, int y);

	int desertStart = 0;
	int desertEnd = 0;
};