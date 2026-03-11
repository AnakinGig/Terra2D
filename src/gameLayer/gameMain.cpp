#include <raylib.h>
#include <asserts.h>
#include <assetManager.h>
#include <gameMap.h>
#include <helpers.h>
#include <iostream>
#include "gameMain.h"
#include <raymath.h>
#include <worldGenerator.h>
#include <imgui.h>
#include <structure.h>
#include <string>
#include <saveMap.h>
#include <physics.h>
#include <entities/slime.h>
#include <entities/droppedItem.h>
#include <entityIdHolder.h>
#include <randomStuff.h>
#include <player.h>
#include <items.h>
#include <audio.h>
#include <settings.h>

struct GameData
{
	GameMap gameMap;
	Camera2D camera = {};

	int creativeSelectedBlock = Block::dirt;

	Vector2 selectionStart = {};
	Vector2 selectionEnd = {};
	Structure copyStructure;

	char saveName[100] = {};

	Player player;
	EntityHolder entities;

}gameData;

AssetManager assetManager;

bool showImgui = false;

// ========== INIT ==========
void spawnSlime(Vector2 position)
{
	Slime slime;

	slime.physics.teleport(position);

	auto id = gameData.entities.idHolder.getEntityIdAndIncrement();

	gameData.entities.entities[id] = std::make_unique<Slime>(slime);
}

void spawnDroppedItem(Vector2 position, int type)
{
	DroppedItem droppedItem;

	droppedItem.teleport(position);
	droppedItem.itemType = type;

	auto id = gameData.entities.idHolder.getEntityIdAndIncrement();

	gameData.entities.entities[id] = std::make_unique<DroppedItem>(droppedItem);
}

bool initGame()
{
	Audio::init();
	assetManager.loadAll();

	generateWorld(gameData.gameMap);

	gameData.camera.target = { 20, 120 };
	gameData.camera.rotation = 0.0f;
	gameData.camera.zoom = 50;

	gameData.player.physics.teleport({20, 60});
	gameData.player.physics.transform.w = 0.9f;
	gameData.player.physics.transform.h = 1.8f;

	spawnSlime({ 18,60 });

	return true;
}

