/****************************************************************************************** 
 *	Chili DirectX Framework Version 16.07.20											  *	
 *	Game.cpp																			  *
 *	Copyright 2016 PlanetChili.net <http://www.planetchili.net>							  *
 *																						  *
 *	This file is part of The Chili DirectX Framework.									  *
 *																						  *
 *	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
 *	it under the terms of the GNU General Public License as published by				  *
 *	the Free Software Foundation, either version 3 of the License, or					  *
 *	(at your option) any later version.													  *
 *																						  *
 *	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
 *	GNU General Public License for more details.										  *
 *																						  *
 *	You should have received a copy of the GNU General Public License					  *
 *	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
 ******************************************************************************************/

 /******************************************************************************************
 *	Game code and amendments by s0lly													   *
 *	https://www.youtube.com/channel/UCOyCXEB8NTx3Xjm8UptwsIg							   *
 *	https://s0lly.itch.io/																   *
 *	https://www.instagram.com/s0lly.gaming/												   *
 ******************************************************************************************/

#include "MainWindow.h"
#include "Game.h"

Game::Game(MainWindow& wnd)
	:
	wnd(wnd),
	gfx(wnd),
	knight(knightStartLoc, knightSpriteMapPtr),
	world(worldSpriteMapPtr),
	musicSound0(L"08 Ascending.wav"),
	musicSound1(L"05 Come and Find Me.wav"),
	musicSound2(L"13 Digital Native.wav"),
	musicStartSound(L"09 Come and Find Me - B mix.wav"),
	miniMap(world)
{
	
	musicStartSound.Play(1.0f, masterVolumeMax);
	SoundSystem::SetMasterVolume(masterVolume);

	spritePtrs.push_back(knightSpriteMapPtr);
	spritePtrs.push_back(ghostSpriteMapPtr);
	spritePtrs.push_back(skeletonSpriteMapPtr);
	spritePtrs.push_back(flagonSpriteMapPtr);
	spritePtrs.push_back(torchSpriteMapPtr);
	spritePtrs.push_back(shieldPowerupSpriteMapPtr);
	spritePtrs.push_back(worldSpriteMapPtr);


}

bool Game::Go()
{
	end = std::chrono::system_clock::now();
	std::chrono::duration<float> elapsedTime = end - start;
	start = end;
	float dt = elapsedTime.count();

	gfx.BeginFrame();	
	UpdateModel(dt);
	ComposeFrame(dt);
	gfx.EndFrame();

	return gameOver;
}

