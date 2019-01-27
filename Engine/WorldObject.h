/******************************************************************************************
*	Game code and amendments by s0lly													   *
*	https://www.youtube.com/channel/UCOyCXEB8NTx3Xjm8UptwsIg							   *
*	https://s0lly.itch.io/																   *
*	https://www.instagram.com/s0lly.gaming/												   *
******************************************************************************************/

#pragma once

#include "Graphics.h"
#include "Surface.h"
#include "Vec2.h"
#include "Colors.h"
#include "RectI.h"
#include "World.h"
#include "Keyboard.h"

enum WORLDOBJECTANIMATION
{
	STILL,
	LEFTFOOT,
	RIGHTFOOT
};

class WorldObject
{
public:
	WorldObject(Vec2 in_loc, Surface* in_spriteMap)
	{
		loc = in_loc;
		spriteMap = in_spriteMap;
	}

	// Functions shared by all child classes

	void Draw(Surface& gfx, Vec2 camLoc)
	{
		//gfx.PutPixel((int)camLoc.x, (int)camLoc.y, c);
		int spriteSelect = 0;
		if (isWalking)
		{
			spriteSelect = (int)walkingAnimation;
		}
		RectI selectedSpriteRect = { { spriteSelect * tileWidth, 0 }, tileWidth, tileHeight };

		gfx.DrawSpriteScaledUpRotated((int)(((float)loc.x - camLoc.x) * (float)tileWidth - (float)(lastDirection == MOVETYPE::NONE || lastDirection == MOVETYPE::UP || lastDirection == MOVETYPE::DOWN ? tileWidth : tileHeight) / 2.0f * (float)scale + (float)(Graphics::ScreenWidth / 2)), (int)(((float)loc.y - camLoc.y) * (float)tileHeight - (float)(lastDirection == MOVETYPE::NONE || lastDirection == MOVETYPE::UP || lastDirection == MOVETYPE::DOWN ? tileHeight : tileWidth) / 2.0f * (float)scale + (float)(Graphics::ScreenHeight / 2)), selectedSpriteRect, Graphics::GetScreenRect(), *spriteMap, scale, lastDirection);
	}
	void Move(MOVETYPE direction, World& world, float dt)
	{
		Vec2 movement = { 0.0f, 0.0f };

		float horizontalNormalise = (float)world.GetTileHeight() / (float)world.GetTileWidth();

		switch (direction)
		{
		case MOVETYPE::NONE:
			movement = { 0.0f, 0.0f };
			break;
		case MOVETYPE::UP:
			movement = { 0.0f, -moveSpeed * dt };
			lastDirection = direction;
			break;
		case MOVETYPE::DOWN:
			movement = { 0.0f, +moveSpeed * dt };
			lastDirection = direction;
			break;
		case MOVETYPE::LEFT:
			movement = { -moveSpeed * dt * horizontalNormalise, 0.0f };
			lastDirection = direction;
			break;
		case MOVETYPE::RIGHT:
			movement = { +moveSpeed * dt * horizontalNormalise, 0.0f };
			lastDirection = direction;
			break;
		}
		if (!HasCollidedWithWall(movement, world))
		{
			loc = loc + movement;
		}
	}
	bool HasCollidedWithWall(Vec2 movement, World& world)
	{

		Vec2 topL = loc + Vec2(-size, -size);
		Vec2 topR = loc + Vec2(+size, -size);
		Vec2 botL = loc + Vec2(-size, +size);
		Vec2 botR = loc + Vec2(+size, +size);

		if (world.GetTileType(topL + movement) == TILETYPE::WALL || world.GetTileType(topR + movement) == TILETYPE::WALL
			|| world.GetTileType(botL + movement) == TILETYPE::WALL || world.GetTileType(botR + movement) == TILETYPE::WALL)
		{
			return true;
		}
		return false;
	}
	Vec2 GetLoc()
	{
		return loc;
	}
	MOVETYPE GetLastDirection()
	{
		return lastDirection;
	}
	float GetSize()
	{
		return size;
	}
	virtual ~WorldObject()
	{

	}

protected:
	Surface * spriteMap = nullptr;
	const int tileWidth = 32;
	const int tileHeight = 24;
	Vec2 loc = { -1.0f, -1.0f }; // Starting location is off world
	float size = 0.1f;
	float collisionScaler = 6.0f;
	int scale = 2;
	MOVETYPE lastDirection = MOVETYPE::NONE;
	float moveSpeed = 0.0f;
	WORLDOBJECTANIMATION walkingAnimation = WORLDOBJECTANIMATION::STILL;
	bool isWalking = false;
	float walkingTimer = 0.0f;
	float walkingTimerMax = 0.35f;
	

private:
	

};

