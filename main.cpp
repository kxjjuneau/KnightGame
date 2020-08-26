/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:        project 6: Rabbit game
//  Filename:       main.cpp
//  Description:    accepts 3 parameters, rabbit (required), knight (optional), log (optional
//  Author:         Joseph Juneau (juneau@etsu.edu)
//  Created:        Thursday, 4/19/2020
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <string>
#include <time.h>
#include <fstream>
#include <unistd.h>
#include "Log.h"
#include "Knight.h"
#include "Rabbit.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <cstring>
#include <pthread.h>
#include <semaphore.h>
#include "safequeue.h"
#include "safequeue.cpp"
#include <signal.h>
#include <queue>

 
using namespace std;
using namespace os_logging;

void runGame(Rabbit rabbit, Knight* knights, int remainingKnights); //runs the game simulation
int modifyDamage(int damageToTarget);//damage modifier function
void ProccessParams(int argc, char** argv, string& logFileName, string& KnightFileName, string& rabbitFileName); 
void *loggerProcess(void* arg); //runs the log child as thread 
void loggerProcess(string logfilename); //runs the logger as proccess
//void KnightProcess(Knight knight, int knightid); //runs the knight child processes
void *KnightProcess(void* arg); //runs the knights as threads
//void RabbitProcess(Rabbit rabbit); //runs the rabbit proccess
void *RabbitProcess(void* arg); //runs the rabbit as threads
int chooseTarget(int remainingknights[], int totalknights); // function to randomly select from remaining knights

//GLOBALS
 //global log file
int value, parendPid, logPid, rabbitPid, enginePid; //proccess ids for for processess NOT IN USE
int logPipe[2];
string Names[10]; //gloabl variable to hold knights names
string logFileName = "log.txt";
Log gameLog;
int groupID; //group id of all processes
bool KNIGHTS_INIT_FAIL = false;
bool RABBIT_INIT_FAIL = false;
struct rabbitStructProcess {
	Rabbit r;
	int countOfKnights;
};
struct knightStructProcess {
	Knight k;
	int id;
};
struct message {
	int from;
	int type;
	int damage;
	int target;
	bool knightsWon = false;
	bool KnightAlive = true;
	bool knightPresent = true;
	bool rabbitWon = false;
};
pthread_mutex_t rabbitlock;
pthread_mutex_t knightlock[10];
safequeue<message> rabbitSafeQueue;
safequeue<message> knightSafeQueue[10];

int main(int argc, char** argv)
{
	Rabbit rabbit; //rabbit class that keeps track of the rabbit during the game
	string temp; // string to hold constructed messages for the log
	Knight* knights; //pointer to knight class to initialize unkown amount of knights
	int countOfKnights; //amount of knights to initialize
	string rabbitFileName;
	string KnightFileName;
	ifstream KnightFile;
	ifstream RabbitFile;
	message m;
	srand(time(NULL));
	groupID = setpgid(0, 0);
	

	ProccessParams(argc, argv, logFileName, KnightFileName, rabbitFileName); // process command line args
	//open pipes
	if (pipe(logPipe) == -1)
	{
		fprintf(stderr, "Pipe Failed");
		return 1;
	}

		KnightFile.open(KnightFileName.c_str()); //open knight file
		if (KnightFile.fail() || !KnightFile.good()) //if it failes to open
		{
			KNIGHTS_INIT_FAIL = true;
			gameLog.writeLogRecord("Knights failed to initialize");
		}
		else
		{
			//get first line from knight file to initialize amount of knights
			getline(KnightFile, temp);
			countOfKnights = atoi(temp.substr(temp.find(":") + 1).c_str());

			//make sure knight count is between 1 and 8
			if (countOfKnights < 1 || countOfKnights > 8)
			{
				gameLog.writeLogRecord("amount of knights is out of bounds");
			}
			else
			{
				//process the knight file
				knights = knights->proccessKnightFile(KnightFile, countOfKnights, gameLog);
				//if it fails to process
				if (knights == NULL)
				{
					KNIGHTS_INIT_FAIL = true;
					gameLog.writeLogRecord("KNIGHTS FAILED TO INITIALIZE");
				}
				else
				{

					//process rabbit file
					rabbit = Rabbit(rabbitFileName, gameLog);
					//if the rabbit file processed correctly
					if (rabbit.getInitialized())
					{
						runGame(rabbit, knights, countOfKnights);
					}
					else
					{
						RABBIT_INIT_FAIL = true;
						gameLog.writeLogRecord("RABBIT FAILED TO INITIALIZE");
					}

				}


			}
		}
	
	temp = "cat " + logFileName;
	system(temp.c_str());
	cout << "enter in anything to quit: ";
	cin >> temp;
	return 0;
}