void Game::UpdateModel(float dt)
{

	if (gameState == GAMESTATE::STARTSCREEN)
	{
		if (wnd.kbd.KeyIsPressed(VK_RETURN))
		{
			if (!menuReturnPressed)
			{
				menuReturnPressed = true;
				gameState = GAMESTATE::MAINMENU;
				mainMenuSelector = MAINMENUSELECTOR::NEWGAME;
			}
		}
		else
		{
			menuReturnPressed = false;
		}


	}

	else if (gameState == GAMESTATE::MAINMENU)
	{

		if (wnd.kbd.KeyIsPressed(VK_DOWN))
		{
			if (!mainMenuDownPressed)
			{
				mainMenuDownPressed = true;
				switch (mainMenuSelector)
				{
				case MAINMENUSELECTOR::NEWGAME:
					mainMenuSelector = MAINMENUSELECTOR::LOADGAME;
					break;
				case MAINMENUSELECTOR::LOADGAME:
					mainMenuSelector = MAINMENUSELECTOR::EXITGAME;
					break;
				}
			}
		}
		else
		{
			mainMenuDownPressed = false;
		}

		if (wnd.kbd.KeyIsPressed(VK_UP))
		{
			if (!mainMenuUpPressed)
			{
				mainMenuUpPressed = true;
				switch (mainMenuSelector)
				{
				case MAINMENUSELECTOR::LOADGAME:
					mainMenuSelector = MAINMENUSELECTOR::NEWGAME;
					break;
				case MAINMENUSELECTOR::EXITGAME:
					mainMenuSelector = MAINMENUSELECTOR::LOADGAME;
					break;
				}
			}
		}
		else
		{
			mainMenuUpPressed = false;
		}

		if (wnd.kbd.KeyIsPressed(VK_RETURN))
		{
			if (!menuReturnPressed)
			{
				menuReturnPressed = true;
				switch (mainMenuSelector)
				{
				case MAINMENUSELECTOR::NEWGAME:
					gameState = GAMESTATE::INGAME;
					break;
				case MAINMENUSELECTOR::LOADGAME:
					gameManager.LoadGame(monsterPtrs, knight, camera, spritePtrs, miniMap);
					gameLoaded = true;
					gameState = GAMESTATE::INGAME;
					break;
				case MAINMENUSELECTOR::EXITGAME:
					gameState = GAMESTATE::EXIT;
					break;
				}
			}
		}
		else
		{
			menuReturnPressed = false;
		}
	}


	else if (gameState == GAMESTATE::INGAME)
	{
		// Select world



			// Set up the appropriate world
		if (!worldSetUp)
		{
			// Initialise the world

			worldSetUp = true;



			// Read from Map File to generate WorldObjects
			
			std::vector<char> charMap;

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
						int mapCharToUse = (int)temp - 48;
						charMap.push_back(mapCharToUse);
					}
				}
			}

			charMap.pop_back();

			mapFile.close();

			// Initialise all objects here

			if (!gameLoaded)
			{

				knight.Init(knightStartLoc);
				camera.Init(knight.GetLoc());

				for (int i = 0; i < monsterPtrs.size(); i++)
				{
					delete monsterPtrs[i];
				}

				monsterPtrs.clear();


				for (int i = 0; i < charMap.size(); i++)
				{
					float mapX = (float)(i % (int)world.GetWorldDims().x) + 0.5f;
					float mapY = (float)(i / (int)world.GetWorldDims().x) + 0.5f;
					switch (charMap[i])
					{
					case 2:
						monsterPtrs.push_back(new Torch({ mapX, mapY }, torchSpriteMapPtr));
						break;
					case 3:
						monsterPtrs.push_back(new Ghost({ mapX, mapY }, ghostSpriteMapPtr, MOVETYPE::NONE));
						break;
					case 4:
						monsterPtrs.push_back(new Flagon({ mapX, mapY }, flagonSpriteMapPtr));
						break;
					case 5:
						monsterPtrs.push_back(new Skeleton({ mapX, mapY }, skeletonSpriteMapPtr, MOVETYPE::NONE));
						break;
					}
				}

				miniMap.Init();
			}
			
			transitionGameStart = true;
			isPaused = false;
		}

		else
		{
			if (!gameRunning)
			{
				if (transitionGameStart)
				{
					transitionGameStartCounter += transitionGameStartCounterTick;

					masterVolume = masterVolume * 0.96f;
					masterVolume = masterVolume < 0.05f ? 0.05f : masterVolume;
					SoundSystem::SetMasterVolume(masterVolume);

					if (!gameLoaded)
					{
						knight.BoostTorchPower(0.1f);
					}

					//knight.Move(MOVETYPE::LEFT, world, 0.0001f);
					//knight.Move(MOVETYPE::RIGHT, world, 0.0001f);
				}
				if (transitionGameStartCounter >= transitionGameStartCounterMax)
				{
					transitionGameStartCounter = 0.0f;
					gameRunning = true;
					gameLoaded = false;
					
					musicSound = &musicSound1;
					inGameMusic = INGAMEMUSICSELECTED::COMEANDFINDME;

					musicStartSound.StopAll();

					SoundSystem::SetMasterVolume(masterVolumeMax);
					masterVolume = masterVolumeMax;

					musicSound->Play(1.0f, masterVolume);

					startInGameMusic = std::chrono::system_clock::now();

					transitionGameStart = true;

					start = std::chrono::system_clock::now();
				}
			}
			
			// Play the game

			// Move the knight
			if (gameRunning)
			{
				if (!knight.GetGameOver())
				{

					if (wnd.kbd.KeyIsPressed(VK_ESCAPE))
					{
						if (!escIsPressed)
						{
							pauseMenuSelector = PAUSEMENUSELECTOR::PAUSE_RETURNGAME;
							isPaused = !isPaused;
						}
						escIsPressed = true;
					}
					else
					{
						escIsPressed = false;
					}



					if (!isPaused)
					{
						if (transitionMenuAction)
						{
							transitionMenuActionCounter += transitionMenuActionCounterTick * dt;
							if (transitionMenuActionCounter >= transitionMenuActionCounterMax)
							{
								transitionMenuAction = false;
								transitionMenuActionCounter = 0.0f;
								transitionMenuActionType = "";
							}
						}

						masterVolume = masterVolumeMax;

						knight.Update(wnd.kbd, world, dt);

						if (knight.IsCreatingTorchthrow())
						{
							monsterPtrs.push_back(new TorchthrowPowerup(knight.GetLoc(), shieldPowerupSpriteMapPtr, knight.GetLastDirection()));
						}

						std::vector<Monster*> thrownTorches;

						for (int i = 0; i < monsterPtrs.size(); i++)
						{
							if (monsterPtrs[i]->IsThrownTorch())
							{
								thrownTorches.push_back(monsterPtrs[i]);;
							}
						}


						Vec2 knightLoc = knight.GetLoc();
						float knightSize = knight.GetSize();

						float maxTorchDisplayDistance = 40.0f;

						for (int i = 0; i < monsterPtrs.size(); i++)
						{
							if (!monsterPtrs[i]->GetDestructionFlag() && abs(monsterPtrs[i]->GetLoc().x - knight.GetLoc().x) < maxTorchDisplayDistance && abs(monsterPtrs[i]->GetLoc().y - knight.GetLoc().y) < maxTorchDisplayDistance)
							{
								monsterPtrs[i]->Update(world, knight, thrownTorches, dt);
							}
						}

						camera.Update(knight);

						mIsPressed = wnd.kbd.KeyIsPressed('M');

						miniMap.Update(knight, world);

					}
					else
					{
						masterVolume = masterVolumeMax * 0.5f;

						if (wnd.kbd.KeyIsPressed(VK_DOWN))
						{
							if (!mainMenuDownPressed)
							{
								mainMenuDownPressed = true;
								switch (pauseMenuSelector)
								{
								case PAUSEMENUSELECTOR::PAUSE_RETURNGAME:
									pauseMenuSelector = PAUSEMENUSELECTOR::PAUSE_SAVEGAME;
									break;
								case PAUSEMENUSELECTOR::PAUSE_SAVEGAME:
									pauseMenuSelector = PAUSEMENUSELECTOR::PAUSE_LOADGAME;
									break;
								case PAUSEMENUSELECTOR::PAUSE_LOADGAME:
									pauseMenuSelector = PAUSEMENUSELECTOR::PAUSE_EXITGAME;
									break;
								}
							}
						}
						else
						{
							mainMenuDownPressed = false;
						}

						if (wnd.kbd.KeyIsPressed(VK_UP))
						{
							if (!mainMenuUpPressed)
							{
								mainMenuUpPressed = true;
								switch (pauseMenuSelector)
								{
								case PAUSEMENUSELECTOR::PAUSE_SAVEGAME:
									pauseMenuSelector = PAUSEMENUSELECTOR::PAUSE_RETURNGAME;
									break;
								case PAUSEMENUSELECTOR::PAUSE_LOADGAME:
									pauseMenuSelector = PAUSEMENUSELECTOR::PAUSE_SAVEGAME;
									break;
								case PAUSEMENUSELECTOR::PAUSE_EXITGAME:
									pauseMenuSelector = PAUSEMENUSELECTOR::PAUSE_LOADGAME;
									break;
								}
							}
						}
						else
						{
							mainMenuUpPressed = false;
						}

						if (wnd.kbd.KeyIsPressed(VK_RETURN))
						{
							if (!menuReturnPressed)
							{
								menuReturnPressed = true;
								switch (pauseMenuSelector)
								{
								case PAUSEMENUSELECTOR::PAUSE_RETURNGAME:
									isPaused = !isPaused;
									break;
								case PAUSEMENUSELECTOR::PAUSE_SAVEGAME:
									gameManager.SaveGame(monsterPtrs, knight, miniMap);
									isPaused = !isPaused;
									transitionMenuAction = true;
									transitionMenuActionType = "GAME SAVED";
									break;
								case PAUSEMENUSELECTOR::PAUSE_LOADGAME:
									gameManager.LoadGame(monsterPtrs, knight, camera, spritePtrs, miniMap);
									isPaused = !isPaused;
									transitionMenuAction = true;
									transitionMenuActionType = "GAME LOADED";
									break;
								case PAUSEMENUSELECTOR::PAUSE_EXITGAME:
									knight.SetGameOver();
									isPaused = !isPaused;
									transitionMenuAction = false;
									break;
								}
							}
						}
						else
						{
							menuReturnPressed = false;
						}
					}

					currentInGameMusicTime = std::chrono::system_clock::now();
					std::chrono::duration<float> elapsedInGameMusicTime = currentInGameMusicTime - startInGameMusic;


					if (elapsedInGameMusicTime.count() > inGameMusicLength[(int)inGameMusic] + 1.0f)
					{
						musicSound->StopAll();

						switch (inGameMusic)
						{
						case INGAMEMUSICSELECTED::ASCENDING:
							inGameMusic = INGAMEMUSICSELECTED::COMEANDFINDME;
							musicSound = &musicSound1;
							break;
						case INGAMEMUSICSELECTED::COMEANDFINDME:
							inGameMusic = INGAMEMUSICSELECTED::DIGITALNATIVE;
							musicSound = &musicSound2;
							break;
						case INGAMEMUSICSELECTED::DIGITALNATIVE:
							inGameMusic = INGAMEMUSICSELECTED::ASCENDING;
							musicSound = &musicSound0;
							break;
						}

						musicSound->Play(1.0f, masterVolume);
						startInGameMusic = std::chrono::system_clock::now();
					}
				}
				else
				{

					if (transitionGameStart)
					{
						transitionGameStartCounter += transitionGameStartCounterTick;

						masterVolume = masterVolume * 0.98f;
						SoundSystem::SetMasterVolume(masterVolume);

						float torchPower = knight.GetTorchPower();
						knight.BoostTorchPower(-torchPower * 0.06f);

						knight.BoostTorchPower(-torchPower * 0.06f);

						if (knight.GetTorchPower() < 0.001f)
						{
							knight.SetTorchPower(0.0f);
						}
					}
					if (transitionGameStartCounter >= transitionGameEndCounterMax)
					{
						transitionGameStartCounter = 0.0f;
						gameRunning = false;

						SoundSystem::SetMasterVolume(masterVolumeMax);
						masterVolume = masterVolumeMax;

						musicSound->StopAll();

						musicStartSound.Play(1.0f, masterVolume);

						transitionGameStart = false;

						// REVISE: MAKE THIS REFER TO WIN SCREEN IN GAME FIRST
						gameState = GAMESTATE::MAINMENU;
						mainMenuSelector = MAINMENUSELECTOR::NEWGAME;
						worldSetUp = false;
					}
				}
			}
		}

	}

	else if (gameState == GAMESTATE::EXIT)
	{
		gameOver = true;
	}

}

