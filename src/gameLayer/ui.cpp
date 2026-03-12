#include "ui.h"

Rectangle placeRectangleTopRightCorner(Rectangle r, float w)
{
	r.x = w - r.width;
	r.y = 0;

	return r;
}

Rectangle placeRectangleTopLeftCorner(Rectangle r, float w)
{
	r.x = 0;
	r.y = 0;
	return r;
}

Rectangle placeRectangleBottomRightCorner(Rectangle r, float w, float h)
{
	r.x = w - r.width;
	r.y = h - r.height;
	return r;
}

Rectangle placeRectangleBottomLeftCorner(Rectangle r, float w, float h)
{
	r.x = 0;
	r.y = h - r.height;
	return r;
}

Rectangle placeRectangleCenter(Rectangle r, float w, float h)
{
	r.x = (w * 0.5f) - (r.width * 0.5f);
	r.y = (h * 0.5f) - (r.height * 0.5f);
	return r;
}

Rectangle placeRectangleCenterTop(Rectangle r, float w)
{
	r.x = (w * 0.5f) - (r.width * 0.5f);
	r.y = 0;
	return r;
}

Rectangle placeRectangleCenterBottom(Rectangle r, float w, float h)
{
	r.x = (w * 0.5f) - (r.width * 0.5f);
	r.y = h - r.height;
	return r;
}

Rectangle placeRectangleCenterLeft(Rectangle r, float h)
{
	r.x = 0;
	r.y = (h * 0.5f) - (r.height * 0.5f);
	return r;
}

Rectangle placeRectangleCenterRight(Rectangle r, float w, float h)
{
	r.x = w - r.width;
	r.y = (h * 0.5f) - (r.height * 0.5f);
	return r;
}

Rectangle enlargeRectanglePixels(Rectangle r, float pixelsX, float pixelsY)
{
	r.width += pixelsX;
	r.height += pixelsY;

	r.x -= pixelsX * 0.5f;
	r.y -= pixelsY * 0.5f;

	return r;
}

Rectangle enlargeRectanglePercentage(Rectangle r, float percentageX, float percentageY)
{
	float enlargeX = r.width * percentageX;
	float enlargeY = r.height * percentageY;

	r.width += enlargeX;
	r.height += enlargeY;

	r.x -= enlargeX * 0.5f;
	r.y -= enlargeY * 0.5f;

	return r;
}

Rectangle shrinkRectanglePixels(Rectangle r, float pixelsX, float pixelsY)
{
	r.width -= pixelsX;
	r.height -= pixelsY;

	r.x += pixelsX * 0.5f;
	r.y += pixelsY * 0.5f;

	return r;
}

Rectangle shrinkRectanglePercentage(Rectangle r, float percentageX, float percentageY)
{
	float shrinkX = r.width * percentageX;
	float shrinkY = r.height * percentageY;

	r.width -= shrinkX;
	r.height -= shrinkY;

	r.x += shrinkX * 0.5f;
	r.y += shrinkY * 0.5f;

	return r;
}

void UIEngine::updateAndRender()
{
	float w = GetScreenWidth();
	float h = GetScreenHeight();

	Rectangle oneButtonRectangle;

	oneButtonRectangle.width = w * 0.8f;
	oneButtonRectangle.height = h / (widgets.size() + 1);

	oneButtonRectangle.height = std::min(oneButtonRectangle.height, oneButtonRectangle.width / 8.0f);

	oneButtonRectangle = placeRectangleCenterTop(oneButtonRectangle, w);
	oneButtonRectangle.y += oneButtonRectangle.height * 0.5f;

	int fontSize = (int)(oneButtonRectangle.height * 0.5f);

	for (auto& w : widgets)
	{
		Rectangle smallerRect = shrinkRectanglePercentage(oneButtonRectangle, 0.1f, 0.1f);

		//DrawRectangle(smallerRect.x, smallerRect.y, smallerRect.width, smallerRect.height,
		//	{ 90, 90, 110, 205 });

		auto drawText = [&](Rectangle smallerRect, float yOffset = 0)
		{
			int textWidth = MeasureText(w.text.c_str(), fontSize);
			int textHeight = fontSize;

			float textX = smallerRect.x + (smallerRect.width - textWidth) * 0.5f;
			float textY = smallerRect.y + (smallerRect.height - textHeight) * 0.5f;

			Color shadowColor = { 0, 0, 0, 200 };
			
			DrawText(w.text.c_str(), textX - fontSize * 0.08f, textY + fontSize * 0.08f + yOffset, fontSize, shadowColor);
			DrawText(w.text.c_str(), textX, textY + yOffset, fontSize, WHITE);
		};

		w.isBeingClicked = false;
		w.isHovered = false;
		w.isReleased = false;

		if (CheckCollisionPointRec(GetMousePosition(), smallerRect))
		{
			w.isHovered = true;

			if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
			{
				w.isBeingClicked = true;
			}
			if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
			{
				w.isReleased = true;
			}
		}

		switch (w.type)
		{
			case button:
			{
				const float clickOffset = 0.05f;
				Color clickColor = { 120, 120, 135, 205 };
				Color defaultColor = { 90, 90, 100, 205 };

				if (w.isBeingClicked)
				{
					DrawRectangle(smallerRect.x, smallerRect.y + smallerRect.height * clickOffset, 
						smallerRect.width, smallerRect.height, clickColor);
				}
				else
				{
					if (w.isHovered)
					{
						DrawRectangle(smallerRect.x, smallerRect.y, smallerRect.width, smallerRect.height,
							clickColor);
					}
					else
					{
						DrawRectangle(smallerRect.x, smallerRect.y, smallerRect.width, smallerRect.height,
							defaultColor);
					}
				}
				if (w.isBeingClicked)
				{
					drawText(smallerRect, smallerRect.height * clickOffset);
				}
				else
				{
					drawText(smallerRect);
				}

				break;
			}

			case title:
			{
				drawText(smallerRect);

				break;
			}
		}

		oneButtonRectangle.y += oneButtonRectangle.height;
	}

	bool disableInputThisFrame = false;

	if (widgets.size() != lastFrameWidgets.size())
	{
		disableInputThisFrame = true;
	}
	else
	{
		for (int i = 0; i < widgets.size(); i++)
		{
			if (widgets[i].type != lastFrameWidgets[i].type)
			{
				disableInputThisFrame = true;
				break;
			}
		}
	}

	lastFrameWidgets = widgets;

	if (disableInputThisFrame)
	{
		for (auto& w : lastFrameWidgets)
		{
			w.isHovered = false;
			w.isReleased = false;
			w.isBeingClicked = false;
		}
	}

	widgets.clear();
}
