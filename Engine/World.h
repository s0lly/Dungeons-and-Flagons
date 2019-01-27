/******************************************************************************************
*	Game code and amendments by s0lly													   *
*	https://www.youtube.com/channel/UCOyCXEB8NTx3Xjm8UptwsIg							   *
*	https://s0lly.itch.io/																   *
*	https://www.instagram.com/s0lly.gaming/												   *
******************************************************************************************/

#pragma once

#include <vector>
#include "Vec2.h"
#include <string>
#include <fstream>
#include <iostream>


struct EdgeData
{
	int numEdgesTotal = 0;
	int numEdgesMerged = 0;
	int numEdgesCalculated = 0;
};


enum EDGETYPE
{
	HORIZONTAL,
	VERTICAL
};

enum EDGEDIRECTION
{
	EDGELEFT,
	EDGEUP,
	EDGERIGHT,
	EDGEDOWN
};

struct Line
{
	Line(Vec2 vertex0, Vec2 vertex1) // need to allow for m = 0 cases
	{
		//if (vertex0.x - vertex1.x == 0.0f)
		//{
		//	vertex1.x += 0.001f;
		//}
		m = (vertex0.y - vertex1.y) / (vertex0.x - vertex1.x);
		c = vertex0.y - m * vertex0.x;
	}

	float GetX(float y)
	{
		return (y - c) / m;
	}

	float GetY(float x)
	{
		return isVertical ? 0.0f : m * x + c;
	}

	bool isVertical = false;

	float m = 0;
	float c = 0;
};


struct Edge
{
	Edge(Vec2 v0, Vec2 v1, Line l0, Line l1, Vec2 distLoc, EDGETYPE in_edgeType, EDGEDIRECTION in_edgeDirection)
	{
		vertex0 = v0;
		vertex1 = v1;
		line0 = l0;
		line1 = l1;
		edgeType = in_edgeType;

		float distSqrd0 = (vertex0.x - distLoc.x) * (vertex0.x - distLoc.x) + (vertex0.y - distLoc.y) * (vertex0.y - distLoc.y);
		float distSqrd1 = (vertex1.x - distLoc.x) * (vertex1.x - distLoc.x) + (vertex1.y - distLoc.y) * (vertex1.y - distLoc.y);

		smallestDistSqrd = distSqrd0 < distSqrd1 ? distSqrd0 : distSqrd1;

		size = 1;

		edgeDirection = in_edgeDirection;
	}

	bool IsHorizontal()
	{
		return abs(vertex0.y - vertex1.y) < 0.01f ? true : false;
	}

	bool operator< (const Edge &other) const
	{
		return smallestDistSqrd < other.smallestDistSqrd;
	}

	bool IsNeighbor(const Edge& rhs) const
	{
		return edgeType == rhs.edgeType
			&& (vertex0 == rhs.vertex0 || vertex1 == rhs.vertex0 || vertex0 == rhs.vertex1 || vertex1 == rhs.vertex1)
			&& !(vertex0 == rhs.vertex0 && vertex1 == rhs.vertex1);
	}

	void MergeIntoNeighbor(Edge& rhs)
	{
		rhs.smallestDistSqrd = smallestDistSqrd < rhs.smallestDistSqrd ? smallestDistSqrd : rhs.smallestDistSqrd;
		//if (rhs.vertex0 == vertex0)
		//{
		//	rhs.vertex0 == vertex1;
		//	rhs.line0 = line1;
		//}
		//else if (rhs.vertex0 == vertex1)
		//{
		//	rhs.vertex0 == vertex0;
		//	rhs.line0 = line0;
		//}
		if (rhs.vertex1 == vertex0)
		{
			rhs.vertex1 = vertex1;
			rhs.line1 = line1;
		}
		//else if (rhs.vertex1 == vertex1)
		//{
		//	rhs.vertex1 == vertex0;
		//	rhs.line1 = line0;
		//}
		rhs.size++;
	}


	Vec2 vertex0 = { 0,0 };
	Vec2 vertex1 = { 0,0 };
	Line line0 = { { 0.0f, 0.0f },{ 1.0f, 1.0f } };
	Line line1 = { { 0.0f, 0.0f },{ 1.0f, 1.0f } };

	float smallestDistSqrd = 0.0f;

	EDGETYPE edgeType = EDGETYPE::HORIZONTAL;

