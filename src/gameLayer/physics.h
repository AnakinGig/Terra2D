#pragma once
#include <raylib.h>
#include <raymath.h>
#include <cmath>
#include <nlohmann/json.hpp>

using Json = nlohmann::json;

// Vector2 operator overloads
inline Vector2 operator+(const Vector2& a, const Vector2& b)
{
	return { a.x + b.x, a.y + b.y };
}

inline Vector2 operator-(const Vector2& a, const Vector2& b)
{
	return { a.x - b.x, a.y - b.y };
}

inline Vector2 operator*(const Vector2& a, float scalar)
{
	return { a.x * scalar, a.y * scalar };
}

inline Vector2 operator/(const Vector2& a, float scalar)
{
	return { a.x / scalar, a.y / scalar };
}


inline Vector2& operator*=(Vector2& a, float scalar)
{
	a.x *= scalar;
	a.y *= scalar;
	return a;
}

inline Vector2& operator/=(Vector2& a, float scalar)
{
	a.x /= scalar;
	a.y /= scalar;
	return a;
}

inline Vector2& operator+=(Vector2& a, float scalar)
{
	a.x += scalar;
	a.y += scalar;
	return a;
}

inline Vector2& operator-=(Vector2& a, float scalar)
{
	a.x -= scalar;
	a.y -= scalar;
	return a;
}


inline bool operator==(const Vector2& a, const Vector2& b)
{
	return a.x == b.x && a.y == b.y;
}

inline bool operator!=(const Vector2& a, const Vector2& b)
{
	return !(a == b);
}

inline Vector2& operator+=(Vector2& a, const Vector2& b)
{
	a.x += b.x;
	a.y += b.y;
	return a;
}

inline Vector2& operator-=(Vector2& a, const Vector2& b)
{
	a.x -= b.x;
	a.y -= b.y;
	return a;
}

inline Vector2& operator*=(Vector2& a, const Vector2& b)
{
	a.x *= b.x;
	a.y *= b.y;
	return a;
}

inline Vector2& operator/=(Vector2& a, const Vector2& b)
{
	a.x /= b.x;
	a.y /= b.y;
	return a;
}

struct Transform2D
{
	Vector2 pos = {}; // Center
	float w = 0; // Width
	float h = 0; // Height

	// Helpers to get key points of the rectangle
	Vector2 getCenter()			const { return { pos.x, pos.y }; }
	Vector2 getTop()			const { return { pos.x, pos.y - h * 0.5f }; }
	Vector2 getBottom()			const { return { pos.x, pos.y + h * 0.5f }; }
	Vector2 getLeft()			const { return { pos.x - w * 0.5f, pos.y }; }
	Vector2 getRight()			const { return { pos.x + w * 0.5f, pos.y }; }
	Vector2 getTopLeft()		const { return { pos.x - w * 0.5f, pos.y - h * 0.5f }; }
	Vector2 getTopRight()		const { return { pos.x + w * 0.5f, pos.y - h * 0.5f }; }
	Vector2 getBottomLeft()		const { return { pos.x - w * 0.5f, pos.y + h * 0.5f }; }
	Vector2 getBottomRight()	const { return { pos.x + w * 0.5f, pos.y + h * 0.5f }; }

	Rectangle getAABB()
	{
		return { pos.x - w * 0.5f, pos.y - h * 0.5f, w, h };
	}

	bool intersectPoint(Vector2 point, float delta = 0)
	{
		Rectangle aabb = getAABB();
		aabb.x -= delta;
		aabb.y -= delta;
		aabb.width += delta * 2;
		aabb.height += delta * 2;

		return CheckCollisionPointRec(point, aabb);
	}

	bool intersectTransform(Transform2D other, float delta = 0)
	{
		Rectangle a = getAABB();
		Rectangle b = other.getAABB();

		a.x -= delta;
		a.y -= delta;
		a.width += delta * 2;
		a.height += delta * 2;

		b.x -= delta;
		b.y -= delta;
		b.width += delta * 2;
		b.height += delta * 2;

		return CheckCollisionRecs(a, b);
	}
};

struct GameMap;

struct PhysicalEntity
{
	Transform2D transform;
	Vector2 lastPosition = {};

	Vector2 velocity = {};
	Vector2 acceleration = {};

	bool upTouch = 0;
	bool downTouch = 0;
	bool leftTouch = 0;
	bool rightTouch = 0;

	Json formatToJson()
	{
		Json j;

		j["posX"] = transform.pos.x;
		j["posY"] = transform.pos.y;
		j["velX"] = velocity.x;
		j["velY"] = velocity.y;

		return j;
	}

	bool loadFromJson(Json j)
	{
		*this = {};

		if (!j.contains("posX") || !j["posX"].is_number())
			return false;
		transform.pos.x = j["posX"];

		if (!j.contains("posY") || !j["posY"].is_number())
			return false;
		transform.pos.y = j["posY"];

		if (j.contains("velX"))
		{
			if (j["velX"].is_number())
			{
				velocity.x = j["velX"];

			}
		}

		if (j.contains("velY"))
		{
			if (j["velY"].is_number())
			{
				velocity.y = j["velY"];
			}
		}

		lastPosition = transform.pos;

		return true;
	}

	void teleport(Vector2 pos)
	{
		transform.pos = pos;
		lastPosition = pos;
	}

	void updateForces(float deltaTime)
	{
		velocity += acceleration * deltaTime;
		transform.pos += velocity * deltaTime;

		Vector2 dragVector = Vector2{ velocity.x * std::abs(velocity.x), velocity.y * std::abs(velocity.y) };
		float drag = 0.01f;

		if (Vector2Length(dragVector) * drag * deltaTime > Vector2Length(velocity))
		{
			velocity = {};
		}
		else
		{
			velocity -= dragVector * drag * deltaTime;
		}

		if (Vector2Length(velocity) < 0.01f)
		{
			velocity = {};
		}

		acceleration = {};
	}

	// Called at the end of a frame
	void updateFinal()
	{
		lastPosition = { transform.pos.x, transform.pos.y };
	}

	void applyGravity()
	{
		acceleration += {0, 20.0};
	}

	void jump(float force)
	{
		if (downTouch)
		{
			velocity.y = -force;
		}
	}

	Vector2& getPosition()
	{
		return transform.pos;
	}

	void resolveConstrains(GameMap& mapData);

	void checkCollisionOnce(Vector2& pos, GameMap &mapData);

	Vector2 performCollisionOnOneAxis(GameMap& mapData, Vector2 pos, Vector2 delta);
};