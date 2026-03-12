#include "player.h"
#include "assetManager.h"
#include <helpers.h>

void Player::render(AssetManager& assetManager)
{
	auto aabb = physics.transform.getAABB();

/*DrawTexturePro(
		assetManager.player,
		{0, 0, (float)assetManager.player.width, (float)assetManager.player.height},
		aabb, //dest
		{ 0, 0 }, //origin
		0.0f, // rotation
		WHITE // tint
	);*/

	auto textureUV = getTextureAtlas(animations.positionX, animations.positionY, 32, 64, animations.movingLeft);

	if (heldItem)
	{
		textureUV = getTextureAtlas(animations.positionX, animations.positionY + 3, 32, 64, animations.movingLeft);
	}

	DrawTexturePro(
		assetManager.getBackTexture(armourChest),
		textureUV,
		aabb, //dest
		{ 0, 0 }, //origin
		0.0f, // rotation
		WHITE // tint
	);

	DrawTexturePro(
		assetManager.getFeetTexture(armourLegs),
		textureUV,
		aabb, //dest
		{ 0, 0 }, //origin
		0.0f, // rotation
		WHITE // tint
	);

	DrawTexturePro(
		assetManager.getHeadTexture(armourHead),
		textureUV,
		aabb, //dest
		{ 0, 0 }, //origin
		0.0f, // rotation
		WHITE // tint
	);

	if (heldItem)
	{
		Texture texture = getTextureForItemType(heldItem, assetManager);
		Rectangle textureUVItem = getTextureCoordonatesForItemType(heldItem);

		auto pos = aabb;

		//block
		if (heldItem < 10000)
		{
			pos.width = 0.4;
			pos.height = 0.4;
			if (animations.movingLeft)
			{
				pos.y += 0.6;
				pos.x -= 0.2;
			}
			else
			{
				pos.y += 0.6;
				pos.x += 0.7;
			}
		}
		else
		{
			pos.width = 1;
			pos.height = 1;
			if (animations.movingLeft)
			{
				pos.y += 0.1;
				pos.x -= 0.7;
				textureUVItem = flipTextureAtlasX(textureUVItem);
			}
			else
			{
				pos.y += 0.1;
				pos.x += 0.5;
			}
		}

		DrawTexturePro(
			texture,
			textureUVItem,
			pos, //dest
			{ 0, 0 }, //origin
			0.0f, // rotation
			WHITE // tint

		);
	}

	DrawTexturePro(
		assetManager.getFrontTexture(armourChest),
		textureUV,
		aabb, //dest
		{ 0, 0 }, //origin
		0.0f, // rotation
		WHITE // tint
	);
}

bool Player::update(float deltaTime, EntityUpdateData entityUpdateData)
{
	return true;
}

Json Player::formatToJson()
{
	Json j;
	addCommonEntityStuffToJson(j);

	return j;
}

bool Player::loadFromJson(Json& j)
{
	*this = {};

	bool rez = loadCommonEntityStuffFromJson(j);

	setColliderSize();

	return rez;
}