// ========== UPDATE ==========
bool updateGame()
{
	Audio::update();
	float deltaTime = GetFrameTime();
	if (deltaTime > 1.f / 5) { deltaTime = 1 / 5.f; }

	gameData.camera.offset = { GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };

	ClearBackground({ 75, 75, 150, 255 });

	if (IsKeyPressed(KEY_F10)) { showImgui = !showImgui; }

#pragma region camera movement

	static float CAMERA_SPEED = 10;
	if (IsKeyDown(KEY_A)) { gameData.player.physics.transform.pos.x -= CAMERA_SPEED * GetFrameTime(); }
	if (IsKeyDown(KEY_D)) { gameData.player.physics.transform.pos.x += CAMERA_SPEED * GetFrameTime(); }
	if (IsKeyDown(KEY_W)) { gameData.player.physics.transform.pos.y -= CAMERA_SPEED * GetFrameTime(); }
	if (IsKeyDown(KEY_S)) { gameData.player.physics.transform.pos.y += CAMERA_SPEED * GetFrameTime(); }

	if (IsKeyDown(KEY_SPACE)) { gameData.player.physics.jump(10); }

#pragma endregion

#pragma region entities

	auto updateEntityPhysics = [&](auto& entity, bool applyGravity = true)
		{
			if (applyGravity) { entity.physics.applyGravity(); }

			entity.physics.updateForces(deltaTime);

			entity.physics.resolveConstrains(gameData.gameMap);

			entity.physics.updateFinal();
		};

	//player
	updateEntityPhysics(gameData.player, false);

	gameData.camera.target = gameData.player.physics.transform.pos;

	float zoom = gameData.camera.zoom;

	float screenWidth = GetScreenWidth();
	float screenHeight = GetScreenHeight();

	float halfViewWidth = (screenWidth * 0.5f) / zoom;
	float halfViewHeight = (screenHeight * 0.5f) / zoom;

	float minX = halfViewWidth;
	float maxX = gameData.gameMap.w - halfViewWidth;
	float minY = halfViewHeight;
	float maxY = gameData.gameMap.h - halfViewHeight;

	if (maxX < minX)
	{
		gameData.camera.target.x = gameData.gameMap.w * 0.5f;
	}
	else
	{
		gameData.camera.target.x = Clamp(gameData.camera.target.x, minX, maxX);
	}

	if (maxY < minY)
	{
		gameData.camera.target.y = gameData.gameMap.h * 0.5f;
	}
	else
	{
		gameData.camera.target.y = Clamp(gameData.camera.target.y, minY, maxY);
	}

	//update all entities
	std::ranlux24_base rng(std::random_device{}());

	for (auto it = gameData.entities.entities.begin(); it != gameData.entities.entities.end(); )
	{
		EntityUpdateData entityUpdateData
		{
			gameData.player.getPosition(),
			rng,
			gameData.entities,
			it->first,
		};

		bool shouldKill = false;

		if (!it->second->update(deltaTime, entityUpdateData) || it->second->life <= 0)
		{
			shouldKill = true;
		}

		if (shouldKill)
		{
			it = gameData.entities.entities.erase(it);
		}
		else
		{
			updateEntityPhysics(*it->second);

			it++;
		}
	}

#pragma endregion

	Vector2 worldPos = GetScreenToWorld2D(GetMousePosition(), gameData.camera);
	int blockX = (int)floor(worldPos.x);
	int blockY = (int)floor(worldPos.y);

	if (gameData.creativeSelectedBlock < 0) { gameData.creativeSelectedBlock = 0; }
	if (gameData.creativeSelectedBlock >= Block::BLOCKS_COUNT) { gameData.creativeSelectedBlock = Block::BLOCKS_COUNT - 1; }
	
	//Selection
	if (showImgui)
	{
		if (IsKeyPressed(KEY_ONE)) { gameData.selectionStart = Vector2{ (float)blockX, (float)blockY }; }
		if (IsKeyPressed(KEY_TWO)) { gameData.selectionEnd = Vector2{ (float)blockX, (float)blockY }; }
		if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_V)) 
		{ 
			gameData.copyStructure.pasteIntoMap(gameData.gameMap, Vector2{ (float)blockX, (float)blockY});
		}

		if (gameData.selectionStart.x > gameData.selectionEnd.x) 
		{ 
			std::swap(gameData.selectionStart.x, gameData.selectionEnd.x); 
		}

		if (gameData.selectionStart.y > gameData.selectionEnd.y) 
		{ 
			std::swap(gameData.selectionStart.y, gameData.selectionEnd.y); 
		}
	}

	if (!showImgui)
	{
		// Block breaking
		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
		{
			auto b = gameData.gameMap.getBlocSafe(blockX, blockY);
			if (b)
			{
				if (b->type)
				{
					spawnDroppedItem({ (float)blockX + getRandomFloat(rng, 0.4, 0.6), (float)blockY + 0.5f}, b->type);
					Audio::playSound(Audio::breakBlock);
				}

				*b = {};
			}
		}

		// Block placing
		if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
		{
			auto b = gameData.gameMap.getBlocSafe(blockX, blockY);
			if (b)
			{
				if (b->type != gameData.creativeSelectedBlock)
				{
					b->type = gameData.creativeSelectedBlock;
					Audio::playSound(Audio::placeBlock);
				}
			}
		}

		if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE))
		{
			auto b = gameData.gameMap.getBlocSafe(blockX, blockY);
			if (b)
			{
				gameData.creativeSelectedBlock = b->type;
			}
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

	//Bloc preview
	auto b = gameData.gameMap.getBlocSafe(blockX, blockY);
	if (b && b->type == Block::air) {
		DrawTexturePro(
			assetManager.textures,
			getTextureAtlas(gameData.creativeSelectedBlock, 0, 32, 32), //source
			{ (float)blockX, (float)blockY, 1, 1 }, //dest
			{ 0, 0 },	//origin
			0.0f, //rotation
			{255,255,255,127} //tint
		);
	}
	
	//Selected box
	DrawTexturePro(
		assetManager.frame,
		{ 0, 0, (float)assetManager.frame.width, (float)assetManager.frame.height },
		{ (float)blockX, (float)blockY, 1, 1 },
		{ 0, 0 },	//origin
		0.0f, //rotation
		WHITE //tint
	);

	//Structure selection box
	if (showImgui)
	{
		Rectangle rect;
		rect.x = gameData.selectionStart.x;
		rect.y = gameData.selectionStart.y;
		rect.width = gameData.selectionEnd.x - gameData.selectionStart.x;
		rect.height = gameData.selectionEnd.y - gameData.selectionStart.y;

		rect.width++;
		rect.height++;

		DrawRectangleLinesEx(rect, 0.1, { 20, 101, 250, 145 });
	}

	//slime
	for (auto& e : gameData.entities.entities)
	{
		e.second->render(assetManager);
	}

	//Player sprite
	Transform2D playerSprite = gameData.player.physics.transform;
	playerSprite.w = 1;
	playerSprite.h = 2;

	playerSprite.pos.y -= (playerSprite.h - gameData.player.physics.transform.h) / 2;

	DrawTexturePro(
		assetManager.player,
		{ 0, 0, (float)assetManager.player.width, (float)assetManager.player.height },
		playerSprite.getAABB(),
		{ 0, 0 },	//origin
		0.0f, //rotation
		WHITE //tint
	);

	DrawRectangleLinesEx(gameData.player.physics.transform.getAABB(), 0.1, {20, 101, 250, 120});

	EndMode2D();

	if (showImgui)
	{
		ImGui::Begin("GameControll");

		ImGui::SliderFloat("Camera zoom", &gameData.camera.zoom, 1, 150);
		ImGui::SliderFloat("Camera speed", &CAMERA_SPEED, 5, 200);

		if (ImGui::Button("Spawn Slime"))
		{
			spawnSlime({ 18, 60 });
		}
		ImGui::SameLine();
		if (ImGui::Button("Hurt a slime"))
		{
			for (auto& e : gameData.entities.entities)
			{
				if (e.second->getEntityType() == EntityType::EntityType_Slime)
				{
					e.second->life -= 3;
					break;
				}
			}
		}

		if (ImGui::Button("Copy"))
		{
			gameData.copyStructure.copyFromMap(gameData.gameMap, gameData.selectionStart, gameData.selectionEnd);
		}

		ImGui::InputText("File name", gameData.saveName, sizeof(gameData.saveName));

		if (ImGui::Button("Save"))
		{
			std::string path = RESOURCES_PATH "structures/";
			path += gameData.saveName;
			path += ".bin";

			saveBlockDataToFile(gameData.copyStructure.mapData, gameData.copyStructure.w, gameData.copyStructure.h, path.c_str());
		}
		ImGui::SameLine();

		if (ImGui::Button("Load"))
		{
			std::string path = RESOURCES_PATH "structures/";
			path += gameData.saveName;
			path += ".bin";

			loadBlockDataFromFile(gameData.copyStructure.mapData, gameData.copyStructure.w, gameData.copyStructure.h, path.c_str());
		}

		ImGui::Separator();

		ImGui::SliderFloat("Master volume", &getSettings().masterVolume, 0, 1);
		ImGui::SliderFloat("Sound volume", &getSettings().soundsVolume, 0, 1);
		ImGui::SliderFloat("Music volume", &getSettings().musicVolume, 0, 1);


		if (ImGui::Button("Play music forest"))
		{
			Audio::playMusic(Audio::musicForest);
		}
		ImGui::SameLine();
		if (ImGui::Button("Play music desert"))
		{
			Audio::playMusic(Audio::musicDesert);
		}

		ImGui::Separator();

		for (int i = 0; i < Block::BLOCKS_COUNT; i++)
		{
			auto atlas = getTextureAtlas(i, 0, 32, 32);
			atlas.x /= assetManager.textures.width;
			atlas.width /= assetManager.textures.width;
			atlas.y /= assetManager.textures.height;
			atlas.height /= assetManager.textures.height;

			ImGui::PushID(i);

			ImTextureID tex = (ImTextureID)(intptr_t)assetManager.textures.id;
			if (ImGui::ImageButton(tex,
				{ 35,35 }, { atlas.x, atlas.y },
				{ atlas.x + atlas.width, atlas.y + atlas.height }))
			{
				gameData.creativeSelectedBlock = i;
			}

			ImGui::PopID();

			if (i % 10 != 0)
			{
				ImGui::SameLine();
			}
		}

		ImGui::End();
	}
#pragma	endregion

	DrawFPS(10, 10);

	return true;
}

// ========== CLOSE ==========
void closeGame()
{
}
