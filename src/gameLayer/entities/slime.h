#pragma once

#include <physics.h>
#include <raylib.h>
#include <random>
#include <entityAnimation.h>
#include <entity.h>

struct AssetManager;

struct Slime : Entity
{
	Slime()
	{
		setColliderSize();

		life = getMaxLife();
	}

	EntityAnimation animation;

	Vector2 &getPosition()
	{
		return physics.transform.pos;
	}

	void render(AssetManager& assetManager) override;

	bool update(float deltaTime, EntityUpdateData entityUpdateData) override;

	int getEntityType() { return EntityType_Slime; }

	Json formatToJson();

	bool loadFromJson(Json & j);

	void setColliderSize()
	{
		physics.transform.w = 0.8f;
		physics.transform.h = 0.8f;
	}

	float getMaxLife() { return 10; }

	enum
	{
		STATE_WONDERING = 0,
		STATE_CHASING,
	};

	int currentState = STATE_WONDERING;
	float changeStateTimer = 1;
	float jumpTimer = 5;
	float moveSpeed = 0;
};