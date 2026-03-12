#pragma once

#include <raylib.h>
#include <string>
#include <vector>

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

struct UIEngine
{
	enum Type
	{
		none,
		title,
		button,
	};

	struct Widget
	{
		std::string text = "";
		int type = 0;

		bool isHovered = false;
		bool isBeingClicked = false;
		bool isReleased = false;
	};

	std::vector<Widget> widgets;

	std::vector<Widget> lastFrameWidgets;

	bool addButton(std::string text)
	{
		Widget w;
		w.type = button;
		w.text = text;

		widgets.push_back(w);

		if (lastFrameWidgets.size() < widgets.size())
		{
			return false;
		}

		if (lastFrameWidgets[widgets.size() - 1].type != button)
		{
			return false;
		}

		return lastFrameWidgets[widgets.size() - 1].isReleased;
	}

	void addTitle(std::string text)
	{
		Widget w;
		w.type = title;
		w.text = text;

		widgets.push_back(w);
	}

	void updateAndRender();
};