//runs the individual knights thread, it first initializes, waits for attack from rabbit then attacks if it istd killed or ran away
//void KnightProcess(Knight knight, int knightid)
void *KnightProcess(void* arg)
{
	knightStructProcess *k = (knightStructProcess*)arg; //passed arg to thread
	Knight knight = (*k).k;
	int knightID = (*k).id; //index number of knight to keep them consistent
	Names[knightID] = knight.getName(); //fill global variable with the names, probably should make this atomic
	message m;
	int done = 0;

	while (done == 0)
	{
		m = knightSafeQueue[knightID].dequeue();
		if (m.from == -1) // if the "GM" told it to kill self
			pthread_exit(0);
		if (m.knightsWon == true) //if rabbit told the knight its dead
			pthread_exit(0);
		knight.setHp((knight.gethp() - m.damage)); //else decrement health cause it got hit

		if (knight.gethp() <= 0) //if knight dies
		{
			m.from = knightID;	//send message to rabbit notifying it
			m.KnightAlive = false;
			rabbitSafeQueue.enqueue(m);
			m.from = knightID + 1;
			write(logPipe[1], (char*)&m, sizeof(message)); // send death to log
			pthread_exit(0); //exit
			done = 1; //not useful, just paranoid
		}
		else if (knight.gethp() < knight.getBravery())
		{
			m.knightPresent = false; //if the knight ran away
			m.from = knightID;
			rabbitSafeQueue.enqueue(m);
			m.from = knightID + 1; //needs to increment by one so that the logger always sees a knights message then decrements it to get NAME
			write(logPipe[1], (char*)&m, sizeof(message)); //log the knight running away event
			pthread_exit(0);
			done = 1;
		}
		else
		{
			m.target++; //increment for logger purposes
			m.from = knightID;
			m.damage = modifyDamage(knight.getDamagePerAttack());
			rabbitSafeQueue.enqueue(m); //push attack message to rabbit
		}

	}
	pthread_exit(0);
	return 0;
}

