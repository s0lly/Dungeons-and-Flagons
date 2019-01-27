/******************************************************************************************
*	Game code and amendments by s0lly													   *
*	https://www.youtube.com/channel/UCOyCXEB8NTx3Xjm8UptwsIg							   *
*	https://s0lly.itch.io/																   *
*	https://www.instagram.com/s0lly.gaming/												   *
******************************************************************************************/

#pragma once
#include "WorldObject.h"
#include "Camera.h"
#include "MiniMap.h"
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

class GameManager
{
public:

	void SaveGame(std::vector<Monster*> monsterPtrs, Knight &knight, MiniMap& miniMap)
	{
		std::ofstream saveFile;
		saveFile.open("savedGame.txt");

		for (auto k : monsterPtrs)
		{

			if (typeid(*k) == typeid(Ghost))
			{
				saveFile << "Ghost";
			}
			if (typeid(*k) == typeid(Torch))
			{
				saveFile << "Torch";
			}
			if (typeid(*k) == typeid(Flagon))
			{
				saveFile << "Flagon";
			}
			if (typeid(*k) == typeid(TorchthrowPowerup))
			{
				saveFile << "TorchthrowPowerup";
			}
			if (typeid(*k) == typeid(Skeleton))
			{
				saveFile << "Skeleton";
			}

			saveFile << "," << k->GetLoc().x << "," << k->GetLoc().y << "," << k->GetLastDirection() << "," << k->GetDestructionFlag();

			saveFile << "\n";

		}

		saveFile << "Knight" << "," << knight.GetLoc().x << "," << knight.GetLoc().y << "," << knight.GetLastDirection() << "," << knight.GetTorchPower() << "," << knight.GetSpeed();
		saveFile << "\n";

		// Add in MiniMap info

		saveFile << "MiniMap";
		for (int i = 0; i < miniMap.GetRevealedLocs().size(); i++)
		{
			saveFile << "," << (int)miniMap.GetRevealedLocs()[i];
		}
		saveFile << "\n";

		saveFile.close();

	}

	void LoadGame(std::vector<Monster*> &monsterPtrs, Knight &knight, Camera &camera, std::vector<Surface*> spritePtrs, MiniMap& miniMap)
	{
		for (int i = 0; i < monsterPtrs.size(); i++)
		{
			delete monsterPtrs[i];
		}

		monsterPtrs.clear();

		
		std::ifstream saveFile;
		saveFile.open("savedGame.txt");
		std::vector<std::vector<std::string>> tempStringMap;
		std::vector<std::string> tempStrings;
		
		std::stringstream tempStringStream;

		int numRows = 0;
		
		char temp;
		
		if (saveFile.is_open())
		{
			while (saveFile.get(temp)) // !saveFile.eof()
			{
				//saveFile >> temp;
				if (temp != ',' && temp != '\n')
				{
					tempStringStream << temp;
				}
		
				if (temp == ',')
				{
					tempStrings.push_back(tempStringStream.str());
					tempStringStream.str("");
				}
		
				if (temp == '\n')
				{
					numRows++;
					tempStrings.push_back(tempStringStream.str());
					tempStringMap.push_back(tempStrings);
					tempStringStream.str("");
					tempStrings.clear();
					
				}
			}
		}
		
		//charMap.pop_back();
		
		saveFile.close();
		

		std::ofstream saveFileCheck;
		saveFileCheck.open("savedGameCheck.txt");

		saveFileCheck << tempStringMap.size();
		saveFileCheck << "\n";

		for (auto k : tempStringMap)
		{
			if (k[0] == "Ghost")
			{
				saveFileCheck << "Ghost" << "," << k[1] << "," << k[2] << "," << k[3] << "," << k[4];

				saveFileCheck << "\n";
			}
		}

		saveFileCheck.close();

		std::vector<bool> locRevealed;
		
		for (auto k : tempStringMap)
		{

			if (k[0] == "Ghost")
			{
				monsterPtrs.push_back(new Ghost({ std::stof(k[1]), std::stof(k[2]) }, spritePtrs[1], (MOVETYPE)std::stoi(k[3])));
				monsterPtrs[monsterPtrs.size() - 1]->SetDestructionFlag((bool)std::stoi(k[4]));
			}
			if (k[0] == "Torch")
			{
				monsterPtrs.push_back(new Torch({ std::stof(k[1]), std::stof(k[2]) }, spritePtrs[4]));
				monsterPtrs[monsterPtrs.size() - 1]->SetDestructionFlag((bool)std::stoi(k[4]));
			}
			if (k[0] == "Flagon")
			{
				monsterPtrs.push_back(new Flagon({ std::stof(k[1]), std::stof(k[2]) }, spritePtrs[3]));
				monsterPtrs[monsterPtrs.size() - 1]->SetDestructionFlag((bool)std::stoi(k[4]));
			}
			if (k[0] == "TorchthrowPowerup")
			{
				monsterPtrs.push_back(new TorchthrowPowerup({ std::stof(k[1]), std::stof(k[2]) }, spritePtrs[5], (MOVETYPE)std::stoi(k[3])));
				monsterPtrs[monsterPtrs.size() - 1]->SetDestructionFlag((bool)std::stoi(k[4]));
			}
			if (k[0] == "Skeleton")
			{
				monsterPtrs.push_back(new Skeleton({ std::stof(k[1]), std::stof(k[2]) }, spritePtrs[2], (MOVETYPE)std::stoi(k[3])));
				monsterPtrs[monsterPtrs.size() - 1]->SetDestructionFlag((bool)std::stoi(k[4]));
			}

			if (k[0] == "Knight")
			{
				knight.Reinit({ std::stof(k[1]), std::stof(k[2]) }, (MOVETYPE)std::stoi(k[3]), (float)std::stoi(k[4]), (float)std::stoi(k[5]));
			}


			if (k[0] == "MiniMap")
			{
				for (int i = 1; i < k.size(); i++)
				{
					locRevealed.push_back((bool)std::stoi(k[i]));
				}
			}

		}

		camera.Init(knight.GetLoc());

		miniMap.Init(locRevealed);

		tempStringMap.clear();
	}


private:


};