#include "droppedItem.h"
#include <assetManager.h>
#include <helpers.h>
#include <entityIdHolder.h>

void DroppedItem::render(AssetManager& assetManager)
{
	float renderSize = isBlock(itemType) ? 1 : 0.5f;
	auto aabb = getRectangleForEntity(physics.transform, renderSize, renderSize);

	float physicSize = isBlock(itemType) ? 0.8 : 0.5f;
	physics.transform.w = physicSize;
	physics.transform.h = physicSize;

	Texture2D texture = getTextureForItemType(itemType, assetManager);
	Rectangle rectangle = getTextureCoordonatesForItemType(itemType);

	DrawTexturePro(
		texture,
		rectangle, //source
		aabb, //dest
		{ 0, 0 }, //origin
		0.0f, // rotation
		WHITE // tint
	);

	// Draw collision box
	//DrawRectangleLinesEx(physics.transform.getAABB(), 0.1, {255, 0, 0, 120});

}

bool DroppedItem::update(float deltaTime, EntityUpdateData entityUpdateData)
{
	for (auto& e : entityUpdateData.entityHolder.entities)
	{
		if (e.first != entityUpdateData.ownId)
		{
			if (e.second->getEntityType() == EntityType::EntityType_DroppedItem)
			{
				DroppedItem* other = reinterpret_cast<DroppedItem*>(e.second.get());

				if (itemType == other->itemType)
				{
					if (Vector2Distance(getPosition(), other->getPosition()) < 0.7)
					{
						other->itemCounter += itemCounter;
						return 0;
					}
				}
			}
		}
	}

	return true;
}