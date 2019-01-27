/******************************************************************************************
*	Game code and amendments by s0lly													   *
*	https://www.youtube.com/channel/UCOyCXEB8NTx3Xjm8UptwsIg							   *
*	https://s0lly.itch.io/																   *
*	https://www.instagram.com/s0lly.gaming/												   *
******************************************************************************************/

#pragma once

class Vec2
{
public:

	Vec2(float a, float b)
	{
		x = a;
		y = b;
	}

	Vec2 operator+(Vec2 rhs)
	{
		return { x + rhs.x, y + rhs.y };
	}

	Vec2 operator-(Vec2 rhs)
	{
		return { x - rhs.x, y - rhs.y };
	}

	float GetLengthSq()
	{
		return (x * x + y * y);
	}

	bool operator==(const Vec2& rhs) const
	{
		return (x == rhs.x && y == rhs.y);
	}

	float x = 0.0f;
	float y = 0.0f;

private:


};