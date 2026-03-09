#include "structure.h"
#include <asserts.h>

void Structure::create(int w, int h)
{
	mapData.resize(w * h);

	this->w = w;
	this->h = h;

	for (auto& e : mapData) { e = {}; } // clear all block data
}

Block& Structure::getBlocUnsafe(int x, int y)
{
	permaAssertCommentDevelopement(mapData.size() == w * h, "Map data not initialized");
	permaAssertCommentDevelopement(x >= 0 && y >= 0 && x < w && y < h, "getBlocUnsafe out of bounds error");

	return mapData[x + y * w];
}

Block* Structure::getBlocSafe(int x, int y)
{
	permaAssertCommentDevelopement(mapData.size() == w * h, "Map data not initialized");

	if (x < 0 || y < 0 || x >= w || y >= h) { return nullptr; }

	return &mapData[x + y * w];
}

void Structure::copyFromMap(GameMap& map, Vector2 start, Vector2 end)
{
	if (end.x > map.w) { end.x = map.w - 1; }
	if (start.x > map.w) { start.x = map.w - 1; }

	if (end.y > map.h) { end.y = map.h - 1; }
	if (start.y > map.h) { start.y = map.h - 1; }

	if (end.x < 0) { end.x = 0; }
	if (end.y < 0) { end.y = 0; }

	if (start.x < 0) { start.x = 0; }
	if (start.y < 0) { start.y = 0; }

	if (start.x > end.x) { std::swap(start.x, end.x); }
	if (start.y > end.y) { std::swap(start.y, end.y); }

	Vector2 size = Vector2{ end.x - start.x + 1, end.y - start.y + 1 };

	if (size.x > map.w) { return; }
	if (size.y > map.h) { return; }

	create(size.x, size.y);

	for (int y = 0; y < size.y; y++)
		for (int x = 0; x < size.x; x++)
		{
			getBlocUnsafe(x, y) = map.getBlocUnsafe(x + start.x, y + start.y);
		}
}

void Structure::pasteIntoMap(GameMap& map, Vector2 start)
{
	for (int y = 0; y < h; y++)
		for (int x = 0; x < w; x++)
		{
			auto b = map.getBlocSafe(x + start.x, y + start.y);

			if (b)
			{
				*b = getBlocUnsafe(x, y);
			}
		}
}
