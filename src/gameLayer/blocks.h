#pragma once
#include <cstdint>

struct Block
{
	enum
	{
		air = 0,
		dirt,

		BLOCK_COUNT,

	};

	std::uint16_t type = 0;
};