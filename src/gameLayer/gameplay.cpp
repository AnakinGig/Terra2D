#include "gameplay.h"
#include <entities/droppedItem.h>
#include <ui.h>
#include <audio.h>
#include <settings.h>
#include <worldGenerator.h>
#include <helpers.h>
#include <imgui.h>
#include <saveMap.h>
#include <physics.h>
#include <randomStuff.h>

Rectangle Gameplay::getInventoryRectangle(float w, float h)
{
	Rectangle inventoryRectangle;

	inventoryRectangle.height = h * 0.3f;
	inventoryRectangle.width = inventoryRectangle.height * 3;

	inventoryRectangle = placeRectangleTopLeftCorner(inventoryRectangle, w);

	float maxWidth = w * 0.9f;
	if (inventoryRectangle.width > maxWidth)
	{
		float scaleFactor = maxWidth / inventoryRectangle.width;
		inventoryRectangle.width *= scaleFactor;
		inventoryRectangle.height *= scaleFactor;
	}

	inventoryRectangle.x += w * 0.01f;
	inventoryRectangle.y += h * 0.01f;

	return inventoryRectangle;
}

void Gameplay::spawnSlime(Vector2 position)
{
	Slime slime;

	slime.physics.teleport(position);

	auto id = entities.idHolder.getEntityIdAndIncrement();

	entities.entities[id] = std::make_unique<Slime>(slime);
}

void Gameplay::spawnDroppedItem(Vector2 position, int type)
{
	DroppedItem droppedItem;

	droppedItem.teleport(position);
	droppedItem.itemType = type;

	auto id = entities.idHolder.getEntityIdAndIncrement();

	entities.entities[id] = std::make_unique<DroppedItem>(droppedItem);
}

bool Gameplay::init()
{
	generateWorld(gameMap);

	camera.target = { 20, 120 };
	camera.rotation = 0.0f;
	camera.zoom = 50;

	player.physics.teleport({ 20, 60 });

	loadWorld(gameMap, entities, player);

	return true;
}