class Knight : public WorldObject
{
public:
	Knight(Vec2 in_loc, Surface* in_spriteMap)
		:
		WorldObject(in_loc, in_spriteMap)
	{
		loc = in_loc;

		torchPower = torchPowerStart;
		gameOver = false;
		gameWon = false;
		moveSpeed = startMoveSpeed;
		walkingAnimation = WORLDOBJECTANIMATION::STILL;
		isWalking = false;
		walkingTimer = 0.0f;
		lastDirection = MOVETYPE::NONE;
	}

	void Init(Vec2 knightStartLoc)
	{
		loc = knightStartLoc;

		torchPower = torchPowerStart;
		gameOver = false;
		gameWon = false;
		moveSpeed = startMoveSpeed;
		walkingAnimation = WORLDOBJECTANIMATION::STILL;
		isWalking = false;
		lastDirection = MOVETYPE::NONE;
	}

	void Reinit(Vec2 knightStartLoc, MOVETYPE newDirection, float newTorchPower, float newSpeed)
	{
		loc = knightStartLoc;

		torchPower = newTorchPower;
		gameOver = false;
		gameWon = false;
		moveSpeed = newSpeed;
		walkingAnimation = WORLDOBJECTANIMATION::STILL;
		isWalking = false;
		lastDirection = newDirection;
	}


	void Update(Keyboard& kbd, World& world, float dt)
	{
		MOVETYPE knightMoveType = MOVETYPE::NONE;
		isWalking = false;

		int numDirections = 0;

		if (kbd.KeyIsPressed(VK_UP))
		{
			numDirections++;
		}
		if (kbd.KeyIsPressed(VK_DOWN))
		{
			numDirections++;
		}
		if (kbd.KeyIsPressed(VK_LEFT))
		{
			numDirections++;
		}
		if (kbd.KeyIsPressed(VK_RIGHT))
		{
			numDirections++;
		}

		float moveNormalise = numDirections > 1 ? pow(2.0f, 0.5f) : 1.0f;

		if (kbd.KeyIsPressed(VK_UP))
		{
			knightMoveType = MOVETYPE::UP;
			Move(knightMoveType, world, dt / moveNormalise);
			isWalking = true;
		}
		if (kbd.KeyIsPressed(VK_DOWN))
		{
			knightMoveType = MOVETYPE::DOWN;
			Move(knightMoveType, world, dt / moveNormalise);
			isWalking = true;
		}
		if (kbd.KeyIsPressed(VK_LEFT))
		{
			knightMoveType = MOVETYPE::LEFT;
			Move(knightMoveType, world, dt / moveNormalise);
			isWalking = true;
		}
		if (kbd.KeyIsPressed(VK_RIGHT))
		{
			knightMoveType = MOVETYPE::RIGHT;
			Move(knightMoveType, world, dt / moveNormalise);
			isWalking = true;
		}


		// Torch Throw
		if (kbd.KeyIsPressed(VK_SPACE))
		{
			if (!stopCreatingTorchthrow)
			{
				if (torchPower == torchPowerMin)
				{
					//gameOver = true;
				}
				else
				{
					creatingTorchthrowPowerup = true;
					stopCreatingTorchthrow = true;
				}
				torchPower -= torchPowerDecreaseFromThrow;
			}
			else
			{
				creatingTorchthrowPowerup = false;
			}
		}
		else
		{
			creatingTorchthrowPowerup = false;
			stopCreatingTorchthrow = false;
		}

		if (isWalking == true)
		{
			walkingTimer += dt;
			if (walkingAnimation == WORLDOBJECTANIMATION::STILL)
			{
				walkingAnimation = WORLDOBJECTANIMATION::LEFTFOOT;
			}
		}
		else
		{
			walkingTimer = 0.0f;
			walkingAnimation = WORLDOBJECTANIMATION::STILL;
		}

		if (walkingTimer > walkingTimerMax)
		{
			walkingTimer = 0.0f;
			if (walkingAnimation == WORLDOBJECTANIMATION::LEFTFOOT)
			{
				walkingAnimation = WORLDOBJECTANIMATION::RIGHTFOOT;
			}
			else if (walkingAnimation == WORLDOBJECTANIMATION::RIGHTFOOT)
			{
				walkingAnimation = WORLDOBJECTANIMATION::LEFTFOOT;
			}
		}


		// Update for GameState changes

		if (loc.x > world.GetWorldDims().x - 1.0f || loc.y > world.GetWorldDims().y - 1.0f
			|| loc.x < 0.0f || loc.y < 0.0f)
		{
			gameOver = true;
			gameWon = true;
		}

		torchPower -= torchPowerDecayRate * dt;
		torchPower = torchPower < torchPowerMin ? torchPowerMin : torchPower;

	}
	void BoostTorchPower(float powerBoost)
	{
		torchPower += powerBoost;
		torchPower = torchPower < 0.0f ? 0.0f : torchPower;
	}
	void Draw(Surface& gfx, Vec2 camLoc)
	{
		WorldObject::Draw(gfx, camLoc);
	}
	Vec2 GetLoc()
	{
		return WorldObject::GetLoc();
	}
	float GetTorchPower()
	{
		return torchPower;
	}
	void SetGameOver()
	{
		gameOver = true;
	}
	bool GetGameOver()
	{
		return gameOver;
	}
	bool GetGameWon()
	{
		return gameWon;
	}
	float GetSize()
	{
		return size;
	}
	float GetSpeed()
	{
		return moveSpeed;
	}
	void SetTorchPower(float powerSet)
	{
		torchPower = powerSet;
	}
	void IncreaseTorchPower(float extraTorchPower)
	{
		torchPower += extraTorchPower;
	}
	void IncreaseMoveSpeed()
	{
		moveSpeed += moveSpeedIncreaseFromFlagon;
	}
	void Attacked()
	{
		torchPower -= torchPowerDecreaseFromGhost;
		if (torchPower < 0.0f)
		{
			torchPower = 0.01f;
			gameOver = true;
			gameWon = false;
		}
	}
	bool IsCreatingTorchthrow()
	{
		return creatingTorchthrowPowerup;
	}

private:

