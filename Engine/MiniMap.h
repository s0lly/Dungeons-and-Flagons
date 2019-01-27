/******************************************************************************************
*	Game code and amendments by s0lly													   *
*	https://www.youtube.com/channel/UCOyCXEB8NTx3Xjm8UptwsIg							   *
*	https://s0lly.itch.io/																   *
*	https://www.instagram.com/s0lly.gaming/												   *
******************************************************************************************/

#pragma once

#include "Vec2.h"
#include "Vei2.h"
#include "World.h"
#include "WorldObject.h"
#include "Graphics.h"
#include <vector>

class MiniMap
{
public:
	MiniMap(World& world)
	{
		mapItems = (int)(world.GetWorldDims().x * world.GetWorldDims().y);
		locRevealed = std::vector<bool> (mapItems, false);
		mapWidth = world.GetWorldWidth();
		mapHeight = world.GetWorldWidth();
	}

	void Init()
	{
		locRevealed = std::vector<bool>(mapItems, false);
	}

	void Init(std::vector<bool> in_locRevealed)
	{
		for (int i = 0; i < mapItems; i++)
		{
			locRevealed[i] = in_locRevealed[i];
		}
		
	}

	void Update(Knight& knight, World& world)
	{
		currentLoc = knight.GetLoc();

		for (int j = 0; j < revealedSize; j++)
		{
			for (int i = 0; i < revealedSize; i++)
			{
				int iLoc = (int)currentLoc.x + i - revealedSize / 2;
				int jLoc = (int)currentLoc.y + j - revealedSize / 2;
				if (iLoc >= 0 && iLoc < mapWidth && jLoc >= 0 && jLoc < mapHeight)
				{
					SetLocRevealed(iLoc, jLoc);
				}
				
			}
		}

	}

	void Draw(Graphics &gfx, World& world)
	{
		for (int j = 0; j < displaySize * miniTileLength; j++)
		{
			for (int i = 0; i < displaySize * miniTileLength; i++)
			{
				int iLoc = gfx.ScreenWidth / 2 - (displaySize / 2) * miniTileLength + i;
				int jLoc = gfx.ScreenHeight / 2 - (displaySize / 2) * miniTileLength + j;

				if (iLoc >= 0 && iLoc < gfx.ScreenWidth && jLoc >= 0 && jLoc < gfx.ScreenHeight)
				{
					Color c = Colors::White;

					if (GetRevealedLoc((i / miniTileLength - (displaySize / 2)) + (int)currentLoc.x, (j / miniTileLength - (displaySize / 2)) + (int)currentLoc.y))
					{
						if (world.GetTileType({ (float)(i / miniTileLength - (displaySize / 2)) + currentLoc.x, (float)(j / miniTileLength - (displaySize / 2)) + currentLoc.y }) == TILETYPE::WALL)
						{
							gfx.PutPixel(iLoc, jLoc, Colors::Cyan);
						}
					}
					
				}

			}
		}
	}

	std::vector<bool>& GetRevealedLocs()
	{
		return locRevealed;
	}

	bool GetRevealedLoc(int i, int j)
	{
		if (i >= 0 && i < mapWidth && j >= 0 && j < mapHeight)
		{
			return locRevealed[j * mapWidth + i];
		}
		return false;
	}

	void SetLocRevealed(int i, int j)
	{
		locRevealed[j * mapWidth + i] = true;
	}


private:

	Vec2 currentLoc = { 0.0f, 0.0f };

	std::vector<bool> locRevealed;

	int mapWidth = 0;
	int mapHeight = 0;

	int mapItems = 0;

	int miniTileLength = 2;

	int revealedSize = 10;

	int displaySize = 200;

};