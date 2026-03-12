#pragma once
#include <raylib.h>
#include <physics.h>

Rectangle getTextureAtlas(int x, int y, int cellSizePixelsX, int cellSizePixelsY, bool flipX = 0);

Rectangle getRectangleForEntity(Transform2D transform, float textureW, float textureH);

Rectangle flipTextureAtlasX(Rectangle r);

Rectangle shrinkUV(Rectangle in);