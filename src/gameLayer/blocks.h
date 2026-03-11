#pragma once
#include <cstdint>

struct Block
{
	enum
	{
		air = 0,
		dirt,
		grassBlock,
		stone,
		grass,
		sand,
		sandRuby,
		sandStone,
		woodPlank,
		stoneBricks,
		clay,
		woodLog,
		leaves,
		copper,
		iron,
		gold,
		copperBlock,
		ironBlock,
		goldBlock,
		bricks,
		snow,
		ice,
		rubyBlock,
		platform,
		workBench,
		glass,
		furnace,
		painting,
		sappling,
		snowBlueRuby,
		blueRubyBlock,
		door,
		jar,
		table,
		wordrobe,
		bookShelf,
		snowBricks,
		iceTable,
		iceWordrobe,
		iceBookShelf,
		icePlatform,
		sandTable,
		sandWordrobe,
		sandBookShelf,
		sandPlatform,
		woodenChest,
		iceChest,
		sandChest,
		boneChest,
		boneBricks,
		boneBench,
		boneWordrobe,
		boneBookShelf,
		bonePlatform,

		BLOCKS_COUNT
	};

	std::uint16_t type = 0;

	void sanitize()
	{
		if (type >= BLOCKS_COUNT) 
		{ 
			type = 0;
		}
	}

	bool isCollidable()
	{
		switch(type)
		{
			case air:
			case leaves:
			case platform:
			case workBench:
			case sappling:
			case door:
			case jar:
			case table:
			case wordrobe:
			case bookShelf:
			case icePlatform:
			case iceTable:
			case iceWordrobe:
			case iceBookShelf:
			case sandPlatform:
			case sandTable:
			case sandWordrobe:
			case sandBookShelf:
			case bonePlatform:
			case boneBench:
			case boneWordrobe:
			case boneBookShelf:
			case woodLog:
				return false;
			default:
				return true;
		}
		return false;
	}
};