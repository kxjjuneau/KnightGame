/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:        project 2: Rabbit game
//  Filename:       knigh.h
//  Description:    definition for the knight class
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
class Knight {
private:
	string Name;
	int hp;
	int bravery;
	int attackRate;
	int damagePerAttack;
	static const string DEFAULT_KNIGHT_FILE_NAME;

public:
	Knight();
	void setName(string Name);
	void setHp(int healthPoints);
	void setBravery(int BraverOfKnight);
	void setAttackRate(int attackRateOfKnight);
	void setDamagePerAttack(int damageOfKnight);
	string getName();
	int gethp();
	int getBravery();
	int getAttackRate();
	int getDamagePerAttack();
	static Knight* proccessKnightFile(ifstream& KnightFile, int countOfKnights, Log& gameLog);
	static string getDefaultKnightFileName();

};