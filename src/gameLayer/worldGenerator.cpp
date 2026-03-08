#include "worldGenerator.h"

void generateWorld(GameMap& gameMap, int seed)
{
	const int w = 900;
	const int h = 500;

	gameMap.create(w, h);

	int stoneSize = 380;
	int dirtSize = 50;

	for (int x = 0; x < w; x++)
	{
		for (int y = 0; y < h; y++)
		{
			Block b;

			if (y < h - (dirtSize + stoneSize))
			{
				// air
			}
			else if (y == h - (dirtSize + stoneSize))
			{
				b.type = Block::grassBlock;
			}
			else if (y < h - stoneSize)
			{
				b.type = Block::dirt;
			}
			else
			{
				b.type = Block::stone;
			}
			gameMap.getBlocUnsafe(x, y) = b;
		}
	}
}
