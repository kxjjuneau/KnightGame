/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:        project 2: Rabbit game
//  Filename:       knight.cpp
//  Description:    function definitions for the knight class
//  Author:         Joseph Juneau (juneau@etsu.edu)
//  Created:        Thursday, 2/1/2020
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "Knight.h"
#include "Log.h"
#include <iostream>
#include <string>
#include <time.h>
#include <fstream>

using namespace std;
using namespace os_logging;
const string Knight::DEFAULT_KNIGHT_FILE_NAME = "knight.txt"; //default knight file name
//default constructor
Knight::Knight()
{
	Name = "";
	hp = 0;
	bravery = 0;
	attackRate = 0;
	damagePerAttack = 0;
}
void Knight::setName(string nameOfKnight)
{
	Name = nameOfKnight;
}
void Knight::setHp(int healthPoints)
{
	hp = healthPoints;
}
void Knight::setBravery(int BraverOfKnight)
{
	bravery = BraverOfKnight;
}
void Knight::setAttackRate(int attackRateOfKnight)
{
	attackRate = attackRateOfKnight;
}
void Knight::setDamagePerAttack(int damageOfKnight)
{
	damagePerAttack = damageOfKnight;
}
string Knight::getName()
{
	return Name;
}
int Knight::gethp()
{
	return hp;
}
int Knight::getBravery()
{
	return bravery;
}
int Knight::getAttackRate()
{
	return attackRate;
}
int Knight::getDamagePerAttack()
{
	return damagePerAttack;
}

//fuction to proccess knight file for a knight pointer
Knight* Knight::proccessKnightFile(ifstream& KnightFile, int countOfKnights, Log& gameLog)
{
	string LineOfKnightFile;
	
	Knight* knights = new Knight[countOfKnights];

	//loops for the amount of knights specified
	for(int i = 0; i < countOfKnights; i++)
	{
		getline(KnightFile, LineOfKnightFile);
		if(LineOfKnightFile[0] = '\n') //if the gotten line is empty, skip it
			getline(KnightFile, LineOfKnightFile);
		//get integer from string and check that it fits in the bounds, if it fails, reutrn null
		knights[i].Name = LineOfKnightFile.substr(LineOfKnightFile.find(":") + 1).c_str();
		if (knights[i].Name.length() < 2 || knights[i].Name.length() > 50)
		{
			gameLog.writeLogRecord("name is out of bounds");
			return NULL;
		}
		getline(KnightFile, LineOfKnightFile);
		knights[i].hp = atoi(LineOfKnightFile.substr(LineOfKnightFile.find(":") + 1).c_str());
		if (knights[i].hp < 10 || knights[i].hp > 40)
		{
			gameLog.writeLogRecord("knight hp is out of bounds");
			return NULL;
		}

		getline(KnightFile, LineOfKnightFile);
		knights[i].bravery = atoi(LineOfKnightFile.substr(LineOfKnightFile.find(":") + 1).c_str());
		if (knights[i].bravery < 0 || knights[i].bravery > 39)
		{
			gameLog.writeLogRecord("knights bravery is out of bounds");
			return NULL;
		}
		else if (knights[i].bravery >= knights[i].hp)
		{
			gameLog.writeLogRecord("knights initial bravery can not be equal or larger than hp");
			return NULL;
		}

		getline(KnightFile, LineOfKnightFile);
		knights[i].attackRate = atoi(LineOfKnightFile.substr(LineOfKnightFile.find(":") + 1).c_str());
		if (knights[i].attackRate < 10 || knights[i].attackRate > 50)
		{
			gameLog.writeLogRecord("knights attackrate is out of bounds");
			return NULL;
		}

		getline(KnightFile, LineOfKnightFile);
		knights[i].damagePerAttack = atoi(LineOfKnightFile.substr(LineOfKnightFile.find(":") + 1).c_str());
		if (knights[i].damagePerAttack < 1 || knights[i].damagePerAttack > 8)
		{
			gameLog.writeLogRecord("knights damage is out of bounds");
			return NULL;
		}
	}
	return knights;

}

string Knight::getDefaultKnightFileName()
{
	return DEFAULT_KNIGHT_FILE_NAME;
}