	float torchPowerStart = 0.5f;
	float torchPower = torchPowerStart;
	float torchPowerDecayRate = 0.2f;
	float torchPowerMin = 0.5f;
	//float torchPowerBoost = 10.0f;

	float startMoveSpeed = 8.0f;

	bool gameOver = false;
	bool gameWon = false;

	float moveSpeedIncreaseFromFlagon = 4.0f;
	float torchPowerDecreaseFromGhost = 0.51f;
	float torchPowerDecreaseFromThrow = 0.51f;

	bool creatingTorchthrowPowerup = false;
	bool stopCreatingTorchthrow = false;
};
class Monster : public WorldObject
{
public:
	Monster(Vec2 in_loc, Surface* in_spriteMap)
		:
		WorldObject(in_loc, in_spriteMap)
	{

	}

	// Virtual functions to be overridden if nessecary
	virtual void Update(World& world, Knight& knight, std::vector<Monster*> &thrownTorches, float dt)
	{
		if (HasCollidedWithOther(knight))
		{
			PerformKnightCollision(knight);
		}
	}
	virtual void PerformKnightCollision(Knight& knight) = 0;

	// Functions shared by all child classes
	bool HasCollidedWithOther(WorldObject& knight)
	{


		Vec2 topL = loc + Vec2(-size * collisionScaler, -size * collisionScaler);
		Vec2 topR = loc + Vec2(+size * collisionScaler, -size * collisionScaler);
		Vec2 botL = loc + Vec2(-size * collisionScaler, +size * collisionScaler);
		Vec2 botR = loc + Vec2(+size * collisionScaler, +size * collisionScaler);

		Vec2 topLKnight = knight.GetLoc() + Vec2(-knight.GetSize() * collisionScaler, -knight.GetSize() * collisionScaler);
		Vec2 topRKnight = knight.GetLoc() + Vec2(+knight.GetSize() * collisionScaler, -knight.GetSize() * collisionScaler);
		Vec2 botLKnight = knight.GetLoc() + Vec2(-knight.GetSize() * collisionScaler, +knight.GetSize() * collisionScaler);
		Vec2 botRKnight = knight.GetLoc() + Vec2(+knight.GetSize() * collisionScaler, +knight.GetSize() * collisionScaler);

		if (topR.x > topLKnight.x && topL.x < topRKnight.x && botR.y > topRKnight.y && topR.y < botRKnight.y)
		{
			return true;
		}
		return false;
	}
	bool GetDestructionFlag()
	{
		return destructionFlag;
	}
	void SetDestructionFlag(bool val)
	{
		destructionFlag = val;
	}
	bool IsThrownTorch()
	{
		return isThrownTorch;
	}

protected:

	bool destructionFlag = false;

	bool xMove = true;

	bool moveTransitionLag = false;
	float moveTransitionCounter = 0.0f;
	float moveTransitionCounterTick = 1.0f;
	float moveTransitionCounterMax = 0.2f;