bool Gameplay::update(AssetManager& assetManager)
{
	Audio::update();
	updateSettings();

	float deltaTime = GetFrameTime();
	if (deltaTime > 1.f / 5) { deltaTime = 1 / 5.f; }

	camera.offset = { GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };

	ClearBackground({ 75, 75, 150, 255 });

	if (IsKeyPressed(KEY_F10)) { showImgui = !showImgui; }

#pragma region camera movement

	static float CAMERA_SPEED = 10;
	static bool creative = false;

	bool moving = false;
	bool falling = false;

	if (IsKeyDown(KEY_A))
	{
		player.physics.transform.pos.x -= CAMERA_SPEED * GetFrameTime();
		moving = true;
		player.animations.movingLeft = true;

	}
	if (IsKeyDown(KEY_D))
	{
		player.physics.transform.pos.x += CAMERA_SPEED * GetFrameTime();
		moving = true;
		player.animations.movingLeft = false;

	}
	if (creative)
	{
		if (IsKeyDown(KEY_W))
		{
			player.physics.transform.pos.y -= CAMERA_SPEED * GetFrameTime();
			moving = true;
		}

		if (IsKeyDown(KEY_S))
		{
			player.physics.transform.pos.y += CAMERA_SPEED * GetFrameTime();
			moving = true;
		}
	}

	if (IsKeyDown(KEY_SPACE))
	{
		player.physics.jump(12.0);
	}

	if (player.physics.downTouch)
	{
		falling = false;
	}
	else
	{
		falling = true;
	}

	if (falling)
	{
		player.animations.setAnimation(2);

	}
	else if (moving)
	{
		player.animations.setAnimation(1);
	}
	else
	{
		player.animations.setAnimation(0);
	}

	player.animations.update(deltaTime, 0.08, 7);

#pragma endregion

#pragma region entities

	auto updateEntityPhysics = [&](auto& entity, bool applyGravity = true)
		{
			if (applyGravity) { entity.physics.applyGravity(); }

			entity.physics.updateForces(deltaTime);

			entity.physics.resolveConstrains(gameMap);

			entity.physics.updateFinal();
		};

	//player
	updateEntityPhysics(player, !creative);

	camera.target = player.physics.transform.pos;

	float zoom = camera.zoom;

	float screenWidth = GetScreenWidth();
	float screenHeight = GetScreenHeight();

	float halfViewWidth = (screenWidth * 0.5f) / zoom;
	float halfViewHeight = (screenHeight * 0.5f) / zoom;

	float minX = halfViewWidth;
	float maxX = gameMap.w - halfViewWidth;
	float minY = halfViewHeight;
	float maxY = gameMap.h - halfViewHeight;

	if (maxX < minX)
	{
		camera.target.x = gameMap.w * 0.5f;
	}
	else
	{
		camera.target.x = Clamp(camera.target.x, minX, maxX);
	}

	if (maxY < minY)
	{
		camera.target.y = gameMap.h * 0.5f;
	}
	else
	{
		camera.target.y = Clamp(camera.target.y, minY, maxY);
	}

	//update all entities
	std::ranlux24_base rng(std::random_device{}());

	for (auto it = entities.entities.begin(); it != entities.entities.end(); )
	{
		EntityUpdateData entityUpdateData
		{
			player.getPosition(),
			rng,
			entities,
			it->first,
		};

		bool shouldKill = false;

		if (!it->second->update(deltaTime, entityUpdateData) || it->second->life <= 0)
		{
			shouldKill = true;
		}

		if (shouldKill)
		{
			it = entities.entities.erase(it);
		}
		else
		{
			updateEntityPhysics(*it->second);

			it++;
		}
	}

#pragma endregion

	bool insideInventoryMenu = 0;
	Rectangle inventoryRectangle = getInventoryRectangle(GetScreenWidth(), GetScreenHeight());

	if (insideInventory && CheckCollisionPointRec(GetMousePosition(), inventoryRectangle))
	{
		insideInventoryMenu = true;
	}

	Vector2 worldPos = GetScreenToWorld2D(GetMousePosition(), camera);
	int blockX = (int)floor(worldPos.x);
	int blockY = (int)floor(worldPos.y);

	if (creativeSelectedBlock < 0) { creativeSelectedBlock = 0; }
	if (creativeSelectedBlock >= Block::BLOCKS_COUNT) { creativeSelectedBlock = Block::BLOCKS_COUNT - 1; }

	//Selection
	if (showImgui)
	{
		if (IsKeyPressed(KEY_ONE)) { selectionStart = Vector2{ (float)blockX, (float)blockY }; }
		if (IsKeyPressed(KEY_TWO)) { selectionEnd = Vector2{ (float)blockX, (float)blockY }; }
		if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_V))
		{
			copyStructure.pasteIntoMap(gameMap, Vector2{ (float)blockX, (float)blockY });
		}

		if (selectionStart.x > selectionEnd.x)
		{
			std::swap(selectionStart.x, selectionEnd.x);
		}

		if (selectionStart.y > selectionEnd.y)
		{
			std::swap(selectionStart.y, selectionEnd.y);
		}
	}

	if (!showImgui)
	{
		if (!insideInventoryMenu)
		{
			// Block breaking
			if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
			{
				auto b = gameMap.getBlocSafe(blockX, blockY);
				if (b)
				{
					if (b->type)
					{
						spawnDroppedItem({ (float)blockX + getRandomFloat(rng, 0.4, 0.6), (float)blockY + 0.5f }, b->type);
						Audio::playSound(Audio::breakBlock);
					}

					*b = {};
				}
			}

			// Block placing
			if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
			{
				auto b = gameMap.getBlocSafe(blockX, blockY);
				if (b)
				{
					if (b->type != creativeSelectedBlock)
					{
						b->type = creativeSelectedBlock;
						Audio::playSound(Audio::placeBlock);
					}
				}
			}
		}

		if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE))
		{
			auto b = gameMap.getBlocSafe(blockX, blockY);
			if (b)
			{
				creativeSelectedBlock = b->type;
			}
		}

		if (IsKeyPressed(KEY_TAB))
		{
			insideInventory = !insideInventory;
		}
	}

