#ifndef LogManager_H
#define LogManager_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <cassert>
#include <iostream>

//#define windows
#define textMode

class LogManager{
	static std::ofstream* currStream;
	static void flushLogFileOStream();

public:
	enum Level
	{
	    Status = 0,
		Error,
	    Critical,
		ExactStatus, //Do not add any decorations to the message, print it exactly
		ExactError, //Do not add any decorations to the message, print it exactly
		ExactCritical
	};

	/**
	 * This function returns an output stream according to the className that has been input.
	 * If LogManager::isLogDisabled(className) returns false, a null stream is returned.
	 * Else the correct stream for the log file is returned.
	 *
	 * \param[in] className: The class and function for which log stream is needed.
	 */
	static std::ofstream& getLogFileOStream(int logLevel, std::string className);
	/**
	 * Close the log file stream.
	 */
	static void closeLogFileOStream();

	/**
	 * Making use of variadic functions in C to write printf statement contents to the log file
	 * http://www.eskimo.com/~scs/C-faq/q15.5.html
	 * \param[in] className Name of the class that invoked this function. This value is used to determine if log information for that class is to be printed or not.
	 * \param[in] logLevel Level of the log. One of the values in LogManager::Level
	 * \param[in] printfStart First variable of the printf statement.
	 * \param[in] Since printf can take multiple arguments, these are passed as is the standard procedure in variadic functions
	 */
	static void writePrintfToLog(int logLevel, std::string className, const char *printfStart, ...){
		if (isLogDisabled(className, logLevel))
			return;
		int n, size = 100;
		char *logMessage;
		va_list ap;
		if ((logMessage = (char *)malloc (size * sizeof(char))) == NULL){
		  throw std::runtime_error("Memory allocation error 1 while creating log message.");
		  //std::cerr << "Mem error 1\n";
		  //return;
		}
		while (1) {
		  // Try to print in the allocated space.
		  va_start(ap, printfStart);
		  n = vsnprintf (logMessage, size, printfStart, ap);
		  va_end(ap);
		  // If that worked, return the string.
		  if (n > -1 && n < size){
		    //std::cerr << "Mem filled correctly" << n << ", " << size << "\n";
		    break;
		  }
		  // Else try again with more space.
		  if (n > -1)    // glibc 2.1
		    size = n+1; // precisely what is needed
		  else           // glibc 2.0
		    size *= 2;  // twice the old size
		  if ((logMessage = (char *)realloc (logMessage, size * sizeof(char))) == NULL){
		    throw std::runtime_error("Memory allocation error 2 while creating log message.");
		    
		    //std::cerr << "Mem error 2"  << n << ", " << size << "\n";
		    //return;
		  }
		}
		
		if (logLevel == ExactStatus || logLevel == ExactError || logLevel == ExactCritical)
			writeToLog(className, logLevel, logMessage, true);
		else
			writeToLog(className, logLevel, logMessage, false);
		free(logMessage);
	};
	/**
	 * This clears the log file obtained from getLogFileName() and initializes a new log with current date and time in it.
	 */
	static void resetLogFile(){
	  std::string logFileName = LogManager::getLogFileName();
	  FILE* stream = fopen(logFileName.c_str(), "w");
	  if (! stream){
	    std::string message("Cannot open log file for writing:");
	    message = message + logFileName.c_str();
	    throw std::invalid_argument(message.c_str()); 
	  }
	  fprintf(stream, " ");
	  
	  time_t rawtime;
	  struct tm * timeinfo;
	  time(&rawtime);
	  timeinfo = localtime(&rawtime);
	  
#ifdef textMode
	  fprintf(stream, "Log Started %s \n", asctime(timeinfo));
#endif
#ifndef textMode
	  fprintf(stream, "<HTML><BODY>");
	  fprintf(stream, "Log Started %s <br>", asctime(timeinfo));
#endif
	  
	  fflush(stream);
	  fclose(stream);
	};
	/**
	 * Set the directory to which log file must be written. If this function is not
	 * called, the default folder is c:\\ or /tmp/
	 * \param[in] inputDirectory Log directory is set to this value.
	 */
	static void setLogDirectory(char* inputDirectory);

	/**
	 * This function can used to control the creation of images during runtime.
	 * \param[in] className This class name is compared to see if images should be created or not during runtime
	 * \param[out] bool false if the image should be skipped (not created), true in case the image needs to be created
	 */
	static bool createImageDuringLogging(std::string className);


private:
	static char directoryPath[1024];
	static bool isPathDefined;

	/**
	 * Returns name of the log file.
	 */
	static std::string getLogFileName();

	/**
	 * Returns the directory of the log file.
	 */
	static const char* getLogDirectoryPath();

	/**
	 * This function can used to control the display of images during runtime.
	 * \param[in] className This class name is compared to see if images should be displayed or not during runtime
	 * \param[out] bool false if the image should be displayed, true in case the image needs to be displayed
	 */
	static bool allowImageDisplay(std::string className);

	/**
	 * This function decides if log information pertaining to the passed function need to be output or not.
	 * \param[in] className This class name is compared to see if it's log should be printed or not
	 * \param[out] bool true if the log information should be skipped (not printed), false in case the log needs to be printed.
	 */
	static bool isLogDisabled(std::string className, int logLevel);

	/**
	 * This function can be used in lieu of directing log results to cout.
	 * If a programmer absolutely wants to write outputs to cout, one can call
	 * this function.
	 * \param[in] className This class name is compared to see if it's log should be printed or not
	 */
	static std::ostream& getCoutStream(int logLevel, std::string className);

	
	/**
	 * Write message to the log file.
	 * \param[in] className Name of the class that invoked this function. This value is used to determine if log information for that class is to be printed or not.
	 * \param[in] message The message needed to be appended to the log file.
	 */
	static void writeToLog(std::string className, int logLevel, char* message, bool exact){
		if (isLogDisabled(className, logLevel))
			return;
		std::string *errMsg = new std::string("");
		
		if (exact){
			errMsg->append(message);
		}else{
			errMsg->append(className)/*+getTime()*/;
			errMsg->append(":");
			errMsg->append(message);
			#ifdef textMode
				errMsg->append("\n");
			#endif
			#ifndef textMode
				errMsg->append("<br>\n");
			#endif

		}
		writeToFile(getLogFileName(), errMsg);
		delete (errMsg);
	};


	/**
	 * gets the local time in ascii format. Example: Thu Aug 05 17:26:57 2004
	*/
	static char* getTime(){
		time_t rawtime;
		tm* ptm;
		time(&rawtime);
		ptm = localtime ( &rawtime );
		char* time = asctime(ptm);
		time[24] = ':';
		return time;
	};

private:
	/**
	 * A wrapper to the printf statement so that we need not bother with flushing the stream on each write. This version appends to the file, if a file is already present.
	 * \param[in] logFileName File to which message is to be appended
	 * \param[in] message message that needs to be printed
	 */
	static void writeToFile(std::string logFileName, std::string* message){
	  FILE* stream = fopen(logFileName.c_str(), "a+");

		if (stream){
			fprintf(stream, "%s", message->c_str());
			fflush(stream);
			fclose(stream);
		}else{
		  std::string message("Cannot open log file for append:");
		  message = message + logFileName.c_str();
		  throw std::invalid_argument(message.c_str()); 		  
		}
	};
};

#endif
