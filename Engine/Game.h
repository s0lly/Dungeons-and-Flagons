/****************************************************************************************** 
 *	Chili DirectX Framework Version 16.07.20											  *	
 *	Game.h																				  *
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

#pragma once

#include "Keyboard.h"
#include "Mouse.h"
#include "Graphics.h"
#include "Surface.h"
#include "Camera.h"
#include "World.h"
#include <vector>
#include "RetroContent.h"
#include "Sound.h"
#include "GameManager.h"
#include "MiniMap.h"
#include <chrono>


enum GAMESTATE
{
	STARTSCREEN,
	MAINMENU,
	INGAME,
	LOADINGGAME,
	EXIT
};

enum MAINMENUSELECTOR
{
	NEWGAME,
	LOADGAME,
	EXITGAME
};

enum PAUSEMENUSELECTOR
{
	PAUSE_RETURNGAME,
	PAUSE_SAVEGAME,
	PAUSE_LOADGAME,
	PAUSE_EXITGAME
};

enum INGAMEMUSICSELECTED
{
	COMEANDFINDME,
	ASCENDING,
	DIGITALNATIVE
};

class Game
{
public:
	Game(class MainWindow& wnd);
	Game(const Game&) = delete;
	Game& operator=(const Game&) = delete;
	bool Go();
private:
	void ComposeFrame(float dt);
	void UpdateModel(float dt);
	/********************************/
	/*  User Functions              */
	/********************************/
private:
	MainWindow & wnd;
	Graphics gfx;
	/********************************/
	/*  User Variables              */
	/********************************/

	Surface knightSpriteMap = Surface("dnf_knight2.bmp");
	Surface ghostSpriteMap = Surface("dnf_ghost.bmp");
	Surface skeletonSpriteMap = Surface("dnf_skeleton.bmp");
	Surface flagonSpriteMap = Surface("dnf_flagon.bmp");
	Surface torchSpriteMap = Surface("dnf_torch2.bmp");
	Surface shieldPowerupSpriteMap = Surface("dnf_torch2.bmp");
	Surface worldSpriteMap = Surface("dnf_worldsprites2.bmp");

	Surface* knightSpriteMapPtr = &knightSpriteMap;
	Surface* ghostSpriteMapPtr = &ghostSpriteMap;
	Surface* skeletonSpriteMapPtr = &skeletonSpriteMap;
	Surface* flagonSpriteMapPtr = &flagonSpriteMap;
	Surface* torchSpriteMapPtr = &torchSpriteMap;
	Surface* shieldPowerupSpriteMapPtr = &shieldPowerupSpriteMap;
	Surface* worldSpriteMapPtr = &worldSpriteMap;

	std::vector<Surface*> spritePtrs;

	Camera camera;
	Knight knight;
	World world;
	std::vector<Monster*> monsterPtrs;

	bool gameOver = false;

	GAMESTATE gameState = GAMESTATE::STARTSCREEN;

	bool menuReturnPressed = false;

	MAINMENUSELECTOR mainMenuSelector = MAINMENUSELECTOR::NEWGAME;
	bool mainMenuDownPressed = false;
	bool mainMenuUpPressed = false;

	bool worldSetUp = false;

	//Vec2 knightStartLoc = { 13.05f, 15.2f };
	Vec2 knightStartLoc = { 299.99f, 299.99f };

	float masterVolume = 0.3f;
	float masterVolumeMin = 0.001f;
	float masterVolumeMax = 0.3f;

	Sound* musicSound = nullptr;
	Sound musicSound0;
	Sound musicSound1;
	Sound musicSound2;
	Sound musicStartSound;

	bool transitionGameStart = false;
	float transitionGameStartCounter = 0.0f;
	float transitionGameStartCounterTick = 1.0f;
	float transitionGameStartCounterMax = 100.0f;
	float transitionGameEndCounterMax = 150.0f;

	bool gameRunning = false;
	bool isPaused = false;

	bool escIsPressed = false;
	PAUSEMENUSELECTOR pauseMenuSelector = PAUSEMENUSELECTOR::PAUSE_RETURNGAME;

	std::chrono::system_clock::time_point start;
	std::chrono::system_clock::time_point end;

	std::chrono::system_clock::time_point startInGameMusic;
	std::chrono::system_clock::time_point currentInGameMusicTime;

	float inGameMusicLength[3] = {192.0f, 199.0f, 196.0f};

	INGAMEMUSICSELECTED inGameMusic = INGAMEMUSICSELECTED::COMEANDFINDME;

	GameManager gameManager;

	bool gameLoaded = false;

	bool transitionMenuAction = false;
	float transitionMenuActionCounter = 0.0f;
	float transitionMenuActionCounterTick = 1.0f;
	float transitionMenuActionCounterMax = 3.0f;
	std::string transitionMenuActionType = "";

	MiniMap miniMap;
	bool mIsPressed = false;

};