/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:        project 2: Rabbit game
//  Filename:       rabbit.h
//  Description:    definition for the rabbit class
//  Author:         Joseph Juneau (juneau@etsu.edu)
//  Created:        Thursday, 2/1/2020
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "Log.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include <time.h>
#include <fstream>

using namespace std;
using namespace os_logging;
class Rabbit {
private:

	int hp;
	int attackRate;
	int biteChance;
	int quickChance;
	int throatChance;
	int weakDamage;
	int strongDamage;
	int evasionChance;
	bool initialized;
public:
	Rabbit();
	Rabbit(string FileName,Log& gameLog);
	void sethp(int hpOfRabbit);
	void setattackRate(int attackRateOfRabbit);
	void setbiteChance(int biteChanceOfRabbit);
	void setquickChance(int quickChanceOfRabbit);
	void setThroatChance(int throatChanceOfRabbit);
	void setWeakDamage(int weakDamageOfRabbit);
	void setStrongDamage(int strongDamageOfRabbit);
	void setEvasionChance(int evasionOfRabbit);
	void setInitialized(bool init);
	int gethp();
	int getAttackRate();
	int getBiteChance();
	int getQuickChance();
	int getThroatChance();
	int getWeakDamage();
	int getStrongDamage();
	int getEvasionChance();
	bool getInitialized();
	



};