//runs the rabbit process. first initialize then attack knights then read all messages passed from the knights
//void RabbitProcess(Rabbit rabbit, int countOfKnights)
void *RabbitProcess(void* arg)
{
	message m;
	rabbitStructProcess *rabbitarg = (rabbitStructProcess*)arg;
	
	Rabbit rabbit;
	rabbit = (*rabbitarg).r;
	int countOfKnights = (*rabbitarg).countOfKnights;
	int BiteChance = rabbit.getBiteChance();//theses are added to each other in order to use random number to get the proper attack
	int QuickChance = rabbit.getQuickChance() + BiteChance; //ex: bitechance = 50%, quickchance = 50% + 10%...
	int ThroatChance = rabbit.getThroatChance() + QuickChance; 
	int weakDamage = rabbit.getWeakDamage();
	int strongDamage = rabbit.getStrongDamage();
	int Chance;
	int remainingKnights[countOfKnights]; //index and chance of hitting the remaining knights
	int target = 0;
	int done = 0;
	for (int i = 0; i < countOfKnights; i++) //initialize all indexes to enable hitting them
		remainingKnights[i] = 1;

	while (done == 0)
	{
	
		

		Chance = rand() % 100 + 1; // get rand num between 1-100
		//ATTACK
		m.from = 0;
		m.rabbitWon = true;
		for (int k = 0; k < countOfKnights; k++) //see if all knights are dead/gone
		{
			if (remainingKnights[k] > 0)
				m.rabbitWon = false;
			else
				continue;
		}
		if (m.rabbitWon == true)//they won
		{
			write(logPipe[1], (char*)&m, sizeof(message));
			pthread_exit(0);
		}
		else
		{

			if (Chance <= BiteChance) //individual attack
			{
				m.type = 0;
				m.target = chooseTarget(remainingKnights, countOfKnights); 
				
				m.damage = modifyDamage(weakDamage);
				write(logPipe[1], (char*)&m, sizeof(message)); //send to logger attack on knight
				knightSafeQueue[m.target].enqueue(m); //tell knight that they got smacked

			}
			else if (Chance <= QuickChance) //all knights get attacked
			{

				m.type = 1;
				m.damage = modifyDamage(weakDamage);
				m.from = 0;
				write(logPipe[1], (char*)&m, sizeof(message)); //tell logger rabbit attacked all knights
				for (int i = 0; i < countOfKnights; i++)
				{

					if (remainingKnights[i] > 0) //only attack remaining knights
					{
						knightSafeQueue[i].enqueue(m); //enables them to attack after quick attack without messaging the logger also
					}
					else
						continue;

				}
			}
			else //strong attack wrong knight attacks
			{
				m.target = chooseTarget(remainingKnights, countOfKnights);
				m.type = 2;
				m.damage = modifyDamage(strongDamage);
				write(logPipe[1], (char*)&m, sizeof(message));
				knightSafeQueue[m.target].enqueue(m);

			}



		}
		
		usleep(rabbit.getAttackRate()*500); //sleep to give knights a chance to retaliate and fill up the queue with their reactions
		while(rabbitSafeQueue.empty() == 0) //proccess all knight reactions
		{
			m = rabbitSafeQueue.dequeue();

			if (m.rabbitWon == true) // check if rabbit has defeated all knights
			{
				write(logPipe[1], (char*)&m, sizeof(message));
				pthread_exit(0);
			}
			else if (!m.KnightAlive || !m.knightPresent) //if rabbit has received message indicating a knight is gone
			{
				remainingKnights[m.from] = -1; //set the fleeing knights id to -1 to signal he is no longer available

				m.rabbitWon == true;
				for (int i = 0; i < countOfKnights; i++) //recheck if any knights are left
				{
					if (remainingKnights[i] > 0)
						m.rabbitWon == false;

				}
				if (m.rabbitWon == true)
				{
					write(logPipe[1], (char*)&m, sizeof(message));
					pthread_exit(0);
				}

			}

			else //the rabbit is getting hit
			{
				Chance = rand() % 100 + 1;
				if (Chance > rabbit.getEvasionChance())
					m.damage = 0;

				rabbit.sethp(rabbit.gethp() - m.damage);
				if (rabbit.gethp() < 0)
				{
					done = 1;
					m.knightsWon = true;
					for (int i = 0; i < countOfKnights; i++)
					{
						knightSafeQueue[i].enqueue(m);
						
					}
					write(logPipe[1], (char*)&m, sizeof(message));
					pthread_exit(0);
				}
				m.from++;
				write(logPipe[1], (char*)&m, sizeof(message));
			}
		}

	}
	pthread_exit(0);

	return 0;
}