	bool isThrownTorch = false;

private:


};

class Ghost : public Monster
{
public:
	Ghost(Vec2 in_loc, Surface* in_spriteMap, MOVETYPE firstDirection)
		:
		Monster(in_loc, in_spriteMap)
	{
		moveSpeed = 6.0f;
		lastDirection = firstDirection;
	}

	void Update(World& world, Knight& knight, std::vector<Monster*> &thrownTorches, float dt) override
	{
		MOVETYPE ghostMoveType = lastDirection;

		if (moveTransitionLag)
		{
			if (moveTransitionCounter < moveTransitionCounterMax)
			{
				moveTransitionCounter += moveTransitionCounterTick * dt;
			}
			else
			{
				moveTransitionLag = false;
				moveTransitionCounter = 0.0f;
			}
		}

		if (!moveTransitionLag)
		{
			if (moveWaitTimer >= moveWaitMax)
			{
				moveWaitTimer = 0.0f;

				while (ghostMoveType == lastDirection)
				{
					ghostMoveType = (MOVETYPE)((int)(rand() % 6));
				}

				moveWaitMax = (float)(rand() % 4);

			}
			else
			{
				moveWaitTimer += 1.0f * dt;
			}
		}

		if (knight.GetTorchPower() > 2.0f && (knight.GetLoc() - loc).GetLengthSq() < knight.GetTorchPower() * 1.0f)
		{
			float xDist = loc.x - knight.GetLoc().x;
			float yDist = loc.y - knight.GetLoc().y;
			
			if (!moveTransitionLag)
			{
				if (abs(xDist) > abs(yDist))
				{
					xMove = true;
				}
				else
				{
					xMove = false;
				}

				if (xMove)
				{
					if (xDist > 0.0f)
					{
						ghostMoveType = MOVETYPE::RIGHT;
					}
					else
					{
						ghostMoveType = MOVETYPE::LEFT;
					}
				}
				else
				{
					if (yDist > 0.0f)
					{
						ghostMoveType = MOVETYPE::DOWN;
					}
					else
					{
						ghostMoveType = MOVETYPE::UP;
					}
				}

				moveTransitionLag = true;
			}
			
		}


		Move(ghostMoveType, world, dt);

		if (HasCollidedWithOther(knight))
		{
			PerformKnightCollision(knight);
		}

		for (int i = 0; i < thrownTorches.size(); i++)
		{
			if (!thrownTorches[i]->GetDestructionFlag() && HasCollidedWithOther(*thrownTorches[i]))
			{
				destructionFlag = true;
			}
		}
	}
	void PerformKnightCollision(Knight& knight) override
	{
		knight.Attacked();
		if (!knight.GetGameOver())
		{
			destructionFlag = true;
		}
	}

private:
	float moveWaitTimer = 0.0f;
	float moveWaitMax = 1.0f;
};

class LightSource : public Monster
{
public:
	LightSource(Vec2 in_loc, Surface* in_spriteMap)
		:
		Monster(in_loc, in_spriteMap)
	{
	}
	float GetTorchPower()
	{
		return torchPower;
	}
protected:
	float torchPower = 6.0f;
};


class Skeleton : public LightSource
{
public:
	Skeleton(Vec2 in_loc, Surface* in_spriteMap, MOVETYPE firstDirection)
		:
		LightSource(in_loc, in_spriteMap)
	{
		moveSpeed = 3.0f;
		lastDirection = firstDirection;
		walkingAnimation = WORLDOBJECTANIMATION::STILL;
		isWalking = false;
		lastDirection = MOVETYPE::NONE;
		scale = 3;
	}

