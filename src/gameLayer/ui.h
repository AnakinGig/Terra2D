#pragma once

#include <raylib.h>

Rectangle placeRectangleTopRightCorner(Rectangle r, float w);

Rectangle placeRectangleTopLeftCorner(Rectangle r, float w);

Rectangle placeRectangleBottomRightCorner(Rectangle r, float w, float h);

Rectangle placeRectangleBottomLeftCorner(Rectangle r, float w, float h);

Rectangle placeRectangleCenter(Rectangle r, float w, float h);

Rectangle placeRectangleCenterTop(Rectangle r, float w);

Rectangle placeRectangleCenterBottom(Rectangle r, float w, float h);

Rectangle placeRectangleCenterLeft(Rectangle r, float h);

Rectangle placeRectangleCenterRight(Rectangle r, float w, float h);

Rectangle enlargeRectanglePixels(Rectangle r, float pixelsX, float pixelsY);

Rectangle enlargeRectanglePercentage(Rectangle r, float percentageX, float percentageY);

Rectangle shrinkRectanglePixels(Rectangle r, float pixelsX, float pixelsY);

Rectangle shrinkRectanglePercentage(Rectangle r, float percentageX, float percentageY);