void Game::ComposeFrame(float dt)
{
	if (gameState == GAMESTATE::STARTSCREEN)
	{
		RetroContent::DrawString(gfx, "DUNGEONS AND FLAGONS", { gfx.ScreenWidth / 2, gfx.ScreenHeight * 4 / 40 }, 5, Color(0, 200, 200));

		RetroContent::DrawString(gfx, "BY", { gfx.ScreenWidth / 2, gfx.ScreenHeight * 9 / 40 }, 2, Color(200, 0, 200));
		RetroContent::DrawString(gfx, "SOLLY", { gfx.ScreenWidth / 2, gfx.ScreenHeight * 11 / 40 }, 2, Color(200, 0, 200));

		RetroContent::DrawString(gfx, "WITH LIMITED LIGHT YOU MUST ESCAPE", { gfx.ScreenWidth / 2, gfx.ScreenHeight * 17 / 40 }, 2, Color(0, 0, 200));
		RetroContent::DrawString(gfx, "BEFORE THE DARKNESS DESCENDS", { gfx.ScreenWidth / 2, gfx.ScreenHeight * 20 / 40 }, 2, Color(0, 0, 200));

		RetroContent::DrawString(gfx, "PRESS ANY KEY TO BEGIN", { gfx.ScreenWidth / 2, gfx.ScreenHeight * 32 / 40 }, 4, Color(200, 200, 0));
		RetroContent::DrawString(gfx, "TO DISCOVER THE PATH TO LIGHT", { gfx.ScreenWidth / 2, gfx.ScreenHeight * 35 / 40 }, 2, Color(200, 200, 0));

	}
	else if (gameState == GAMESTATE::MAINMENU)
	{
		RetroContent::DrawString(gfx, "DUNGEONS AND FLAGONS", { gfx.ScreenWidth / 2, gfx.ScreenHeight * 4 / 40 }, 5, Color(0, 200, 200));

		RetroContent::DrawString(gfx, "NEW GAME", { gfx.ScreenWidth / 2, gfx.ScreenHeight * 16 / 40 }, 3, (mainMenuSelector == MAINMENUSELECTOR::NEWGAME) ? Color(200, 200, 0) : Color(200, 0, 200));
		RetroContent::DrawString(gfx, "LOAD GAME", { gfx.ScreenWidth / 2, gfx.ScreenHeight * 24 / 40 }, 3, (mainMenuSelector == MAINMENUSELECTOR::LOADGAME) ? Color(200, 200, 0) : Color(200, 0, 200));
		RetroContent::DrawString(gfx, "EXIT", { gfx.ScreenWidth / 2, gfx.ScreenHeight * 32 / 40 }, 3, (mainMenuSelector == MAINMENUSELECTOR::EXITGAME) ? Color(200, 200, 0) : Color(200, 0, 200));
	}
	else if (gameState == GAMESTATE::INGAME)
	{
		if (!worldSetUp)
		{
			//Add Loading Screen
			//RetroContent::DrawString(gfx, "LOADING", { (float)(paddingSides + boxWidth / 2), (float)(paddingTopBot + boxHeight + (int)((float)selectorWidth * 4.0f)) }, 3, Color(255, 200, 100));
		}
		else
		{
			camera.DrawView(gfx, world, knight, monsterPtrs);

			// Show FPS
			//RetroContent::DrawString(gfx, "FPS", { gfx.ScreenWidth * 4 / 40, gfx.ScreenHeight * 2 / 40 }, 2, Color(0, 200, 200));
			//RetroContent::DrawString(gfx, std::to_string((int)(1.0f / dt)), { gfx.ScreenWidth * 4 / 40, gfx.ScreenHeight * 4 / 40 }, 2, Color(0, 200, 200));


			if (knight.GetGameOver())
			{
				if (knight.GetGameWon())
				{
					RetroContent::DrawString(gfx, "YOU HAVE ESCAPED THE DUNGEON", { gfx.ScreenWidth / 2, gfx.ScreenHeight * 16 / 40 }, 3, Color(200, 200, 100));

				}
				else
				{
					RetroContent::DrawString(gfx, "YOU SUCCUMBED TO THE DARKNESS", { gfx.ScreenWidth / 2, gfx.ScreenHeight * 16 / 40 }, 3, Color(200, 0, 0));
				}
			}
			else
			{
				if (isPaused)
				{
					RetroContent::DrawString(gfx, "GAME PAUSED", { gfx.ScreenWidth / 2, gfx.ScreenHeight * 4 / 40 }, 5, Color(0, 200, 200));

					RetroContent::DrawString(gfx, "RETURN TO GAME", { gfx.ScreenWidth / 2, gfx.ScreenHeight * 16 / 40 }, 3, (pauseMenuSelector == PAUSEMENUSELECTOR::PAUSE_RETURNGAME) ? Color(200, 200, 0) : Color(200, 0, 200));
					RetroContent::DrawString(gfx, "SAVE", { gfx.ScreenWidth / 2, gfx.ScreenHeight * 20 / 40 }, 3, (pauseMenuSelector == PAUSEMENUSELECTOR::PAUSE_SAVEGAME) ? Color(200, 200, 0) : Color(200, 0, 200));
					RetroContent::DrawString(gfx, "LOAD", { gfx.ScreenWidth / 2, gfx.ScreenHeight * 24 / 40 }, 3, (pauseMenuSelector == PAUSEMENUSELECTOR::PAUSE_LOADGAME) ? Color(200, 200, 0) : Color(200, 0, 200));
					RetroContent::DrawString(gfx, "EXIT", { gfx.ScreenWidth / 2, gfx.ScreenHeight * 28 / 40 }, 3, (pauseMenuSelector == PAUSEMENUSELECTOR::PAUSE_EXITGAME) ? Color(200, 200, 0) : Color(200, 0, 200));
				}
				else
				{
					if (transitionMenuAction)
					{
						RetroContent::DrawString(gfx, transitionMenuActionType, { gfx.ScreenWidth / 2, gfx.ScreenHeight * 10 / 40 }, 3, Color(255, 200, 200));
					}
					if (mIsPressed)
					{
						miniMap.Draw(gfx, world);
					}
				}
			}
		}

	}
	else if (gameState == GAMESTATE::EXIT)
	{

	}


	
}
