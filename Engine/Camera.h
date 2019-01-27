/******************************************************************************************
*	Game code and amendments by s0lly													   *
*	https://www.youtube.com/channel/UCOyCXEB8NTx3Xjm8UptwsIg							   *
*	https://s0lly.itch.io/																   *
*	https://www.instagram.com/s0lly.gaming/												   *
******************************************************************************************/

#pragma once

#include "World.h"
#include "WorldObject.h"
#include <algorithm>
#include "RetroContent.h"
#include <chrono>
#include <algorithm>
#include <functional>
#include <thread>




class Camera
{
public:
	Camera()
	{
		loc = { 12.5f, 12.5f };

		for (int y = 0; y < cameraSurface.GetHeight(); y++)
		{
			for (int x = 0; x < cameraSurface.GetWidth(); x++)
			{
				float distSqrd = (float)((x - cameraSurface.GetWidth() / 2) * (x - cameraSurface.GetWidth() / 2) + (y - cameraSurface.GetHeight() / 2) * (y - cameraSurface.GetHeight() / 2));
				//distSqrds.push_back(pow(distSqrd, 0.5));
				//distSqrd = distSqrd > 100000.0f ? distSqrd * 6.0f : distSqrd;
				float distCalc = 1.0f / (distSqrd) * 200.0f; // 400.0f
				distCalc = distCalc > 1.0f ? 1.0f : distCalc;
				distSqrdsInv.push_back(distCalc);//
			}
		}


		pixelFOVToggle = std::vector<bool>(cameraSurface.GetWidth() * cameraSurface.GetHeight(), true);

		newVec = std::vector<bool>(cameraSurface.GetWidth() * cameraSurface.GetHeight(), true);

	}

	void Init(Vec2 knightLoc)
	{
		loc = knightLoc;
	}
	void Update(Knight& knight)
	{
		loc = knight.GetLoc();
	}
	void DrawView(Graphics& gfx, World& world, Knight& knight, std::vector<Monster*> monsterPtrs)
	{
		
		
		

		// Draw the world
		cameraSurface.Refresh();

		world.Draw(cameraSurface, loc);


		// Draw Monsters

		for (int i = 0; i < monsterPtrs.size(); i++)
		{
			if (!monsterPtrs[i]->GetDestructionFlag())
			{
				monsterPtrs[i]->Draw(cameraSurface, loc);
			}
		}
		


		// Draw the Knight

		knight.Draw(cameraSurface, loc);

		//gfx.DrawSprite(0, 0, cameraSurface);

		

		float dt = AddLighting(world, knight, monsterPtrs);

		

		gfx.DrawSprite(0, 0, cameraSurface);

		
		// Performance Testing Info

		//RetroContent::DrawString(gfx, std::to_string((int)dt), { gfx.ScreenWidth / 2, gfx.ScreenHeight * 2 / 40 }, 2, Color(200, 0, 0));
		//RetroContent::DrawString(gfx, std::to_string(edgeData.numEdgesMerged), { gfx.ScreenWidth / 2, gfx.ScreenHeight * 4 / 40 }, 2, Color(0, 200, 0));
		//RetroContent::DrawString(gfx, std::to_string(edgeData.numEdgesCalculated), { gfx.ScreenWidth / 2, gfx.ScreenHeight * 6 / 40 }, 2, Color(0, 0, 200));

		

	}





