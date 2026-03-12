#pragma once

#include <physics.h>
#include <raylib.h>
#include <random>
#include <memory>
#include <unordered_map>
#include <nlohmann/json.hpp>

using Json = nlohmann::json;

struct AssetManager;
struct EntityHolder;

enum EntityType
{
	EntityType_Player = 0,
	EntityType_Slime,
	EntityType_DroppedItem,
};

struct EntityUpdateData
{
	Vector2 playerPosition = {};

	std::ranlux24_base& rng;
	EntityHolder& entityHolder;

	std::uint64_t ownId = 0;
};

struct Entity
{
	PhysicalEntity physics;
	float life = 1;

	Vector2& getPosition()
	{
		return physics.transform.pos;
	}

	void teleport(Vector2 pos)
	{
		physics.teleport(pos);
	}

	virtual void render(AssetManager& assetManager) = 0;

	virtual bool update(float deltaTime, EntityUpdateData entityUpdateData) = 0;

	virtual int getEntityType() = 0;

	virtual void setColliderSize() = 0;

	virtual float getMaxLife() = 0;

	virtual Json formatToJson() = 0;

	virtual bool loadFromJson(Json& j) = 0;

	void addCommonEntityStuffToJson(Json& json)
	{
		json["physics"] = physics.formatToJson();
		json["life"] = life;

		json["entityType"] = getEntityType();
	}

	bool loadCommonEntityStuffFromJson(Json& json)
	{
		if (json.contains("physics"))
		{
			auto j = json["physics"];

			if (j.is_object())
			{
				if (!physics.loadFromJson(j))
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}

		if (json["life"].is_number())
		{
			life = json["life"];

		}
	}
};