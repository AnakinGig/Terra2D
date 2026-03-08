#include <raylib.h>
#include <asserts.h>
#include <assetManager.h>
#include <gameMap.h>
#include <helpers.h>
#include <iostream>
#include "gameMain.h"
#include <raymath.h>

struct GameData
{
	GameMap gameMap;
	Camera2D camera;

}gameData;

AssetManager assetManager;

bool initGame()
{
	assetManager.loadAll();

	gameData.gameMap.create(700, 500);

	for (int y = 0; y < gameData.gameMap.h; y++)
		for (int x = 0; x < gameData.gameMap.w; x++)
		{
			gameData.gameMap.getBlocUnsafe(x, y).type = Block::dirt;
		}

	gameData.camera.target = { 0, 0 };
	gameData.camera.rotation = 0.0f;
	gameData.camera.zoom = 50.0f;

	return true;
}

bool updateGame()
{
	float deltaTime = GetFrameTime();
	if (deltaTime > 1.f / 5) { deltaTime = 1 / 5.f; }

	gameData.camera.offset = { GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };

	ClearBackground({ 75, 75, 150, 255 });

#pragma region camera movement

	if (IsKeyDown(KEY_A)) { gameData.camera.target.x -= 7.0f * deltaTime; }
	if (IsKeyDown(KEY_D)) { gameData.camera.target.x += 7.0f * deltaTime; }
	if (IsKeyDown(KEY_W)) { gameData.camera.target.y -= 7.0f * deltaTime; }
	if (IsKeyDown(KEY_S)) { gameData.camera.target.y += 7.0f * deltaTime; }

#pragma endregion

	Vector2 worldPos = GetScreenToWorld2D(GetMousePosition(), gameData.camera);
	int blockX = (int)floor(worldPos.x);
	int blockY = (int)floor(worldPos.y);

	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
	{
		auto b = gameData.gameMap.getBlocSafe(blockX, blockY);
		if (b)
		{
			*b = {};
		}
	}

	if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
	{
		auto b = gameData.gameMap.getBlocSafe(blockX, blockY);
		if (b)
		{
			b->type = Block::grassBlock;
		}
	}

#pragma region draw world
	BeginMode2D(gameData.camera);

	Vector2 topLeftView = GetScreenToWorld2D({ 0, 0 }, gameData.camera);
	Vector2 bottomRightView = GetScreenToWorld2D({ (float)GetScreenWidth(), (float)GetScreenHeight() }, gameData.camera);

	int startXView = (int)floorf(topLeftView.x - 1);
	int endXView = (int)ceilf(bottomRightView.x + 1);
	int startYView = (int)floorf(topLeftView.y - 1);
	int endYView = (int)ceilf(bottomRightView.y + 1);

	startXView = Clamp(startXView, 0, gameData.gameMap.w - 1);
	endXView = Clamp(endXView, 0, gameData.gameMap.w - 1);

	startYView = Clamp(startYView, 0, gameData.gameMap.h - 1);
	endYView = Clamp(endYView, 0, gameData.gameMap.h - 1);

	for (int y = startYView; y < endYView; y++)
		for (int x = startXView; x < endXView; x++)
		{
			auto& b = gameData.gameMap.getBlocUnsafe(x, y);

			if (b.type != Block::air)
			{
				DrawTexturePro(
					assetManager.textures, 
					getTextureAtlas(b.type, 0, 32, 32), //source
					{ (float)x, (float)y, 1, 1 }, //dest
					{0, 0},	//origin
					0.0f, //rotation
					WHITE //tint
				);
			}
		}

	DrawTexturePro(
		assetManager.frame,
		{ 0, 0, (float)assetManager.frame.width, (float)assetManager.frame.height },
		{ (float)blockX, (float)blockY, 1, 1 },
		{ 0, 0 },	//origin
		0.0f, //rotation
		WHITE //tint
	);

	EndMode2D();
#pragma	endregion

	DrawFPS(10, 10);

	return true;
}

void closeGame()
{
}