//runs the proccess for bookkeeping
void loggerProcess(string logFileName)
{
	message m;
	int lock = 1;
	int i = 0;
	string temp;
	//close(logPipe[1]); //close writing end
	gameLog.setLogfileName(logFileName);//open log file using either passed argument or default
	gameLog.close(); //fixed a bug where it wasnt opening the text
	gameLog.open();
	int done = 0;

	if ((read(logPipe[0], (char*)&m, sizeof(message))) != 0) //first message needs to log start of game
	{
		if ((m.from == -1))
		{
			//cout << "START GAME" << endl;
			gameLog.writeLogRecord("START GAME");
		}
		else if (m.from == -2)
		{
			gameLog.writeLogRecord("knights failed to initialize");
			return;
		}
		else if (m.from == -3)
		{
			gameLog.writeLogRecord("rabbit failed to initialize");
			return;
		}
		
			
	}

	//run loop to log game events
	while (done == 0)
	{
		if ((read(logPipe[0], (char*)&m, sizeof(message))) != 0)
		{
			if (m.from == -1)// game is over 
			{
				gameLog.writeLogRecord("END");
			}
			else if (m.from == 0)//rabbit message
			{
				if (m.rabbitWon == true)
				{
					//cout << "RABBIT WON" << endl;
					gameLog.writeLogRecord("RABBIT WON");
					done = 1;
				}
				else if (m.type == 1)
				{
					//cout << "Rabbit hit all knights for " << m.damage << endl;
					temp = "RABBIT hit all knights for " + m.damage;
					gameLog.writeLogRecord(temp);
				}
				else
				{
					//cout << "Rabbit hit " << Names[m.target] << " for " << m.damage << " damage" << endl;
					temp = "Rabbit hit " + Names[m.target] + " for " + to_string( m.damage) + " damage";
					gameLog.writeLogRecord(temp);
				}
			}
			else if (m.from > 0)//knight message
			{
				if (m.knightsWon == true)
				{
					//cout << "Knights WON" << endl;
					temp = "KNIGHTS WON";
					gameLog.writeLogRecord(temp);
				}
				if (m.KnightAlive == false)
				{
					//cout << Names[m.from - 1] << " Is DEAD" << endl;
					temp = Names[m.from - 1] + " IS DEAD";
					gameLog.writeLogRecord(temp);
				}
				else if (m.knightPresent == false)
				{
					//cout << Names[m.from - 1] << " ran away" << endl;
					temp = Names[m.from - 1] + " RAN AWAY";
					gameLog.writeLogRecord(temp);
				}
				else
				{
					//cout << Names[m.from - 1] << " hits the rabbit for " << m.damage << " damage" << endl;
					temp = Names[m.from - 1] + " hits the rabbit for " + to_string(m.damage) + " damage";
					gameLog.writeLogRecord(temp);
				}
			}
		}

	}
	gameLog.close();
	return;
}


