#include <assetManager.h>
#include <audio.h>
#include <settings.h>
#include <saveMap.h>
#include <gameplay.h>
#include <ui.h>

/*
TODO : 

Implement proper saving : 
- Separate save map and entities
- Save map every x blocs update
- Save entities every x seconds
- Save everything every x minutes

Implement audio switch when chenging biomes (like we did for the background)

Implement fully working inventory with item rendering and selection

Implement more enemies

Implement chests and loot

Crafting system

Creative mode

World selector

Animate main menu background (move camera, and sometimes switch bioms)

Bug fix :

*/

AssetManager assetManager;
Gameplay gameplay;
UIEngine mainMenu;
Background backgroundMainMenu;
bool gameplayRunning = false;

// ========== INIT ==========

bool initGame()
{
	Audio::init();
	assetManager.loadAll();
	loadSettings();

	gameplay.init();

	return true;
}

// ========== UPDATE ==========
bool updateGame()
{
	Audio::update();
	updateSettings();

	ClearBackground({ 0, 0, 0, 255 });

	if (!gameplayRunning)
	{
		Camera2D c = {};
		c.offset = { GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };
		c.target = { 200, 500 };
		c.zoom = 20;
		backgroundMainMenu.draw(GetFrameTime(), assetManager, c, { 1000, 1000 });

		mainMenu.addTitle("Main menu");

		if (mainMenu.addButton("Start game"))
		{
			gameplayRunning = true;
			gameplay = {};
			gameplay.init();
		}

		mainMenu.addButton("Settings");

		if (mainMenu.addButton("Exit"))
		{
			return false;
		}

		mainMenu.updateAndRender();

		return true;
	}
	else
	{
		return gameplay.update(assetManager);
	}
}

// ========== CLOSE ==========
void closeGame()
{
	saveWorld(gameplay.gameMap, gameplay.entities, gameplay.player);
}