//
// class Log
//
// This class encapsulates the log file functionality
// 
// DO NOT MODIFY THIS HEADER IN YOUR PROJECT:
// Grading will be done with this interface
//
// @author Martin L. Barrett
// @author Matthew S. Harrison: harrisonms1@etsu.edu
//

// Multiple-include protection
#ifndef LOG_H
#define LOG_H
#include <stdio.h>
#include <iostream>
#include <string>
#include <time.h>
#include <fstream>

using namespace std;

// Google C++ standard: do not indent namespaces
namespace os_logging {

//
// class Log:
//    Opens a text log file, timestamp at beginning,
//       writes string entries, timestamp at end
//
class Log {
	
private:
    // Some constants
    static const int MAX_LOG_STRING = 1024;				// Biggest log string
    static const string DEFAULT_LOG_FILE_NAME;			// Default log name

	// TODO: Put the next line in log.cc/cpp (your implementation file):
	// const string Log::DEFAULT_LOG_FILE_NAME = "log";

	// Success and failure of operations
	static const int SUCCESS = 1;
    static const int FAILURE = 0;

    string logFileName;     // Log file name
    fstream logFile;        // Log file variable
	
    string getTimeStamp(); // Get the timestamp value
    
public:
    // Constructors
    // Default constructor: set the default file name
    Log();

    // Overloaded char* constructor
    Log(char* name);

    // Overloaded string class constructor
    Log(string name);

    // Setters
    // set the log file name
    void setLogfileName(string name);

    // Getters
    // Get the log file name
    string getLogfileName() const;

    // Get the default log file name
    static string getDefaultLogfileName();

    // Log functions
    // Return SUCCESS or FAILURE
    //
    // open the log with timestamp
    int open();

    // close the log with timestamp
    int close();

    // Write a string to the log
    int writeLogRecord(string message);

}; // class Log

}  // end namespace os_logging

#endif 

