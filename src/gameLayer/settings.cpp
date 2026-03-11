#include "settings.h"
#include <fstream>
#include <filesystem>
#include <raymath.h>

Settings settings;

Settings& getSettings()
{
	return settings;
}