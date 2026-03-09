#include "saveMap.h"
#include <asserts.h>

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