	void Update(World& world, Knight& knight, std::vector<Monster*> &thrownTorches, float dt) override
	{
		MOVETYPE moveType = lastDirection;

		if (moveTransitionLag)
		{
			if (moveTransitionCounter < moveTransitionCounterMax)
			{
				moveTransitionCounter += moveTransitionCounterTick * dt;
			}
			else
			{
				moveTransitionLag = false;
				moveTransitionCounter = 0.0f;
			}
		}

		if (!moveTransitionLag)
		{
			if (moveWaitTimer >= moveWaitMax)
			{
				moveWaitTimer = 0.0f;

				while (moveType == lastDirection)
				{
					moveType = (MOVETYPE)((int)(rand() % 6));
				}

				moveWaitMax = (float)(rand() % 4);

			}
			else
			{
				moveWaitTimer += 1.0f * dt;
			}
		}

		if (knight.GetTorchPower() > 2.0f && (knight.GetLoc() - loc).GetLengthSq() < knight.GetTorchPower() * 2.0f)
		{
			float xDist = loc.x - knight.GetLoc().x;
			float yDist = loc.y - knight.GetLoc().y;

			
			if (!moveTransitionLag)
			{
				if (abs(xDist) > abs(yDist))
				{
					xMove = true;
				}
				else
				{
					xMove = false;
				}

				if (xMove)
				{
					if (xDist < 0.0f)
					{
						moveType = MOVETYPE::RIGHT;
					}
					else
					{
						moveType = MOVETYPE::LEFT;
					}
				}
				else
				{
					if (yDist < 0.0f)
					{
						moveType = MOVETYPE::DOWN;
					}
					else
					{
						moveType = MOVETYPE::UP;
					}
				}

				moveTransitionLag = true;
			}

		}



		Move(moveType, world, dt);

		if (HasCollidedWithOther(knight))
		{
			PerformKnightCollision(knight);
		}

		isWalking = (moveType != MOVETYPE::NONE);

		if (isWalking == true)
		{
			walkingTimer += dt;
			if (walkingAnimation == WORLDOBJECTANIMATION::STILL)
			{
				walkingAnimation = WORLDOBJECTANIMATION::LEFTFOOT;
			}
		}
		else
		{
			walkingTimer = 0.0f;
			walkingAnimation = WORLDOBJECTANIMATION::STILL;
		}

		if (walkingTimer > walkingTimerMax)
		{
			walkingTimer = 0.0f;
			if (walkingAnimation == WORLDOBJECTANIMATION::LEFTFOOT)
			{
				walkingAnimation = WORLDOBJECTANIMATION::RIGHTFOOT;
			}
			else if (walkingAnimation == WORLDOBJECTANIMATION::RIGHTFOOT)
			{
				walkingAnimation = WORLDOBJECTANIMATION::LEFTFOOT;
			}
		}
	}
	void PerformKnightCollision(Knight& knight) override
	{
		//knight.IncreaseTorchPower(torchPower);
		knight.SetGameOver();
		if (!knight.GetGameOver())
		{
			destructionFlag = true;
		}
	}

private:
	float moveWaitTimer = 0.0f;
	float moveWaitMax = 1.0f;
};


class Torch : public LightSource
{
public:
	Torch(Vec2 in_loc, Surface* in_spriteMap)
		:
		LightSource(in_loc, in_spriteMap)
	{
		moveSpeed = 0.0f;
		scale = 1;
	}

	void PerformKnightCollision(Knight& knight) override
	{
		knight.IncreaseTorchPower(torchPower);
		destructionFlag = true;
	}

private:
};
class Flagon : public Monster
{
public:
	Flagon(Vec2 in_loc, Surface* in_spriteMap)
		:
		Monster(in_loc, in_spriteMap)
	{
		moveSpeed = 0.0f;
		scale = 1;
	}

	void PerformKnightCollision(Knight& knight) override
	{
		knight.IncreaseMoveSpeed();
		destructionFlag = true;
	}


private:
	
};

class TorchthrowPowerup : public LightSource
{
public:
	TorchthrowPowerup(Vec2 in_loc, Surface* in_spriteMap, MOVETYPE moveType)
		:
		LightSource(in_loc, in_spriteMap)
	{
		moveSpeed = 12.0f;
		lastDirection = moveType;
		scale = 1;
		walkingAnimation = WORLDOBJECTANIMATION::LEFTFOOT;
		walkingTimerMax = 0.2f;
		walkingTimer = 0.0f;
		isWalking = true;
		isThrownTorch = true;
	}

	void Update(World& world, Knight& knight, std::vector<Monster*> &thrownTorches, float dt) override
	{
		Vec2 previousLoc = loc;

		currentDistance += moveSpeed * dt;
		Move(lastDirection, world, dt);


		walkingTimer += dt;


		if (walkingTimer > walkingTimerMax && !(loc == previousLoc))
		{
			walkingTimer = 0.0f;
			if (walkingAnimation == WORLDOBJECTANIMATION::LEFTFOOT)
			{
				walkingAnimation = WORLDOBJECTANIMATION::RIGHTFOOT;
			}
			else if (walkingAnimation == WORLDOBJECTANIMATION::RIGHTFOOT)
			{
				walkingAnimation = WORLDOBJECTANIMATION::LEFTFOOT;
			}
		}



		if (currentDistance > maxDistance)
		{
			destructionFlag = true;
			moveSpeed = 0.0f;
		}

	}

	void PerformKnightCollision(Knight& knight) override
	{
		// Do Nothing
	}


private:

	float currentDistance = 0.0f;
	float maxDistance = 18.0f;

};