	float AddLighting(World& world, Knight& knight, std::vector<Monster*> monsterPtrs)
	{

		std::chrono::system_clock::time_point start;
		std::chrono::system_clock::time_point end;


		int maxThreads = 10;


		

		// Deselect pixels to draw via shadow casting

		//for (int j = 0; j < cameraSurface.GetHeight(); j++)
		//{
		//	for (int i = 0; i < cameraSurface.GetWidth(); i++)
		//	{
		//		pixelFOVToggle[j * cameraSurface.GetWidth() + i] = true;
		//	}
		//}

		pixelFOVToggle = std::vector<bool>(cameraSurface.GetWidth() * cameraSurface.GetHeight(), true);


		// top left corner of camera view:

		std::vector<Edge> edges = world.GetAllEdges();

		float edgeIncludeLeft = knight.GetLoc().x - cameraSurface.GetWidth() * 2 / world.GetTileWidth();
		float edgeIncludeRight = knight.GetLoc().x + cameraSurface.GetWidth() * 2 / world.GetTileWidth();
		float edgeIncludeTop = knight.GetLoc().y - cameraSurface.GetHeight() * 2 / world.GetTileHeight();
		float edgeIncludeBot = knight.GetLoc().y + cameraSurface.GetHeight() * 2 / world.GetTileHeight();

		for (int k = 0; k < edges.size();)
		{
			if (edges[k].IsHorizontal())
			{
				if (!(edges[k].vertex0.y >= edgeIncludeTop && edges[k].vertex0.y <= edgeIncludeBot
					&& edgeIncludeLeft <= edges[k].vertex1.x && edgeIncludeRight >= edges[k].vertex0.x))
				{
					edges.erase(edges.begin() + k);
				}
				else
				{
					k++;
				}
			}
			else
			{
				if (!(edges[k].vertex0.x >= edgeIncludeLeft && edges[k].vertex0.x <= edgeIncludeRight
					&& edgeIncludeTop <= edges[k].vertex1.y && edgeIncludeBot >= edges[k].vertex0.y))
				{
					edges.erase(edges.begin() + k);
				}
				else
				{
					k++;
				}
			}
		}

		for (int k = 0; k < edges.size(); k++)
		{
			edges[k].line0 = Line(edges[k].vertex0, knight.GetLoc());
			edges[k].line1 = Line(edges[k].vertex1, knight.GetLoc());
			//float distSqrd0 = (edges[k].vertex0.x - knight.GetLoc().x) * (edges[k].vertex0.x - knight.GetLoc().x) + (edges[k].vertex0.y - knight.GetLoc().y) * (edges[k].vertex0.y - knight.GetLoc().y);
			//float distSqrd1 = (edges[k].vertex1.x - knight.GetLoc().x) * (edges[k].vertex1.x - knight.GetLoc().x) + (edges[k].vertex1.y - knight.GetLoc().y) * (edges[k].vertex1.y - knight.GetLoc().y);
			//
			//edges[k].smallestDistSqrd = distSqrd0 < distSqrd1 ? distSqrd0 : distSqrd1;

			float smallestYDist = 0.0f;
			float smallestXDist = 0.0f;

			if (edges[k].IsHorizontal())
			{
				smallestYDist = (edges[k].vertex0.y - knight.GetLoc().y);
				if (!(knight.GetLoc().x > edges[k].vertex0.x && knight.GetLoc().x < edges[k].vertex1.x))
				{
					smallestXDist = abs(knight.GetLoc().x - edges[k].vertex0.x) < abs(knight.GetLoc().x - edges[k].vertex1.x) ? knight.GetLoc().x - edges[k].vertex0.x : knight.GetLoc().x - edges[k].vertex1.x;
				}
			}
			else
			{
				smallestXDist = (edges[k].vertex0.x - knight.GetLoc().x);
				if (!(knight.GetLoc().y > edges[k].vertex0.y && knight.GetLoc().y < edges[k].vertex1.y))
				{
					smallestYDist = abs(knight.GetLoc().y - edges[k].vertex0.y) < abs(knight.GetLoc().y - edges[k].vertex1.y) ? knight.GetLoc().y - edges[k].vertex0.y : knight.GetLoc().y - edges[k].vertex1.y;
				}
			}


			edges[k].smallestDistSqrd = smallestXDist * smallestXDist + smallestYDist * smallestYDist;



		}

		
		

		// sort edges by closest edge

		std::sort(edges.begin(), edges.end());

		

		// sort the edges by distance to player?
		// if both vertices already untoggled, don't continue looking at that edge?
		// allow for pure horizontal / vertical lines
		// ambient lighting?

		int wallVisibility = 0;

		for (auto k = edges.begin(); k < edges.end(); k++)
		{
			// if edge is in shadow, do not compute
			bool isVisible = true;

			{
				// if edge is horizontal
				if (k->IsHorizontal())
				{
					// check if any part of the wall is visible; if not, don't compute edge
					int pStartX = (int)((k->vertex0.x - knight.GetLoc().x) * (float)world.GetTileWidth()) + cameraSurface.GetWidth() / 2;
					int pStartY = (int)((k->vertex0.y - knight.GetLoc().y) * (float)world.GetTileHeight()) + cameraSurface.GetHeight() / 2;
					int nonVisibilityCounter = 0;
					for (int pY = pStartY; pY <= pStartY; pY++)
					{
						for (int pX = pStartX; pX < pStartX + world.GetTileWidth() * k->size; pX++)
						{
							if (pY < 0 || pY >= cameraSurface.GetHeight() || pX < 0 || pX >= cameraSurface.GetWidth())
							{
								nonVisibilityCounter++;
							}
							else if (pixelFOVToggle[(pY < 0 ? 0 : (pY >= cameraSurface.GetHeight() ? cameraSurface.GetHeight() - 1 : pY)) * cameraSurface.GetWidth() + (pX < 0 ? 0 : (pX >= cameraSurface.GetWidth() ? cameraSurface.GetWidth() - 1 : pX))] == false)
							{
								nonVisibilityCounter++;
							}
						}
					}
					if (nonVisibilityCounter == (world.GetTileWidth() * k->size))
					{
						isVisible = false;
					}
					if (isVisible)
					{
						// if edge is below player
						if (k->vertex0.y > knight.GetLoc().y)
						{
							for (int j = (int)((k->vertex0.y - knight.GetLoc().y) * (float)world.GetTileHeight()) + cameraSurface.GetHeight() / 2 + 1 + wallVisibility; j < cameraSurface.GetHeight() + 1; j++)
							{
								int x0 = (int)((k->line0.GetX(knight.GetLoc().y + (float)((float)j - (float)cameraSurface.GetHeight() / (float)2) / (float)world.GetTileHeight()) - knight.GetLoc().x) * (float)world.GetTileWidth()) + cameraSurface.GetWidth() / 2;
								int x1 = (int)((k->line1.GetX(knight.GetLoc().y + (float)((float)j - (float)cameraSurface.GetHeight() / (float)2) / (float)world.GetTileHeight()) - knight.GetLoc().x) * (float)world.GetTileWidth()) + cameraSurface.GetWidth() / 2;
								if (x0 > x1)
								{
									std::swap(x0, x1);
								}
								x0 = x0 < 0 ? 0 : x0;
								x1 = x1 >= cameraSurface.GetWidth() ? cameraSurface.GetWidth() - 1 : x1;
								for (int i = (int)x0; i <= x1; i++)
								{
									pixelFOVToggle[(j - 1) * cameraSurface.GetWidth() + i] = false;
								}
							}
						}

						//// if edge is above player
						if (k->vertex0.y < knight.GetLoc().y)
						{
							for (int j = 0; j < (int)((k->vertex0.y - knight.GetLoc().y) * (float)world.GetTileHeight()) + cameraSurface.GetHeight() / 2 - wallVisibility; j++)
							{
								int x0 = (int)((k->line0.GetX(knight.GetLoc().y - (float)(((float)cameraSurface.GetHeight() / (float)2) / (float)world.GetTileHeight()) + (float)(j) / (float)world.GetTileHeight()) - knight.GetLoc().x) * (float)world.GetTileWidth()) + cameraSurface.GetWidth() / 2;
								int x1 = (int)((k->line1.GetX(knight.GetLoc().y - (float)(((float)cameraSurface.GetHeight() / (float)2) / (float)world.GetTileHeight()) + (float)(j) / (float)world.GetTileHeight()) - knight.GetLoc().x) * (float)world.GetTileWidth()) + cameraSurface.GetWidth() / 2;
								if (x0 > x1)
								{
									std::swap(x0, x1);
								}
								x0 = x0 < 0 ? 0 : x0;
								x1 = x1 >= cameraSurface.GetWidth() ? cameraSurface.GetWidth() - 1 : x1;

								for (int i = x0; i <= x1; i++)
								{
									pixelFOVToggle[j * cameraSurface.GetWidth() + i] = false;
								}

							}
						}
					}
				}

				if (!k->IsHorizontal())
				{
					// check if any part of the wall is visible; if not, don't compute edge
					int pXStart = (int)((k->vertex0.x - knight.GetLoc().x) * (float)world.GetTileWidth()) + cameraSurface.GetWidth() / 2;
					int pStartY = (int)((k->vertex0.y - knight.GetLoc().y) * (float)world.GetTileHeight()) + cameraSurface.GetHeight() / 2;
					int nonVisibilityCounter = 0;
					for (int pX = pXStart - 1; pX <= pXStart + 1; pX++)
					{
						for (int pY = pStartY - 1; pY < pStartY + world.GetTileHeight() * k->size + 1; pY++)
						{
							if (pY < 0 || pY >= cameraSurface.GetHeight() || pX < 0 || pX >= cameraSurface.GetWidth())
							{
								nonVisibilityCounter++;
							}
							else if (pixelFOVToggle[(pY < 0 ? 0 : (pY >= cameraSurface.GetHeight() ? cameraSurface.GetHeight() - 1 : pY)) * cameraSurface.GetWidth() + (pX < 0 ? 0 : (pX >= cameraSurface.GetWidth() ? cameraSurface.GetWidth() - 1 : pX))] == false)
							{
								nonVisibilityCounter++;
							}
						}
					}
					if (nonVisibilityCounter == (world.GetTileHeight() * k->size + 2) * 3)
					{
						isVisible = false;
					}
					if (isVisible)
					{
						// if edge is right of player
						if (k->vertex0.x > knight.GetLoc().x)
						{
							for (int j = (int)((k->vertex0.x - knight.GetLoc().x) * (float)world.GetTileWidth()) + cameraSurface.GetWidth() / 2 + 1 + wallVisibility; j < cameraSurface.GetWidth() + 1; j++)
							{
								int y0 = (int)((k->line0.GetY(knight.GetLoc().x + (float)(j - cameraSurface.GetWidth() / 2) / (float)world.GetTileWidth()) - knight.GetLoc().y) * (float)world.GetTileHeight()) + cameraSurface.GetHeight() / 2;
								int y1 = (int)((k->line1.GetY(knight.GetLoc().x + (float)(j - cameraSurface.GetWidth() / 2) / (float)world.GetTileWidth()) - knight.GetLoc().y) * (float)world.GetTileHeight()) + cameraSurface.GetHeight() / 2;
								if (y0 > y1)
								{
									std::swap(y0, y1);
								}
								y0 = y0 < 0 ? 0 : y0;
								y1 = y1 >= cameraSurface.GetHeight() ? cameraSurface.GetHeight() - 1 : y1;
								for (int i = (int)y0; i <= y1; i++)
								{
									pixelFOVToggle[i * cameraSurface.GetWidth() + j - 1] = false;
								}
							}
						}

						// if edge is to left of player
						if (k->vertex0.x < knight.GetLoc().x)
						{
							for (int j = 0; j < (k->vertex0.x - knight.GetLoc().x) * world.GetTileWidth() + cameraSurface.GetWidth() / 2 - wallVisibility; j++)
							{
								int y0 = (int)((k->line0.GetY(knight.GetLoc().x - (float)(((float)cameraSurface.GetWidth() / (float)2) / (float)world.GetTileWidth()) + (float)(j) / (float)world.GetTileWidth()) - knight.GetLoc().y) * (float)world.GetTileHeight()) + cameraSurface.GetHeight() / 2;
								int y1 = (int)((k->line1.GetY(knight.GetLoc().x - (float)(((float)cameraSurface.GetWidth() / (float)2) / (float)world.GetTileWidth()) + (float)(j) / (float)world.GetTileWidth()) - knight.GetLoc().y) * (float)world.GetTileHeight()) + cameraSurface.GetHeight() / 2;
								if (y0 > y1)
								{
									std::swap(y0, y1);
								}
								y0 = y0 < 0 ? 0 : y0;
								y1 = y1 >= cameraSurface.GetHeight() ? cameraSurface.GetHeight() - 1 : y1;
								for (int i = (int)y0; i <= y1; i++)
								{
									pixelFOVToggle[i * cameraSurface.GetWidth() + j] = false;
								}
							}
						}
					}
				}
			}
		}
		
		//cameraSurfaceLighting.AddFOV(cameraSurface, pixelFOVToggle);


		// Draw light sources - optimise!!

		std::vector<std::vector<Edge>> torchEdgesMap;

		std::vector<LightSource*> activeTorches;
		float maxTorchDisplayDistance = 40.0f;

		for (int i = 0; i < monsterPtrs.size(); i++)
		{
			if (LightSource* torch = dynamic_cast<LightSource*>(monsterPtrs[i]))
			{
				if (!torch->GetDestructionFlag() && abs(torch->GetLoc().x - knight.GetLoc().x) < maxTorchDisplayDistance && abs(torch->GetLoc().y - knight.GetLoc().y) < maxTorchDisplayDistance)
				{
					activeTorches.push_back(torch);

					torchEdgesMap.push_back(edges);


					for (int k = 0; k < torchEdgesMap[torchEdgesMap.size() - 1].size(); k++)
					{
						torchEdgesMap[torchEdgesMap.size() - 1][k].line0 = Line(torchEdgesMap[torchEdgesMap.size() - 1][k].vertex0, torch->GetLoc());
						torchEdgesMap[torchEdgesMap.size() - 1][k].line1 = Line(torchEdgesMap[torchEdgesMap.size() - 1][k].vertex1, torch->GetLoc());

						float smallestYDist = 0.0f;
						float smallestXDist = 0.0f;

						if (torchEdgesMap[torchEdgesMap.size() - 1][k].IsHorizontal())
						{
							smallestYDist = (torchEdgesMap[torchEdgesMap.size() - 1][k].vertex0.y - torch->GetLoc().y);
							if (!(torch->GetLoc().x > torchEdgesMap[torchEdgesMap.size() - 1][k].vertex0.x && torch->GetLoc().x < torchEdgesMap[torchEdgesMap.size() - 1][k].vertex1.x))
							{
								smallestXDist = abs(torch->GetLoc().x - torchEdgesMap[torchEdgesMap.size() - 1][k].vertex0.x) < abs(torch->GetLoc().x - torchEdgesMap[torchEdgesMap.size() - 1][k].vertex1.x) ? torch->GetLoc().x - torchEdgesMap[torchEdgesMap.size() - 1][k].vertex0.x : torch->GetLoc().x - torchEdgesMap[torchEdgesMap.size() - 1][k].vertex1.x;
							}
						}
						else
						{
							smallestXDist = (torchEdgesMap[torchEdgesMap.size() - 1][k].vertex0.x - torch->GetLoc().x);
							if (!(torch->GetLoc().y > torchEdgesMap[torchEdgesMap.size() - 1][k].vertex0.y && torch->GetLoc().y < torchEdgesMap[torchEdgesMap.size() - 1][k].vertex1.y))
							{
								smallestYDist = abs(torch->GetLoc().y - torchEdgesMap[torchEdgesMap.size() - 1][k].vertex0.y) < abs(torch->GetLoc().y - torchEdgesMap[torchEdgesMap.size() - 1][k].vertex1.y) ? torch->GetLoc().y - torchEdgesMap[torchEdgesMap.size() - 1][k].vertex0.y : torch->GetLoc().y - torchEdgesMap[torchEdgesMap.size() - 1][k].vertex1.y;
							}
						}


						torchEdgesMap[torchEdgesMap.size() - 1][k].smallestDistSqrd = smallestXDist * smallestXDist + smallestYDist * smallestYDist;


					}

					std::sort(torchEdgesMap[torchEdgesMap.size() - 1].begin(), torchEdgesMap[torchEdgesMap.size() - 1].end());
					
				}
			}
		}




		



		// Deselect pixels to light via torch's shadow casting

		std::vector<std::vector<bool>> torchPixelFOVToggleMap;

		for (int k = 0; k < activeTorches.size(); k++)
		{
			torchPixelFOVToggleMap.push_back(newVec);
		}


		

		start = std::chrono::system_clock::now();


		std::vector<std::thread> threadList3;

		
		for (int m = 0; m < activeTorches.size(); m++)
		{
			threadList3.push_back(std::thread([&activeTorches, m, &world, &torchPixelFOVToggleMap, &torchEdgesMap, &knight]()
			{
				for (auto k = torchEdgesMap[m].begin(); k < torchEdgesMap[m].end(); k++)
				{
					// if edge is in shadow, do not compute
					bool isVisible = true;

					{
						// if edge is horizontal
						if (k->IsHorizontal())
						{
							// check if any part of the wall is visible; if not, don't compute edge
							int pStartX = (int)((k->vertex0.x - activeTorches[m]->GetLoc().x) * (float)world.GetTileWidth()) + Graphics::ScreenWidth / 2;
							int pStartY = (int)((k->vertex0.y - activeTorches[m]->GetLoc().y) * (float)world.GetTileHeight()) + Graphics::ScreenHeight / 2;

							if (isVisible)
							{
								// if edge is below player
								if (k->vertex0.y > activeTorches[m]->GetLoc().y)
								{
									for (int j = (int)((k->vertex0.y - activeTorches[m]->GetLoc().y) * (float)world.GetTileHeight()) + Graphics::ScreenHeight / 2 + 1; j < Graphics::ScreenHeight + 1; j++)
									{
										int x0 = (int)((k->line0.GetX(activeTorches[m]->GetLoc().y + (float)((float)j - (float)Graphics::ScreenHeight / (float)2) / (float)world.GetTileHeight()) - activeTorches[m]->GetLoc().x) * (float)world.GetTileWidth()) + Graphics::ScreenWidth / 2;
										int x1 = (int)((k->line1.GetX(activeTorches[m]->GetLoc().y + (float)((float)j - (float)Graphics::ScreenHeight / (float)2) / (float)world.GetTileHeight()) - activeTorches[m]->GetLoc().x) * (float)world.GetTileWidth()) + Graphics::ScreenWidth / 2;
										if (x0 > x1)
										{
											std::swap(x0, x1);
										}
										x0 = x0 < 0 ? 0 : x0;
										x1 = x1 >= Graphics::ScreenWidth ? Graphics::ScreenWidth - 1 : x1;
										for (int i = (int)x0; i <= x1; i++)
										{
											torchPixelFOVToggleMap[m][(j - 1) * Graphics::ScreenWidth + i] = false;
										}
									}
								}

								//// if edge is above player
								if (k->vertex0.y < activeTorches[m]->GetLoc().y)
								{
									for (int j = 0; j < (int)((k->vertex0.y - activeTorches[m]->GetLoc().y) * (float)world.GetTileHeight()) + Graphics::ScreenHeight / 2; j++)
									{
										int x0 = (int)((k->line0.GetX(activeTorches[m]->GetLoc().y - (float)(((float)Graphics::ScreenHeight / (float)2) / (float)world.GetTileHeight()) + (float)(j) / (float)world.GetTileHeight()) - activeTorches[m]->GetLoc().x) * (float)world.GetTileWidth()) + Graphics::ScreenWidth / 2;
										int x1 = (int)((k->line1.GetX(activeTorches[m]->GetLoc().y - (float)(((float)Graphics::ScreenHeight / (float)2) / (float)world.GetTileHeight()) + (float)(j) / (float)world.GetTileHeight()) - activeTorches[m]->GetLoc().x) * (float)world.GetTileWidth()) + Graphics::ScreenWidth / 2;
										if (x0 > x1)
										{
											std::swap(x0, x1);
										}
										x0 = x0 < 0 ? 0 : x0;
										x1 = x1 >= Graphics::ScreenWidth ? Graphics::ScreenWidth - 1 : x1;

										for (int i = x0; i <= x1; i++)
										{
											torchPixelFOVToggleMap[m][j * Graphics::ScreenWidth + i] = false;
										}

									}
								}
							}
						}

						if (!k->IsHorizontal())
						{
							// check if any part of the wall is visible; if not, don't compute edge
							int pXStart = (int)((k->vertex0.x - activeTorches[m]->GetLoc().x) * (float)world.GetTileWidth()) + Graphics::ScreenWidth / 2;
							int pStartY = (int)((k->vertex0.y - activeTorches[m]->GetLoc().y) * (float)world.GetTileHeight()) + Graphics::ScreenHeight / 2;

							if (isVisible)
							{
								// if edge is right of player
								if (k->vertex0.x > activeTorches[m]->GetLoc().x)
								{
									for (int j = (int)((k->vertex0.x - activeTorches[m]->GetLoc().x) * (float)world.GetTileWidth()) + Graphics::ScreenWidth / 2 + 1; j < Graphics::ScreenWidth + 1; j++)
									{
										int y0 = (int)((k->line0.GetY(activeTorches[m]->GetLoc().x + (float)(j - Graphics::ScreenWidth / 2) / (float)world.GetTileWidth()) - activeTorches[m]->GetLoc().y) * (float)world.GetTileHeight()) + Graphics::ScreenHeight / 2;
										int y1 = (int)((k->line1.GetY(activeTorches[m]->GetLoc().x + (float)(j - Graphics::ScreenWidth / 2) / (float)world.GetTileWidth()) - activeTorches[m]->GetLoc().y) * (float)world.GetTileHeight()) + Graphics::ScreenHeight / 2;
										if (y0 > y1)
										{
											std::swap(y0, y1);
										}
										y0 = y0 < 0 ? 0 : y0;
										y1 = y1 >= Graphics::ScreenHeight ? Graphics::ScreenHeight - 1 : y1;
										for (int i = (int)y0; i <= y1; i++)
										{
											torchPixelFOVToggleMap[m][i * Graphics::ScreenWidth + j - 1] = false;
										}
									}
								}

								// if edge is to left of player
								if (k->vertex0.x < activeTorches[m]->GetLoc().x)
								{
									for (int j = 0; j < (int)((k->vertex0.x - activeTorches[m]->GetLoc().x) * (float)world.GetTileWidth()) + Graphics::ScreenWidth / 2; j++)
									{
										int y0 = (int)((k->line0.GetY(activeTorches[m]->GetLoc().x - (float)(((float)Graphics::ScreenWidth / (float)2) / (float)world.GetTileWidth()) + (float)(j) / (float)world.GetTileWidth()) - activeTorches[m]->GetLoc().y) * (float)world.GetTileHeight()) + Graphics::ScreenHeight / 2;
										int y1 = (int)((k->line1.GetY(activeTorches[m]->GetLoc().x - (float)(((float)Graphics::ScreenWidth / (float)2) / (float)world.GetTileWidth()) + (float)(j) / (float)world.GetTileWidth()) - activeTorches[m]->GetLoc().y) * (float)world.GetTileHeight()) + Graphics::ScreenHeight / 2;
										if (y0 > y1)
										{
											std::swap(y0, y1);
										}
										y0 = y0 < 0 ? 0 : y0;
										y1 = y1 >= Graphics::ScreenHeight ? Graphics::ScreenHeight - 1 : y1;
										for (int i = (int)y0; i <= y1; i++)
										{
											torchPixelFOVToggleMap[m][i * Graphics::ScreenWidth + j] = false;
										}
									}
								}
							}
						}
					}
				}
			}));
			
		}

		std::for_each(threadList3.begin(), threadList3.end(), std::mem_fn(&std::thread::join));
		



		std::vector<float>* distSqrdsInvPtr = &distSqrdsInv;
		std::vector<bool>* pixelFOVTogglePtr = &pixelFOVToggle;


		std::vector<std::thread> threadList2;

		float* appliedLighting = new float[Graphics::ScreenWidth*Graphics::ScreenHeight];

		for (int t = 0; t < maxThreads; t++)
		{
			threadList2.push_back(std::thread([t, maxThreads, pixelFOVTogglePtr, appliedLighting, &knight, distSqrdsInvPtr]()
			{
				for (int y = (Graphics::ScreenHeight * t) / maxThreads; y < (Graphics::ScreenHeight * (t + 1)) / maxThreads; y++)
				{
					for (int x = 0; x < Graphics::ScreenWidth; x++)
					{
						int mark = y * Graphics::ScreenWidth + x;

						if ((*pixelFOVTogglePtr)[mark])
						{
							appliedLighting[mark] = (*distSqrdsInvPtr)[mark] * (knight.GetTorchPower());
						}
						else
						{
							appliedLighting[mark] = 0.0f;
						}
					}
				}
			}));
		}

		std::for_each(threadList2.begin(), threadList2.end(), std::mem_fn(&std::thread::join));

		

		std::vector<std::thread> threadList;

		for (int i = 0; i < activeTorches.size(); i++)
		{
		
			threadList.push_back(std::thread([&activeTorches, i, &world, &torchPixelFOVToggleMap, appliedLighting, &knight, distSqrdsInvPtr]()
				{
				int torchXStart = (int)((knight.GetLoc().x - activeTorches[i]->GetLoc().x) * (float)world.GetTileWidth());
				int torchXEnd = (int)((knight.GetLoc().x - activeTorches[i]->GetLoc().x) * (float)world.GetTileWidth()) + Graphics::ScreenWidth;
				int torchYStart = (int)((knight.GetLoc().y - activeTorches[i]->GetLoc().y) * (float)world.GetTileHeight());
				int torchYEnd = (int)((knight.GetLoc().y - activeTorches[i]->GetLoc().y) * (float)world.GetTileHeight()) + Graphics::ScreenHeight;

				torchXEnd = torchXEnd >= Graphics::ScreenWidth ? Graphics::ScreenWidth : torchXEnd;
				torchYEnd = torchYEnd >= Graphics::ScreenHeight ? Graphics::ScreenHeight : torchYEnd;


				for (int y = (torchYStart < 0 ? 0 : torchYStart); y < torchYEnd; y++)
				{
					for (int x = (torchXStart < 0 ? 0 : torchXStart); x < torchXEnd; x++)
					{
						if (torchPixelFOVToggleMap[i][(y)* Graphics::ScreenWidth + (x)])
						{
							appliedLighting[(y - torchYStart) * Graphics::ScreenWidth + (x - torchXStart)] += ((*distSqrdsInvPtr)[y * Graphics::ScreenWidth + x] * 6.0f);
						}
					}
				}
			}));
		
		}

		std::for_each(threadList.begin(), threadList.end(), std::mem_fn(&std::thread::join));

		
		
		cameraSurface.ApplyLighting(pixelFOVToggle, appliedLighting);

		
		
		delete appliedLighting;


		end = std::chrono::system_clock::now();
		std::chrono::duration<float> elapsedTime = end - start;
		float dt = elapsedTime.count()*1000.0f;


		return dt;
	}



private:
	Vec2 loc = { -1.0f, -1.0f };
	Surface cameraSurface = Surface(Graphics::ScreenWidth, Graphics::ScreenHeight);
	std::vector<float> distSqrdsInv;
	std::vector<bool> pixelFOVToggle;
	std::vector<bool> newVec;
};