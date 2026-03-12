#include "background.h"
#include <assetManager.h>
#include <raymath.h>
#include <algorithm>

void Background::draw(float deltaTime, AssetManager& assetManager, Camera2D camera, Vector2 mapSize)
{
	auto drawOneBackground = [&](int type, float parallax, float opacity)
	{
		Texture bg = assetManager.forestBG;

		switch (type)
		{
			case forest: bg = assetManager.forestBG; break;
			case desert: bg = assetManager.desertBg; break;
			case snow: bg = assetManager.snowBG; break;
			case cave: bg = assetManager.caveBG; break;
		}

		int screenW = GetScreenWidth();
		int screenH = GetScreenHeight();

		float aspectRatio = (float)bg.width / bg.height;
		float bgScaleScreen = 2.0f;

		float base = std::max(screenW, screenH) * bgScaleScreen;

		float bgW = base;
		float bgH = base;

		if (aspectRatio > 1.0f)
		{
			bgH = bgW / aspectRatio;
		}
		else
		{
			bgW = bgH * aspectRatio;
		}

		//compute camera ranges
		const float halfViewW = (screenW * 0.5f) / camera.zoom;
		const float halfViewH = (screenH * 0.5f) / camera.zoom;

		const float camMinX = halfViewW;
		const float camMaxX = mapSize.x - halfViewW;
		const float camMinY = halfViewH;
		const float camMaxY = mapSize.y - halfViewH;

		Vector2 camPos = camera.target;
		camPos.x = Clamp(camPos.x, camMinX, camMaxX);
		camPos.y = Clamp(camPos.y, camMinY, camMaxY);

		const float camRangX = std::max(0.0f, camMaxX - camMinX);
		const float camRangY = std::max(0.0f, camMaxY - camMinY);

		const float normX = camRangX > 0.0f ? (camPos.x - camMinX) / camRangX : 0.0f;
		const float normY = camRangY > 0.0f ? (camPos.y - camMinY) / camRangY : 0.0f;

		//how far the bg can move inside the screen
		const float maxOffX = bgW - screenW;
		const float maxOffY = bgH - screenH;

		const float offX = -maxOffX * normX * parallax;
		const float offY = -maxOffY * normY * parallax;

		Rectangle src = { 0, 0, (float)bg.width, (float)bg.height };
		Rectangle dest = { offX, offY, bgW, bgH };

		DrawTexturePro(bg, src, dest, { 0, 0 }, 0.0f, { 255, 255, 255, (unsigned char)(255 * opacity) });
	};

	drawOneBackground(currentBackgroundType , 0.3, 1);

	transitionTime -= deltaTime;
	if (transitionTime > 0)
	{
		float opacity = transitionTime;
		if (opacity > 1) { opacity = 1; }
		drawOneBackground(currentTransitionType, 0.3, opacity);
	}
}

void Background::setBackground(int background)
{
	if (background != currentBackgroundType)
	{
		if (transitionTime <= 0)
		{
			transitionTime = 1; //transition time in seconds
			currentTransitionType = currentBackgroundType;
			currentBackgroundType = background;
		}
	}
}