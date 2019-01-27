/******************************************************************************************
*	Game code and amendments by s0lly													   *
*	https://www.youtube.com/channel/UCOyCXEB8NTx3Xjm8UptwsIg							   *
*	https://s0lly.itch.io/																   *
*	https://www.instagram.com/s0lly.gaming/												   *
******************************************************************************************/

#pragma once

#include "Colors.h"
#include <string>
#include "RectI.h"
#include <vector>


enum MOVETYPE
{
	NONE,
	UP,
	DOWN,
	LEFT,
	RIGHT
};

class Surface
{
public:
	Surface(const std::string& filename);
	Surface(int width, int height);
	Surface(const Surface&);
	~Surface();
	Surface& operator=(const Surface&);

	void PutPixel(int x, int y, Color c);
	Color GetPixel(int x, int y) const;
	int GetWidth() const;
	int GetHeight() const;
	RectI GetRect() const;

	void DrawSprite(int x, int y, RectI srcRect, const RectI& clip, const Surface& s, Color chroma = Colors::Magenta);
	void DrawSpriteScaledUpRotated(int x, int y, RectI srcRect, const RectI& clip, const Surface& s, int scale, MOVETYPE direction, Color chroma = Colors::Magenta);

	void AddFOV(Surface& cameraSurface, std::vector<bool>& pixelFOVToggle);

	void Refresh()
	{
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				PutPixel(x, y, Colors::Black);
			}
		}
	}

	void ApplyLighting(std::vector<bool> &pixelFOVToggle, float* appliedLighting)
	{
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				if (pixelFOVToggle[y * width + x])
				{

					Color pixelColor = pPixels[y * width + x];
					float appliedLight = appliedLighting[y * width + x] > 1.0f ? 1.0f : appliedLighting[y * width + x];

					//appliedLight = appliedLight < 0.08f ? 0.0f : appliedLight;

					pPixels[y * width + x] = { (unsigned char)((float)(pPixels[y * width + x].GetR()) * appliedLight), (unsigned char)((float)(pixelColor.GetG()) * appliedLight), (unsigned char)((float)(pixelColor.GetB()) * appliedLight) };
				}
				else
				{
					pPixels[y * width + x] = Colors::Black;
				}
			}
		}
	}

private:
	Color* pPixels = nullptr;
	int width;
	int height;


};