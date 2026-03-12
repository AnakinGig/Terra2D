#pragma once
#include <vector>
#include <blocks.h>
#include <fstream>

bool saveBlockDataToFile(const std::vector<Block> &blocks, int w, int h, const char* fileName);

bool loadBlockDataFromFile(std::vector<Block>& blocks, int& w, int& h, const char* fileName);

struct GameMap;
struct Player;
struct EntityHolder;

void saveWorld(GameMap& gameMap, EntityHolder& entities, Player& player);

bool loadWorld(GameMap& gameMap, EntityHolder& entities, Player& player);