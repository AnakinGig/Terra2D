#pragma once
#include <raylib.h>
#include <physics.h>

Rectangle getTextureAtlas(int x, int y, int cellSizePixelsX, int cellSizePixelsY);

Rectangle getRectangleForEntity(Transform2D transform, float textureW, float textureH);