#include <assetManager.h>
#include <audio.h>
#include <settings.h>
#include <saveMap.h>
#include <gameplay.h>

/*
TODO : 

Implement proper saving : 
- Separate save map and entities
- Save map every x blocs update
- Save entities every x seconds
- Save everything every x minutes

Implement audio switch when chenging biomes (like we did for the background)

Implement fully working inventory with item rendering and selection

Bug fix :
- Change bloc placement to check for entities or existing blocks

*/

AssetManager assetManager;

Gameplay gameplay;
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
	return gameplay.update(assetManager);
}

// ========== CLOSE ==========
void closeGame()
{
	saveWorld(gameplay.gameMap, gameplay.entities, gameplay.player);
}