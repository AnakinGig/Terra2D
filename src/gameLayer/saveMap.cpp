#include "saveMap.h"
#include <asserts.h>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <gameMap.h>
#include <entityIdHolder.h>
#include <player.h>
#include <entities/slime.h>
#include <entities/droppedItem.h>

struct BlockSaveRepresentation1
{
	std::uint16_t type = 0;

	Block toBlock()
	{
		Block b;
		b.type = type;
		return b;
	}
};

const int VERSION = 1;

BlockSaveRepresentation1 toBlockSaveRepresentation(Block b)
{
	BlockSaveRepresentation1 rez;
	rez.type = b.type;
	return rez;
}

bool saveBlockDataToFile(const std::vector<Block> &blocks, int w, int h, const char* fileName)
{
	std::ofstream f(fileName, std::ios::binary);

	if (!f.is_open()) { return false; }

	if (blocks.size() != w * h) { return false; }

	f.write((const char*)&VERSION, sizeof(VERSION));
	f.write((const char*)&w, sizeof(w));
	f.write((const char*)&h, sizeof(h));

	for (int i = 0; i < blocks.size(); i++)
	{
		auto b = toBlockSaveRepresentation(blocks[i]);
		f.write((const char*)&b, sizeof(b));
	}

	f.close();

	return true;
}

bool loadBlockDataFromFile(std::vector<Block>& blocks, int& w, int& h, const char* fileName)
{
	blocks.clear();
	w = 0;
	h = 0;

	std::ifstream f(fileName, std::ios::binary);

	if (!f.is_open()) { return false; }

	int readVerion = 0;

	f.read((char*)&readVerion, sizeof(readVerion));
	f.read((char*)&w, sizeof(w));
	f.read((char*)&h, sizeof(h));

	if (!f || w <= 0 || h <= 0) 
	{ 
		f.close();
		return false; 
	}

	if (w > 10000 || h > 10000) //Prevent corrupted data
	{
		f.close();
		return false;
	}

	switch (readVerion)
	{
		case 1:
		{
			size_t blockCount = w * h;
			blocks.resize(blockCount);

			for (int i = 0; i < blockCount; i++)
			{
				BlockSaveRepresentation1 read;
				f.read((char*)&read, sizeof(read));

				if (!f)
				{
					blocks.clear();
					w = 0;
					h = 0;
					f.close();
					return false;
				}

				blocks[i] = read.toBlock();
			}

			break;
		}

		default:
		{
			// Unsupported version
			w = 0;
			h = 0;
			f.close();
			return false;
		}
	}

	for (int i = 0; i < blocks.size(); i++)
	{
		blocks[i].sanitize();
	}

	f.close();
	return true;
}

using Json = nlohmann::json;

void saveWorld(GameMap& gameMap, EntityHolder& entities, Player& player)
{
	std::error_code errorCode;
	std::filesystem::create_directories(RESOURCES_PATH "../saves/", errorCode);

	//map
	saveBlockDataToFile(gameMap.mapData, gameMap.w, gameMap.h, RESOURCES_PATH "../saves/world.bin.tmp");

	//id holder
	{
		std::ofstream f(RESOURCES_PATH "../saves/idHolder.txt.tmp", std::ios::binary);
		f << entities.idHolder.idCounter;
		f.close();
	}

	//player
	{
		Json j = player.formatToJson();

		std::ofstream f(RESOURCES_PATH "../saves/player.txt.tmp");
		f << j.dump(2);
		f.close();
	}

	//entities
	{
		Json j;

		for (auto& e : entities.entities)
		{
			j[std::to_string(e.first)] = e.second->formatToJson();
		}

		std::ofstream f(RESOURCES_PATH "../saves/entities.txt.tmp");
		f << j.dump(2);
		f.close();
	}

	std::filesystem::rename(RESOURCES_PATH "../saves/world.bin.tmp", RESOURCES_PATH "../saves/world.bin", errorCode);
	std::filesystem::rename(RESOURCES_PATH "../saves/idHolder.txt.tmp", RESOURCES_PATH "../saves/idHolder.txt", errorCode);
	std::filesystem::rename(RESOURCES_PATH "../saves/player.txt.tmp", RESOURCES_PATH "../saves/player.txt", errorCode);
	std::filesystem::rename(RESOURCES_PATH "../saves/entities.txt.tmp", RESOURCES_PATH "../saves/entities.txt", errorCode);
}

bool loadWorld(GameMap& gameMap, EntityHolder& entities, Player& player)
{
	gameMap = {};
	entities.entities.clear();
	player = {};
	entities.idHolder = {};

	if (!loadBlockDataFromFile(gameMap.mapData, gameMap.w, gameMap.h, RESOURCES_PATH "../saves/world.bin"))
	{
		return false;
	}

	//id holder
	{
		std::ifstream f(RESOURCES_PATH "../saves/idHolder.txt");

		if (!f.is_open()) { return false; }
		f >> entities.idHolder.idCounter;
		f.close();
	}

	//player
	{
		std::ifstream f(RESOURCES_PATH "../saves/player.txt");
		if (!f.is_open()) { return false; }

		Json j;
		j = Json::parse(f, nullptr, /*allow_exceptions=*/false);

		if (!player.loadFromJson(j))
		{
			return false;
		}
	}

	//entities
	{
		std::ifstream f(RESOURCES_PATH "../saves/entities.txt");
		if (!f.is_open()) { return false; }

		Json j;
		j = Json::parse(f, nullptr, /*allow_exceptions=*/false);

		for (auto it = j.begin(); it != j.end(); ++it)
		{
			const std::string& keyStr = it.key();
			bool isNumeric = !keyStr.empty() && std::all_of(keyStr.begin(), keyStr.end(), ::isdigit);

			if (!isNumeric)
				continue; // skip non-numeric keys

			std::uint64_t id = 0;

			for (auto c : keyStr)
			{
				id *= 10;
				id += c - '0';
			}

			Json& entityJson = it.value();

			int entityType = 0;

			if (entityJson["entityType"].is_number())
			{
				entityType = entityJson["entityType"];

				switch (entityType)
				{
					case EntityType_Slime:
					{
						Slime slime;
						if (slime.loadFromJson(entityJson))
						{
							entities.entities[id] = std::make_unique<Slime>(slime);
						}
						break;
					}

					case EntityType_DroppedItem:
					{
						DroppedItem droppedItem;
						if (droppedItem.loadFromJson(entityJson))
						{
							entities.entities[id] = std::make_unique<DroppedItem>(droppedItem);
						}
						break;
					}
				}
			}
		}
	}

	return true;
}
