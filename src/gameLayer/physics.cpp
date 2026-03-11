#include "physics.h"
#include "gameMap.h"
#include <raymath.h>

void PhysicalEntity::resolveConstrains(GameMap& mapData)
{
	upTouch = false;
	downTouch = false;
	leftTouch = false;
	rightTouch = false;

	Vector2& pos = transform.pos;

	float distance = Vector2Distance(lastPosition, pos);

	if (distance == 0)
	{
		return;
	}

	float GRANULARITY = 0.8;

	if (distance <= GRANULARITY)
	{
		checkCollisionOnce(pos, mapData);

	}
	else
	{
		Vector2 newPos = lastPosition;
		Vector2 delta = pos - lastPosition;
		delta = Vector2Normalize(delta);
		delta *= GRANULARITY * 0.99;

		do
		{
			newPos += delta;
			Vector2 posTest = newPos;
			checkCollisionOnce(newPos, mapData);

			if (newPos != posTest)
			{
				pos = newPos;
				goto end;
			}
		} while (Vector2Length((newPos + delta) - pos) > GRANULARITY);

		checkCollisionOnce(pos, mapData);
	}

end:

	if (pos.x - transform.w / 2 < 0) { pos.x = transform.w / 2; }
	if (pos.x + transform.w / 2 > mapData.w) { pos.x = mapData.w - transform.w / 2; }
	if (pos.y - transform.h / 2 > mapData.h) { pos.y = mapData.h - transform.h / 2; }

	if (leftTouch && velocity.x < 0) { velocity.x = 0; }
	if (rightTouch && velocity.x > 0) { velocity.x = 0; }

	if (upTouch && velocity.y < 0) { velocity.y = 0; }
	if (downTouch && velocity.y > 0) { velocity.y = 0; }
}

void PhysicalEntity::checkCollisionOnce(Vector2& pos, GameMap &mapData)
{
	Vector2 delta = pos - lastPosition;

	Vector2 newPos = performCollisionOnOneAxis(mapData, {pos.x, lastPosition.y}, { delta.x, 0 });

	pos = performCollisionOnOneAxis(mapData, { newPos.x, pos.y }, { 0, delta.y });
}

Vector2 PhysicalEntity::performCollisionOnOneAxis(GameMap& mapData, Vector2 pos, Vector2 delta)
{
	if (delta.x == 0 && delta.y == 0) { return pos; }

	Vector2 dimensions = { transform.w, transform.h };

	int minX = floor(pos.x - dimensions.x / 2.f - 1);
	int maxX = ceil(pos.x + dimensions.x / 2.f + 1);

	int minY = floor(pos.y - dimensions.y / 2.f - 1);
	int maxY = ceil(pos.y + dimensions.y / 2.f + 1);

	minX = std::max(0, minX);
	minY = std::max(0, minY);
	maxX = std::min(mapData.w , maxX);
	maxY = std::min(mapData.h, maxY);

	for (int y = minY; y < maxY; y++)
		for (int x = minX; x < maxX; x++)
		{
			if (mapData.getBlocUnsafe(x, y).isCollidable())
			{
				Transform2D entity;
				entity.pos = pos;
				entity.w = dimensions.x;
				entity.h = dimensions.y;

				Transform2D block;
				block.pos = { x + 0.5f, y + 0.5f };
				block.w = 1;
				block.h = 1;

				if (entity.intersectTransform(block, -0.00005f))
				{
					if (delta.x != 0)
					{
						if (delta.x < 0)
						{
							leftTouch = 1;
							pos.x = x + 1.f + dimensions.x / 2;
							return pos;
						}
						else
						{
							rightTouch = 1;
							pos.x = x - dimensions.x / 2;
							return pos;
						}
					}
					else if (delta.y != 0)
					{
						if (delta.y < 0)
						{
							upTouch = 1;
							pos.y = y + 1.f + dimensions.y / 2;
							return pos;
						}
						else
						{
							downTouch = 1;
							pos.y = y - dimensions.y / 2;
							return pos;
						}
					}
				}
			}
		}
	return pos;
}