	int size = 0;

	EDGEDIRECTION edgeDirection;

	bool vertex0Hold = false;
	bool vertex1Hold = false;
};







enum TILETYPE
{
	FLOOR,
	WALL,
	EMPTY
};


class World
{
public:
	World(Surface* in_spriteMap)
	{
		spriteMap = in_spriteMap;

		Init();


	}

	void Init()
	{
		charMap.clear();

		std::ifstream mapFile;
		mapFile.open("map.txt");
		char temp;
		if (mapFile.is_open())
		{
			while (!mapFile.eof())
			{
				mapFile >> temp;
				if (temp != ',' && temp != '\n')
				{
					int mapCharToUse = (int)temp - 48 > 1 ? 0 : (int)temp - 48;
					charMap.push_back(mapCharToUse);
				}
			}
		}

		charMap.pop_back();

		mapFile.close();


		for (int y = 0; y < mapHeight; y++)
		{
			for (int x = 0; x < mapWidth; x++)
			{
				mapSpace.push_back((TILETYPE)charMap[y * mapWidth + x]);
			}
		}



		allEdges.clear();

		float edgeShift = 0.25f;

		//for (int j = (int)(knight.GetLoc().y - (float)((float)(cameraSurface.GetHeight() * (float)2) / (float)world.GetTileHeight())); j < (int)(knight.GetLoc().y - (float)((float)(cameraSurface.GetHeight() * (float)2) / (float)world.GetTileHeight())) + cameraSurface.GetHeight() * 4 / world.GetTileHeight() + 1; j++)
		//{
		//	for (int i = (int)(knight.GetLoc().x - (float)((float)(cameraSurface.GetWidth() * (float)2) / (float)world.GetTileWidth())); i < (int)(knight.GetLoc().x - (float)((float)(cameraSurface.GetWidth() * (float)2) / (float)world.GetTileWidth())) + cameraSurface.GetWidth() * 4 / world.GetTileWidth() + 1; i++)
		for (int j = 0; j < mapHeight; j++)
		{
			for (int i = 0; i < mapWidth; i++)
			{
				if (i >= 0 && i < GetWorldDims().x && j >= 0 && j < GetWorldDims().y)
				{
					Vec2 vertex0 = { 0.0f, 0.0f };
					Vec2 vertex1 = { 0.0f, 0.0f };

					if (GetTileType({ (float)i,(float)j }) == TILETYPE::WALL)
					{
						// left edge
						if ((i != 0) && !(GetTileType({ (float)(i - 1),(float)j }) == TILETYPE::WALL))
						{
							vertex0 = Vec2((float)i, (float)j);
							vertex1 = Vec2((float)i, (float)(j + 1));
							allEdges.push_back(Edge(vertex0, vertex1, Line(vertex0, Vec2(-1.0f, -1.0f)), Line(vertex1, Vec2(-1.0f, -1.0f)), Vec2(-1.0f, -1.0f), EDGETYPE::VERTICAL, EDGEDIRECTION::EDGELEFT));
						}
						// top edge
						if ((j != 0) && !(GetTileType({ (float)i,(float)(j - 1) }) == TILETYPE::WALL))
						{
							vertex0 = Vec2((float)i, (float)j);
							vertex1 = Vec2((float)(i + 1), (float)j);
							allEdges.push_back(Edge(vertex0, vertex1, Line(vertex0, Vec2(-1.0f, -1.0f)), Line(vertex1, Vec2(-1.0f, -1.0f)), Vec2(-1.0f, -1.0f), EDGETYPE::HORIZONTAL, EDGEDIRECTION::EDGEUP));
						}
						// right edge
						if ((i != mapWidth - 1) && !(GetTileType({ (float)(i + 1),(float)(j) }) == TILETYPE::WALL))
						{
							vertex0 = Vec2((float)(i + 1), (float)j);
							vertex1 = Vec2((float)(i + 1), (float)(j + 1));
							allEdges.push_back(Edge(vertex0, vertex1, Line(vertex0, Vec2(-1.0f, -1.0f)), Line(vertex1, Vec2(-1.0f, -1.0f)), Vec2(-1.0f, -1.0f), EDGETYPE::VERTICAL, EDGEDIRECTION::EDGERIGHT));
						}
						// bot edge
						if ((j != mapHeight - 1) && !(GetTileType({ (float)i,(float)(j + 1) }) == TILETYPE::WALL))
						{
							vertex0 = Vec2((float)i, (float)(j + 1));
							vertex1 = Vec2((float)(i + 1), (float)(j + 1));
							allEdges.push_back(Edge(vertex0, vertex1, Line(vertex0, Vec2(-1.0f, -1.0f)), Line(vertex1, Vec2(-1.0f, -1.0f)), Vec2(-1.0f, -1.0f), EDGETYPE::HORIZONTAL, EDGEDIRECTION::EDGEDOWN));
						}
					}
				}
			}
		}

		// if vertex joined on to another, delete one and combine? (or similar)

		//for (int i = 1; i < allEdges.size(); i++)
		//{
		//	for (int j = 0; j < i; j++)
		//	{
		//		if (allEdges[i].vertex0 == allEdges[j].vertex0 && allEdges[i].vertex1 == allEdges[j].vertex1)
		//		{
		//			allEdges.erase(allEdges.begin() + i);
		//			allEdges.erase(allEdges.begin() + j);
		//			i--;
		//			i--;
		//			j--;
		//		}
		//	}
		//}

		for (int i = 1; i < allEdges.size(); )
		{
			bool foundMerger = false;
			for (int j = 0; j < i; )
			{
				{
					if (allEdges[i].IsNeighbor(allEdges[j]))
					{
						allEdges[i].MergeIntoNeighbor(allEdges[j]);
						allEdges.erase(allEdges.begin() + i);
						//std::swap(edges[i], edges.back());
						//edges.pop_back();
						foundMerger = true;
						break;
					}
					else
					{
						j++;
					}
				}
			}
			if (!foundMerger)
			{
				i++;
			}
		}


		for (auto k = allEdges.begin() + 1; k < allEdges.end(); k++)
		{
			for (auto m = allEdges.begin(); m < k; m++)
			{
				if (k->edgeDirection == EDGEDIRECTION::EDGELEFT
					&& (m->edgeDirection == EDGEDIRECTION::EDGEUP && k->vertex1 == m->vertex1))
				{
					k->vertex1Hold = true;
					m->vertex1Hold = true;
				}
				if (k->edgeDirection == EDGEDIRECTION::EDGELEFT
					&& (m->edgeDirection == EDGEDIRECTION::EDGEDOWN && k->vertex0 == m->vertex1))
				{
					k->vertex0Hold = true;
					m->vertex1Hold = true;
				}

				if (k->edgeDirection == EDGEDIRECTION::EDGEUP
					&& (m->edgeDirection == EDGEDIRECTION::EDGELEFT && k->vertex1 == m->vertex1))
				{
					k->vertex1Hold = true;
					m->vertex1Hold = true;
				}
				if (k->edgeDirection == EDGEDIRECTION::EDGEUP
					&& (m->edgeDirection == EDGEDIRECTION::EDGERIGHT && k->vertex0 == m->vertex1))
				{
					k->vertex0Hold = true;
					m->vertex1Hold = true;
				}

				if (k->edgeDirection == EDGEDIRECTION::EDGERIGHT
					&& (m->edgeDirection == EDGEDIRECTION::EDGEUP && k->vertex1 == m->vertex0))
				{
					k->vertex1Hold = true;
					m->vertex0Hold = true;
				}
				if (k->edgeDirection == EDGEDIRECTION::EDGERIGHT
					&& (m->edgeDirection == EDGEDIRECTION::EDGEDOWN && k->vertex0 == m->vertex0))
				{
					k->vertex0Hold = true;
					m->vertex0Hold = true;
				}

				if (k->edgeDirection == EDGEDIRECTION::EDGEDOWN
					&& (m->edgeDirection == EDGEDIRECTION::EDGELEFT && k->vertex1 == m->vertex0))
				{
					k->vertex1Hold = true;
					m->vertex0Hold = true;
				}
				if (k->edgeDirection == EDGEDIRECTION::EDGEDOWN
					&& (m->edgeDirection == EDGEDIRECTION::EDGERIGHT && k->vertex0 == m->vertex0))
				{
					k->vertex0Hold = true;
					m->vertex0Hold = true;
				}
			}
		}

		for (auto k = allEdges.begin(); k < allEdges.end(); k++)
		{
			if (k->edgeDirection == EDGEDIRECTION::EDGELEFT)
			{
				k->vertex0.x += edgeShift;
				k->vertex1.x += edgeShift;
				(!k->vertex0Hold) ? k->vertex0.y += edgeShift : k->vertex0.y -= edgeShift * 2.0f;
				(!k->vertex1Hold) ? k->vertex1.y -= edgeShift : k->vertex1.y += edgeShift * 2.0f;
				k->line0 = Line(k->vertex0, Vec2(-1.0f, -1.0f));
				k->line1 = Line(k->vertex1, Vec2(-1.0f, -1.0f));
			}
			if (k->edgeDirection == EDGEDIRECTION::EDGEUP)
			{
				(!k->vertex0Hold) ? k->vertex0.x += edgeShift : k->vertex0.x -= edgeShift * 2.0f;
				(!k->vertex1Hold) ? k->vertex1.x -= edgeShift : k->vertex1.x += edgeShift * 2.0f;
				k->vertex0.y += edgeShift;
				k->vertex1.y += edgeShift;
				k->line0 = Line(k->vertex0, Vec2(-1.0f, -1.0f));
				k->line1 = Line(k->vertex1, Vec2(-1.0f, -1.0f));
			}
			if (k->edgeDirection == EDGEDIRECTION::EDGERIGHT)
			{
				k->vertex0.x -= edgeShift;
				k->vertex1.x -= edgeShift;
				(!k->vertex0Hold) ? k->vertex0.y += edgeShift : k->vertex0.y -= edgeShift * 2.0f;
				(!k->vertex1Hold) ? k->vertex1.y -= edgeShift : k->vertex1.y += edgeShift * 2.0f;
				k->line0 = Line(k->vertex0, Vec2(-1.0f, -1.0f));
				k->line1 = Line(k->vertex1, Vec2(-1.0f, -1.0f));
			}
			if (k->edgeDirection == EDGEDIRECTION::EDGEDOWN)
			{
				(!k->vertex0Hold) ? k->vertex0.x += edgeShift : k->vertex0.x -= edgeShift * 2.0f;
				(!k->vertex1Hold) ? k->vertex1.x -= edgeShift : k->vertex1.x += edgeShift * 2.0f;
				k->vertex0.y -= edgeShift;
				k->vertex1.y -= edgeShift;
				k->line0 = Line(k->vertex0, Vec2(-1.0f, -1.0f));
				k->line1 = Line(k->vertex1, Vec2(-1.0f, -1.0f));
			}
		}



	}