//proccesses the parameters and spits out error messages if you messed up
void ProccessParams(int argc, char** argv, string& logFileName, string& KnightFileName, string& rabbitFileName)
{
	Knight* knights;
	switch (argc) //switch based on the number of parameters
	{
	case 3: //if only one comand line argument is passed check to make sure that it is the rabbit file
		if (string(argv[1]) == "-r" || string(argv[1]) == "-R")
		{
			rabbitFileName = argv[2];
			KnightFileName = knights->getDefaultKnightFileName();
			logFileName = gameLog.getDefaultLogfileName();
		}
		else
			cout << "fix your parameters";
		break;
	case 5: //if 2 arguments are passed
		//if the first arg is rabbit file
		if (string(argv[1]) == "-r" || string(argv[1]) == "-R")
		{
			//if second arg is knight file
			if (string(argv[3]) == "-k" || string(argv[1]) == "-K")
			{
				rabbitFileName = argv[2];
				KnightFileName = argv[4];
				logFileName = gameLog.getDefaultLogfileName();
			}
			//if second arg is log file
			else if (string(argv[3]) == "-l" || string(argv[3]) == "-L")
			{
				rabbitFileName = argv[2];
				KnightFileName = knights->getDefaultKnightFileName();
				logFileName = argv[4];
			}
			//else wrong parameters have been passed
			else
				cout << "fix your parameters";
		}
		//if rabbit is second parameter is passed
		else if (string(argv[3]) == "-r" || string(argv[3]) == "-R")
		{
			if (string(argv[1]) == "-k" || string(argv[1]) == "-K")
			{
				rabbitFileName = argv[4];
				KnightFileName = argv[2];
				logFileName = gameLog.getDefaultLogfileName();
			}
			else if (string(argv[1]) == "-l" || string(argv[1]) == "-L")
			{
				rabbitFileName = argv[4];
				KnightFileName = knights->getDefaultKnightFileName();
				logFileName = argv[2];
			}
			else
				cout << "fix your parameters";
		}
		else
			cout << "fix your parameters";
		break;
	case 7: //if rabbit is third arg passed
		if (string(argv[1]) == "-r" || string(argv[1]) == "-R")
		{
			if (string(argv[3]) == "-k" || string(argv[3]) == "-K")
			{
				if (string(argv[5]) == "-l" || string(argv[5]) == "-L")
				{
					rabbitFileName = argv[2];
					KnightFileName = argv[4];
					logFileName = argv[6];
				}
				else
					break;//quit incorrect params
			}
			else if (string(argv[3]) == "-l" || string(argv[3]) == "-L")
			{
				if (string(argv[5]) == "-k" || string(argv[5]) == "-K")
				{
					rabbitFileName = argv[2];
					KnightFileName = argv[6];
					logFileName = argv[4];
				}
				else
					break;//quit incorrect params
			}
		}
		else if (string(argv[3]) == "-r" || string(argv[3]) == "-R")
		{
			if (string(argv[1]) == "-k" || string(argv[1]) == "-K")
			{
				if (string(argv[5]) == "-l" || string(argv[5]) == "-L")
				{
					rabbitFileName = argv[4];
					KnightFileName = argv[2];
					logFileName = argv[6];
				}
				else
					cout << "fix your parameters";
				break;//quit incorrect params
			}
			else if (string(argv[1]) == "-l" || string(argv[1]) == "-L")
			{
				if (string(argv[5]) == "-k" || string(argv[5]) == "-K")
				{
					rabbitFileName = argv[4];
					KnightFileName = argv[6];
					logFileName = argv[2];
				}
				else
					cout << "fix your parameters";
				break;//quit incorrect params
			}
			else
				cout << "fix your parameters";
			break;
		}
		else if (string(argv[5]) == "-r" || string(argv[5]) == "-R")
		{
			if (string(argv[3]) == "-k" || string(argv[3]) == "-K")
			{
				if (string(argv[1]) == "-l" || string(argv[1]) == "-L")
				{
					rabbitFileName = argv[6];
					KnightFileName = argv[4];
					logFileName = argv[2];
				}
				else
					cout << "fix your parameters";
				break;//quit incorrect params
			}
			else if (string(argv[3]) == "-l" || string(argv[3]) == "-L")
			{
				if (string(argv[1]) == "-k" || string(argv[1]) == "-K")
				{
					rabbitFileName = argv[6];
					KnightFileName = argv[2];
					logFileName = argv[4];
				}
				else
					cout << "fix your parameters";
				break;//quit incorrect params
			}
			else
				cout << "fix your parameters";
		}
		else
			cout << "fix your parameters";
		break;
	default:
		cout << "fix your parameters";
		return;

	}
}


