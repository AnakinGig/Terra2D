#include "worldGenerator.h"
#include "randomStuff.h"
#include <FastNoiseSIMD.h>
#include <structure.h>
#include <saveMap.h>

// TODO : Refactor the code 

// create the basic world shape with stone
auto createStoneLayer = [&]()
	{

	};

// TODO : create one montain with 
auto createOneMountain = [&]()
	{

	};

// create the dirt / grassblock layer above stone
auto addDirtLayer = [&]()
	{

	};

// create caves
auto addCaves = [&]()
	{

	};

// create desert biome
auto addDesertBiome = [&]()
	{

	};

// populate world with trees
auto addTrees = [&]()
	{

	};

void generateWorld(GameMap& gameMap, int seed)
{
	const int w = 2000;
	const int h = 500;

	gameMap.create(w, h);

	std::ranlux24_base rng(seed++);

	int desertStart = getRandomInt(rng, 10, w - 210);
	int desertEnd = desertStart + 100 + getRandomInt(rng, 100, 200);
	if (desertEnd > w) { desertEnd = w;  }

	gameMap.desertStart = desertStart;
	gameMap.desertEnd = desertEnd;

	Structure treeStructure;
	bool treeLoaded = loadBlockDataFromFile(treeStructure.mapData, treeStructure.w, treeStructure.h, RESOURCES_PATH "structures/tree.bin");

	std::unique_ptr<FastNoiseSIMD> dirtNoiseGenerator(FastNoiseSIMD::NewFastNoiseSIMD());
	std::unique_ptr<FastNoiseSIMD> caveNoiseGenerator(FastNoiseSIMD::NewFastNoiseSIMD());

	dirtNoiseGenerator->SetSeed(seed++);
	caveNoiseGenerator->SetSeed(seed++);

	dirtNoiseGenerator->SetNoiseType(FastNoiseSIMD::NoiseType::SimplexFractal);
	dirtNoiseGenerator->SetFractalOctaves(6);
	dirtNoiseGenerator->SetFractalGain(0.4);
	dirtNoiseGenerator->SetFrequency(0.01);

	caveNoiseGenerator->SetNoiseType(FastNoiseSIMD::NoiseType::SimplexFractal);
	caveNoiseGenerator->SetFractalOctaves(3);
	caveNoiseGenerator->SetFrequency(0.02);

	float* dirtNoise = FastNoiseSIMD::GetEmptySet(w);
	dirtNoiseGenerator->FillNoiseSet(dirtNoise, 0, 0, 0, w, 1, 1);

	// Convert from [-1 1] to [0 1]
	for (int i = 0; i < w; i++)
	{
		dirtNoise[i] = (dirtNoise[i] + 1) / 2;
	}

	float* caveNoise = FastNoiseSIMD::GetEmptySet(w * h);
	caveNoiseGenerator->FillNoiseSet(caveNoise, 0, 0, 0, h, w, 1);

	// Convert from [-1 1] to [0 1]
	for (int i = 0; i < w * h; i++)
	{
		caveNoise[i] = (caveNoise[i] + 1) / 2;
	}

	auto getCaveNoise = [&](int x, int y)
	{
		return caveNoise[x + y * w];
	};

	int dirtOffsetStart = -5;
	int dirtOffsetEnd = 35;

	int keepDirectionTimeStone = getRandomInt(rng, 5, 40);
	int directionStone = getRandomInt(rng, -2, 2);

	int stoneHeight = 90;

	for (int x = 0; x < w; x++)
	{

		bool inDesert = (x >= desertStart && x <= desertEnd);
#pragma region stone layer
		keepDirectionTimeStone--;
		if (keepDirectionTimeStone <= 0)
		{
			keepDirectionTimeStone = getRandomInt(rng, 5, 40);
			directionStone = getRandomInt(rng, -2, 2);
		}

		if (directionStone == -2)
		{
			if (getRandomChance(rng, 0.25))
			{
				stoneHeight --;
			}
			if (getRandomChance(rng, 0.25))
			{
				stoneHeight--;
			}
		}
		else if (directionStone == -1)
		{
			if (getRandomChance(rng, 0.25))
			{
				stoneHeight--;
			}
		}
		else if (directionStone == 2)
		{
			if (getRandomChance(rng, 0.25))
			{
				stoneHeight++;
			}

			if (getRandomChance(rng, 0.25))
			{
				stoneHeight++;
			}
		}
		else if (directionStone == 1)
		{
			if (getRandomChance(rng, 0.25))
			{
				stoneHeight++;
			}
		}

		if (stoneHeight < 60)
		{
			stoneHeight = 60;
		}

		if (stoneHeight > 120)
		{
			stoneHeight = 120;
		}
#pragma	endregion

#pragma region draw layers
		int dirtHeight = dirtOffsetStart + (dirtOffsetEnd - dirtOffsetStart) * dirtNoise[x];
		dirtHeight = stoneHeight - dirtHeight;

		int dirtType = Block::dirt;
		int grassType = Block::grassBlock;
		int stoneType = Block::stone;

		if (inDesert)
		{
			dirtType = Block::sand;
			grassType = Block::sand;
			stoneType = Block::sandStone;
		}

		for (int y = 0; y < h; y++)
		{
			Block b;

			if (y > dirtHeight)
			{
				b.type = dirtType;
			}
			if (y == dirtHeight)
			{
				b.type = grassType;
			}
			if (y >= stoneHeight)
			{
				b.type = stoneType;
			}

			if (inDesert)
			{
				int desertMid = (desertEnd + desertStart) / 2;
				int desertHalfWidth = (desertEnd - desertStart) / 2;
				int distanceFromDesertMid = std::abs(x - desertMid);

				// Give a value from 0 at edge to 1 at center
				float desertDistance = 1 - distanceFromDesertMid / float(desertHalfWidth);

				int desertStoneStart = 10 + stoneHeight;
				int desertStoneDepth = 20 + stoneHeight;

				int triangleStoneY = desertStoneStart + desertDistance * desertStoneDepth;

				if (y > triangleStoneY)
				{
					b.type = Block::stone;
				}
			}

			if (getCaveNoise(x, y) < 0.80 && getCaveNoise(x, y) > 0.60)
			{
				b.type = Block::air;
			}

			gameMap.getBlocUnsafe(x, y) = b;
		}
	}

	FastNoiseSIMD::FreeNoiseSet(dirtNoise);
	FastNoiseSIMD::FreeNoiseSet(caveNoise);
#pragma	endregion

#pragma region perlin worms

	for (int i = 0; i < 20; i++)
	{
		float x = getRandomInt(rng, 10, w - 10);
		float y = getRandomInt(rng, 51, h - 10);

		float dirX = (getRandomFloat(rng, -1, 1));
		float dirY = (getRandomFloat(rng, -1, 1));

		int wormLength = getRandomInt(rng, 200, 700);
		float radius = 2.5;
		
		int changeDirectionTime = getRandomInt(rng, 5, 20);

		for (int j = 0; j < wormLength; j++)
		{
			int intRadius = std::ceil(radius);
			for (int ox = -intRadius; ox <= intRadius; ox++)
			{
				for (int oy = -intRadius; oy <= intRadius; oy++)
				{
					float distSq = ox * ox + oy * oy;
					if (distSq <= radius * radius)
					{
						int digX = x + ox;
						int digY = y + oy;

						auto b = gameMap.getBlocSafe(digX, digY);
						if (b)
						{
							b->type = Block::air;
						}
					}
				}
			}

			changeDirectionTime--;
			if (changeDirectionTime <= 0)
			{
				changeDirectionTime = getRandomInt(rng, 5, 20);

				if (getRandomChance(rng, 0.7))
				{
					float keepFactor = 0.8;

					dirX = dirX * keepFactor + getRandomFloat(rng, -1, 1) * (1.f - keepFactor);
					dirY = dirY * keepFactor + getRandomFloat(rng, -1, 1) * (1.f - keepFactor);
				}
				else
				{
					float keepFactor = 0.2;

					dirX = dirX * keepFactor + getRandomFloat(rng, -1, 1) * (1.f - keepFactor);
					dirY = dirY * keepFactor + getRandomFloat(rng, -1, 1) * (1.f - keepFactor);
				}
			}

			x += dirX * 1.5f;
			y += dirY * 1.5f;

			radius += (getRandomFloat(rng, -0.2, 0.2));
			radius = std::clamp(radius, 2.2f, 8.5f);
		}
	}

#pragma endregion

#pragma region fill tree
	if (treeLoaded)
	{
		for (int x = 0; x < w; x++)
		{
			if (getRandomChance(rng, 0.08))
			{
				for (int y = 0; y < h; y++)
				{
					auto type = gameMap.getBlocUnsafe(x, y).type;
					if (type == Block::air)
					{
						continue;
					}

					if (type == Block::grassBlock)
					{
						//Plant tree
						Vector2 spawnPos{ (float)x, (float)y };

						spawnPos.x -= treeStructure.w / 2;
						spawnPos.y -= treeStructure.h;

						treeStructure.pasteIntoMap(gameMap, spawnPos);

						x += 3; //prevent clipping trees

						break;
					}
					else
					{
						break;
					}
				}
			}
		}
	}

#pragma endregion
}