#pragma region draw world
	//Biome related stuff
	int backgroundType = DrawBackground::forest;
	Audio::playMusic(Audio::musicForest);

	if (player.getPosition().x > gameMap.desertStart && player.getPosition().x < gameMap.desertEnd)
	{
		backgroundType = DrawBackground::desert;
		Audio::playMusic(Audio::musicDesert);
	}
	if (player.getPosition().y > 120)
	{
		backgroundType = DrawBackground::cave;
		Audio::playMusic(Audio::musicCave);
	}

	background.setBackground(backgroundType);

	background.draw(deltaTime, assetManager, camera, { (float)gameMap.w, (float)gameMap.h });

	//draw rest of the world
	BeginMode2D(camera);

	Vector2 topLeftView = GetScreenToWorld2D({ 0, 0 }, camera);
	Vector2 bottomRightView = GetScreenToWorld2D({ (float)GetScreenWidth(), (float)GetScreenHeight() }, camera);

	int startXView = (int)floorf(topLeftView.x - 1);
	int endXView = (int)ceilf(bottomRightView.x + 1);
	int startYView = (int)floorf(topLeftView.y - 1);
	int endYView = (int)ceilf(bottomRightView.y + 1);

	startXView = Clamp(startXView, 0, gameMap.w - 1);
	endXView = Clamp(endXView, 0, gameMap.w - 1);

	startYView = Clamp(startYView, 0, gameMap.h - 1);
	endYView = Clamp(endYView, 0, gameMap.h - 1);

	for (int y = startYView; y < endYView; y++)
		for (int x = startXView; x < endXView; x++)
		{
			auto& b = gameMap.getBlocUnsafe(x, y);

			if (b.type != Block::air)
			{
				DrawTexturePro(
					assetManager.textures,
					getTextureAtlas(b.type, 0, 32, 32), //source
					{ (float)x, (float)y, 1, 1 }, //dest
					{ 0, 0 },	//origin
					0.0f, //rotation
					WHITE //tint
				);
			}
		}



	if (!insideInventoryMenu)
	{
		//Bloc preview
		auto b = gameMap.getBlocSafe(blockX, blockY);
		if (b && b->type == Block::air) {
			DrawTexturePro(
				assetManager.textures,
				getTextureAtlas(creativeSelectedBlock, 0, 32, 32), //source
				{ (float)blockX, (float)blockY, 1, 1 }, //dest
				{ 0, 0 },	//origin
				0.0f, //rotation
				{ 255,255,255,127 } //tint
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
	}

	//Structure selection box
	if (showImgui)
	{
		Rectangle rect;
		rect.x = selectionStart.x;
		rect.y = selectionStart.y;
		rect.width = selectionEnd.x - selectionStart.x;
		rect.height = selectionEnd.y - selectionStart.y;

		rect.width++;
		rect.height++;

		DrawRectangleLinesEx(rect, 0.1, { 20, 101, 250, 145 });
	}

	//slime
	for (auto& e : entities.entities)
	{
		e.second->render(assetManager);
	}

	//Player
	player.render(assetManager);

	DrawRectangleLinesEx(player.physics.transform.getAABB(), 0.1, { 20, 101, 250, 120 });

	EndMode2D();

#pragma region ui
	{
		float w = GetScreenWidth();
		float h = GetScreenHeight();

		Rectangle heartRectangle;

		heartRectangle.height = h * 0.05f;
		heartRectangle.width = heartRectangle.height * 5;

		heartRectangle = placeRectangleTopRightCorner(heartRectangle, w);

		//DrawRectangle(heartRectangle.x, heartRectangle.y, heartRectangle.width, heartRectangle.height, { 230,41,55,155 });

		for (int i = 0; i < 5; i++)
		{
			Rectangle oneHeartRectangle = heartRectangle;
			oneHeartRectangle.width = heartRectangle.height;
			oneHeartRectangle.x += oneHeartRectangle.width * i;

			DrawTexturePro(
				assetManager.hearts,
				getTextureAtlas(0, 0, assetManager.hearts.width / 3, assetManager.hearts.height), //source
				oneHeartRectangle,
				{ 0, 0 },	//origin
				0.0f, //rotation
				WHITE//tint
			);
		}

		if (insideInventory)
		{
			Rectangle inventoryRectangle = getInventoryRectangle(w, h);

			DrawRectangle(inventoryRectangle.x, inventoryRectangle.y,
				inventoryRectangle.width,
				inventoryRectangle.height,
				{ 100, 100, 100, 100 });

			inventoryRectangle = shrinkRectanglePercentage(inventoryRectangle, 0.01f, 0.01f);

			Rectangle oneCellRectangle;
			oneCellRectangle.height = inventoryRectangle.height / 3;
			oneCellRectangle.width = oneCellRectangle.height;
			oneCellRectangle.x = inventoryRectangle.x;
			oneCellRectangle.y = inventoryRectangle.y;

			for (int i = 0; i < 9; i++)
				for (int j = 0; j < 3; j++)
				{
					Rectangle r = oneCellRectangle;
					r.x += oneCellRectangle.width * i;
					r.y += oneCellRectangle.height * j;

					r = shrinkRectanglePercentage(r, 0.1f, 0.1f);

					if (CheckCollisionPointRec(GetMousePosition(), r))
					{
						DrawTexturePro(
							assetManager.frame,
							{ 0, 0, (float)assetManager.frame.width, (float)assetManager.frame.height }, //source
							r, //dest
							{ 0, 0 },	//origin
							0.0f, //rotation
							{ 220, 250, 220, 250 }//tint
						);
					}
					else
					{
						DrawTexturePro(
							assetManager.frame,
							{ 0, 0, (float)assetManager.frame.width, (float)assetManager.frame.height }, //source
							r, //dest
							{ 0, 0 },	//origin
							0.0f, //rotation
							{ 180, 180, 200, 240 }//tint
						);
					}
				}
		}
	}
#pragma endregion

	if (showImgui)
	{
		ImGui::Begin("GameControll");

		ImGui::SliderFloat("Camera zoom", &camera.zoom, 1, 150);
		ImGui::SliderFloat("Camera speed", &CAMERA_SPEED, 5, 200);
		ImGui::Checkbox("Creative mode", &creative);

		if (ImGui::Button("Spawn Slime"))
		{
			spawnSlime({ 18, 60 });
		}
		ImGui::SameLine();
		if (ImGui::Button("Hurt a slime"))
		{
			for (auto& e : entities.entities)
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
			copyStructure.copyFromMap(gameMap, selectionStart, selectionEnd);
		}

		ImGui::InputText("File name", saveName, sizeof(saveName));

		if (ImGui::Button("Save"))
		{
			std::string path = RESOURCES_PATH "structures/";
			path += saveName;
			path += ".bin";

			saveBlockDataToFile(copyStructure.mapData, copyStructure.w, copyStructure.h, path.c_str());
		}
		ImGui::SameLine();

		if (ImGui::Button("Load"))
		{
			std::string path = RESOURCES_PATH "structures/";
			path += saveName;
			path += ".bin";

			loadBlockDataFromFile(copyStructure.mapData, copyStructure.w, copyStructure.h, path.c_str());
		}

		ImGui::Separator();

		ImGui::SliderFloat("Master volume", &getSettings().masterVolume, 0, 1);
		ImGui::SliderFloat("Sound volume", &getSettings().soundsVolume, 0, 1);
		ImGui::SliderFloat("Music volume", &getSettings().musicVolume, 0, 1);

		if (ImGui::Button("Save settings"))
		{
			saveSettings();
		}
		if (ImGui::Button("Load settings"))
		{
			loadSettings();
		}


		if (ImGui::Button("Save World"))
		{
			saveWorld(gameMap, entities, player);
		}
		ImGui::SameLine();
		if (ImGui::Button("Load world"))
		{
			if (!loadWorld(gameMap, entities, player))
			{
				return false;
			}
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
				creativeSelectedBlock = i;
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
