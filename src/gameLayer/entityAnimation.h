#pragma once

struct EntityAnimation
{
	int positionX = 0;
	int positionY = 0;

	float timer = 0;
	bool movingLeft = 0;

	void update(float deltaTime, float frameDuration, int framesCount)
	{
		timer -= deltaTime;

		if (timer <= 0)
		{
			timer += frameDuration;
			positionX++;
		}
		positionX %= framesCount;
	}

	void setAnimation(int animation)
	{
		if (positionY != animation)
		{
			positionX = 0;
			positionY = animation;
		}
	}
};