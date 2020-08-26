/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:        project 2: Rabbit game
//  Filename:       log.cpp
//  Description:    function definitions for the log class
//  Author:         Joseph Juneau (juneau@etsu.edu)
//  Created:        Thursday, 2/1/2020
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma warning(disable : 4996)

#include "Log.h"
#include <iostream>
#include <string>
#include <time.h>
#include <fstream>
using namespace std;
using namespace os_logging;
const string Log::DEFAULT_LOG_FILE_NAME = "log.txt"; //default log file name

//default constructor
Log::Log()
{
	logFileName = DEFAULT_LOG_FILE_NAME;
}

//initialize log with c string file name
Log::Log(char* name)
{
	logFileName = name;
}
//initialize log with string file name
Log::Log(string name)
{
	logFileName = name;
}

void Log::setLogfileName(string name)
{
	logFileName = name;
}

string Log::getLogfileName() const
{
	return logFileName;
}

string Log::getDefaultLogfileName()
{
	return DEFAULT_LOG_FILE_NAME;
}

//opens log file and checks for a valid file
int Log::open()
{
	logFile.open(logFileName.c_str(), ios_base::app);
	if(logFile.fail() || !logFile.good())
	{
		cout << "Required LOG text input file " << logFileName << " does not exist" << endl;
		return 0;
	}
	return 1;
}

//close file
int Log::close()
{
	logFile.close();
	return 0;
}

//write a record with a time stamp
int Log::writeLogRecord(string message)
{
	time_t TimeStamp;
	time(&TimeStamp);
	logFile << ctime(&TimeStamp) << message << endl;
	return 0;
}