	void Draw(Surface& gfx, Vec2 camLoc)
	{
		for (int j = 0; j < mapHeight; j++)
		{
			for (int i = 0; i < mapWidth; i++)
			{
				RectI selectedSpriteRect = { { tileWidth * mapSpace[j * mapWidth + i], 0 }, tileWidth, tileHeight };

				gfx.DrawSprite((int)(((float)i - camLoc.x) * (float)tileWidth + (float)(Graphics::ScreenWidth / 2)), (int)(((float)j - camLoc.y) * (float)tileHeight + (float)(Graphics::ScreenHeight / 2)), selectedSpriteRect, Graphics::GetScreenRect(), *spriteMap);
			}
		}
	}


	TILETYPE GetTileType(Vec2 loc)
	{
		if (loc.x >= 0.0f && loc.x < mapWidth && loc.y >= 0.0f && loc.x < mapHeight)
		{
			return mapSpace[(int)loc.y * mapWidth + (int)loc.x];
		}
		return TILETYPE::EMPTY;
	}

	int GetTileWidth()
	{
		return tileWidth;
	}

	int GetTileHeight()
	{
		return tileHeight;
	}

	Vec2 GetWorldDims()
	{
		return Vec2((float)mapWidth, (float)mapHeight);
	}

	int GetWorldWidth()
	{
		return mapWidth;
	}

	int GetWorldHeight()
	{
		return mapHeight;
	}

	std::vector<Edge>& GetAllEdges()
	{
		return allEdges;
	}

private:
	Surface* spriteMap = nullptr;

	std::vector<TILETYPE> mapSpace;
	const int mapWidth = 600;
	const int mapHeight = 600;
	//const int mapWidth = 25;
	//const int mapHeight = 25;

	const int tileWidth = 32;
	const int tileHeight = 24;

	std::vector<char> charMap;

	std::vector<Edge> allEdges;

};