//spins up each neccesary procces/threads and kills them all on completion.
void runGame(Rabbit rabbit, Knight* knights, int countOfKnights)
{
	message m;
	int turn = 1;
	int Chance;
	int value;
	int remainingKnights = countOfKnights;
	knightStructProcess k[countOfKnights];



	//pthread_t logthread;
	//pthread_create(&logthread, NULL, loggerProcess, &logFileName);
	pthread_t rabbitthread;
	pthread_t knightthreads[countOfKnights];
	rabbitStructProcess r;
	r.r = rabbit;
	r.countOfKnights = countOfKnights;
	//first start knight threads since they wait for rabbit anyways
	for (int i = 0; i < countOfKnights; i++)
	{
		k[i].k = knights[i];
		k[i].id = i; 
		pthread_create(&knightthreads[i], NULL, KnightProcess, (void *)&k[i]);
	}
	//make sure NAMES has finished initializing to start the logger with the global variable filled because proccesses dont share memory
	while (Names[countOfKnights - 1] == "")
	{
		continue;
	}
	value = fork();
	if (value < 0)
	{
		//error
		return;
	}
	else if (value == 0)
	{
		setpgid(0, 0);
		logPid = getpid();
		loggerProcess(logFileName);
		exit(0);
	}
	else
	{
		if (KNIGHTS_INIT_FAIL == true)
		{
			m.from = -2;
			write(logPipe[1], (char*)&m, sizeof(message));
		}
		else if (RABBIT_INIT_FAIL == true)
		{
			m.from = -3;
			write(logPipe[1], (char*)&m, sizeof(message));
		}
		else
		{
			m.from = -1;
			write(logPipe[1], (char*)&m, sizeof(message));
			pthread_create(&rabbitthread, NULL, RabbitProcess, (void *)&r);

			pthread_join(rabbitthread, NULL);
			m.from = -1; //end logger
			write(logPipe[1], (char*)&m, sizeof(message));
		}
		//kill(-groupID, SIGTERM); //KILL IT if its still up, again paranoid
		sleep(2);
		//kill(-groupID, SIGKILL);
		m.from = -1;
		for (int i = 0; i < countOfKnights; i++) //kill any remaining knight threads, paranoid
		{
			knightSafeQueue[i].enqueue(m);
		}
	}
	
	return;
}

//function to choose nessesary target
int chooseTarget(int remainingknights[], int totalknights)
{
	int Chance;
	int target = 0;
	for (int i = 0; i < totalknights; i++)
	{
		if (remainingknights[i] > 0)
		{
			remainingknights[i] = rand() % 100 + 1;
			if (i > 0 && remainingknights[i] > remainingknights[i - 1])
				target = i;

		}
	}
	return target;
}

int modifyDamage(int damageToTarget)
{
	int randNum = rand() % 7 + 1;
	switch (randNum)
	{
	case 1:
		if (damageToTarget >= 3)
			return damageToTarget - 3;
	case 2:
		if (damageToTarget >= 2)
			return damageToTarget - 2;
	case 3:
		if (damageToTarget >= 1)
			return damageToTarget - 1;
	case 4:
		return damageToTarget;
	case 5:
		return damageToTarget + 1;
	case 6:
		return damageToTarget + 2;
	case 7:
		return damageToTarget + 3;
	default:
		return damageToTarget;
	}
}

//runs the proccess for bookkeeping
//NOT USED in this project, JUST FOR FUN
void *loggerProcess(void* arg)
{
	string logFileName = *static_cast<string*>(arg);
	message m;
	int lock = 1;
	int i = 0;
	string temp;
	//close(logPipe[1]); //close writing end
	gameLog.setLogfileName(logFileName);//open log file using either passed argument or default
	gameLog.close(); //fixed a bug where it wasnt opening the text
	gameLog.open();
	int done = 0;
	//error on initialization logs
	while (done == 0)
	{
		//m.from = read(logPipe[0], (char*)&m, sizeof(message));
		if ((read(logPipe[0], (char*)&m, sizeof(message))) != 0)
		{
			if (m.from == -1)// game is over 
			{

			}
			else if (m.from == 0)//rabbit message
			{
				if (m.rabbitWon == true)
				{
					cout << "RABBIT WON" << endl;
					done = 1;
				}
				else if (m.type == 1)
				{
					cout << "Rabbit hit all knights for " << m.damage << endl;
				}
				else
				{
					cout << "Rabbit hit " << Names[m.target] << " for " << m.damage << " damage" << endl;
				}
			}
			else if (m.from > 0)//knight message
			{
				if (m.knightsWon == true)
				{
					cout << "Knights WON" << endl;
				}
				if (m.KnightAlive == false)
				{
					cout << Names[m.from - 1] << " Is DEAD" << endl;
				}
				else if (m.knightPresent == false)
				{
					cout << Names[m.from - 1] << " ran away" << endl;
				}
				else
				{
					cout << Names[m.from - 1] << " hits the rabbit for " << m.damage << " damage" << endl;
				}
			}
		}

	}
	gameLog.close();
	return 0;
}
