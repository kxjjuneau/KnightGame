/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:        project 2: Rabbit game
//  Filename:       rabbit.cpp
//  Description:    function definitions for the rabbit class
//  Author:         Joseph Juneau (juneau@etsu.edu)
//  Created:        Thursday, 2/1/2020
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "Rabbit.h"
#include "Log.h"
#include <iostream>
#include <string>
#include <time.h>
#include <fstream>

using namespace std;
using namespace os_logging;
//default constructor
Rabbit::Rabbit()
{
	hp = 0;
	attackRate = 0;
	biteChance = 0;
	quickChance = 0;
	throatChance = 0;
	weakDamage = 0;
	strongDamage = 0;
	evasionChance = 0;
	initialized = false;
}
//constructor with a given filename and game log
Rabbit::Rabbit(string FileName, Log& gameLog )
{
	initialized = false; //set the rabbit to unitialized
	ifstream RabbitFile; //create file
	string LineOfRabbitFile; //holds the given line from the rabbit file
	RabbitFile.open(FileName.c_str());
	if (RabbitFile.fail() || !RabbitFile.good())
	{
		cout << "Required Rabbit text input file " << FileName << " does not exist " << endl;
		return;
	}
	//checks the file that it is correctly formated and that fill in the uninitialized variables
	getline(RabbitFile, LineOfRabbitFile);
	hp = atoi(LineOfRabbitFile.substr(LineOfRabbitFile.find(":") + 1).c_str());
	if (hp < 25 || hp > 100)
	{
		gameLog.writeLogRecord("hp is not within bounds");
		return;
	}
	
	getline(RabbitFile, LineOfRabbitFile);
	attackRate = atoi(LineOfRabbitFile.substr(LineOfRabbitFile.find(":") + 1).c_str());
	if (attackRate < 3 || attackRate > 10)
	{
		gameLog.writeLogRecord("attack rate is not within bounds");
		return;
	}

	getline(RabbitFile, LineOfRabbitFile);
	biteChance = atoi(LineOfRabbitFile.substr(LineOfRabbitFile.find(":") + 1).c_str());
	if (biteChance < 60 || biteChance > 75)
	{
		gameLog.writeLogRecord("bitechance is not within bounds");
		return;
	}

	getline(RabbitFile, LineOfRabbitFile);
	quickChance = atoi(LineOfRabbitFile.substr(LineOfRabbitFile.find(":") + 1).c_str());
	if (quickChance < 10 || quickChance > 20)
	{
		gameLog.writeLogRecord("quickchance is not within bounds");
		return;
	}

	getline(RabbitFile, LineOfRabbitFile);
	throatChance = atoi(LineOfRabbitFile.substr(LineOfRabbitFile.find(":") + 1).c_str());
	if (throatChance < 5 || throatChance > 20)
	{
		gameLog.writeLogRecord("throatchance is not within bounds");
		return;
	}
	else if ((throatChance + quickChance + biteChance) != 100)
	{
		gameLog.writeLogRecord("percentages do not add to 100");
		return;
	}

	getline(RabbitFile, LineOfRabbitFile);
	weakDamage = atoi(LineOfRabbitFile.substr(LineOfRabbitFile.find(":") + 1).c_str());
	if (weakDamage < 1 || weakDamage > 9)
	{
		gameLog.writeLogRecord("weakDamage is out of bounds");
		return;
	}

	getline(RabbitFile, LineOfRabbitFile);
	strongDamage = atoi(LineOfRabbitFile.substr(LineOfRabbitFile.find(":") + 1).c_str());
	if (strongDamage < 30 || strongDamage > 40)
	{
		gameLog.writeLogRecord("strongdamage is out of bounds");
		return;
	}

	getline(RabbitFile, LineOfRabbitFile);
	evasionChance = atoi(LineOfRabbitFile.substr(LineOfRabbitFile.find(":") + 1).c_str());
	if (evasionChance < 5 || evasionChance > 95)
	{
		gameLog.writeLogRecord("evasion chance is out of bounds");
		return;
	}
	initialized = true;
	return;
}

void Rabbit::sethp(int hpOfRabbit)
{
	hp = hpOfRabbit;
}
void Rabbit::setattackRate(int attackRateOfRabbit)
{
	attackRate = attackRateOfRabbit;
}
void Rabbit::setbiteChance(int biteChanceOfRabbit)
{
	biteChance = biteChanceOfRabbit;
}
void Rabbit::setquickChance(int quickChanceOfRabbit)
{
	quickChance = quickChanceOfRabbit;
}
void Rabbit::setThroatChance(int throatChanceOfRabbit)
{
	throatChance = throatChanceOfRabbit;
}
void Rabbit::setWeakDamage(int weakDamageOfRabbit)
{
	weakDamage = weakDamageOfRabbit;
}
void Rabbit::setStrongDamage(int strongDamageOfRabbit)
{
	strongDamage = strongDamageOfRabbit;
}
void Rabbit::setEvasionChance(int evasionOfRabbit)
{
	evasionChance = evasionOfRabbit;
}
void Rabbit::setInitialized(bool init)
{
	initialized = init;
}
int Rabbit::gethp()
{
	return hp;
}
int Rabbit::getAttackRate()
{
	return attackRate;
}
int Rabbit::getBiteChance()
{
	return biteChance;
}
int Rabbit::getQuickChance()
{
	return quickChance;
}
int Rabbit::getThroatChance()
{
	return throatChance;
}
int Rabbit::getWeakDamage()
{
	return weakDamage;
}
int Rabbit::getStrongDamage()
{
	return strongDamage;
}
int Rabbit::getEvasionChance()
{
	return evasionChance;
}
bool Rabbit::getInitialized()
{
	return initialized;
}
