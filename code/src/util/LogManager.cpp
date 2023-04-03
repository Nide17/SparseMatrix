#include "LogManager.h"

bool LogManager::isPathDefined = false;
std::ofstream* LogManager::currStream = NULL;
char LogManager::directoryPath[1024];

// The stream that log output is sent to. By default this points to std::cerr.
std::ostream* ptrToCOUTStream(&std::cout);

// Null stream.
std::ofstream nullStream;

std::ostream& LogManager::getCoutStream(int logLevel, std::string className)
{
	//http://w3facility.org/question/redirect-debug-output-to-null-stream-instead-of-stdcerr/
	//http://stackoverflow.com/questions/19200207/redirect-debug-output-to-null-stream-instead-of-stdcerr
	if (isLogDisabled(className, logLevel))
		return nullStream;
	return *ptrToCOUTStream;
}

void LogManager::closeLogFileOStream(){
	if (LogManager::currStream != NULL){
		LogManager::currStream->flush();
		LogManager::currStream->close();
	}
	LogManager::currStream = NULL;
}
void LogManager::flushLogFileOStream(){
	if (LogManager::currStream != NULL)
		LogManager::currStream->flush();
}

std::ofstream& LogManager::getLogFileOStream(int logLevel, std::string className)
{
	//http://w3facility.org/question/redirect-debug-output-to-null-stream-instead-of-stdcerr/
	//http://stackoverflow.com/questions/19200207/redirect-debug-output-to-null-stream-instead-of-stdcerr
	if (isLogDisabled(className, logLevel))
		return nullStream;
	if (LogManager::currStream == NULL){
		LogManager::currStream = new std::ofstream();
		LogManager::currStream->open(getLogFileName(), std::ofstream::out | std::ofstream::app);
	}
	if (LogManager::currStream != NULL ? LogManager::currStream->is_open() : false)
		return *LogManager::currStream;
	return nullStream;
}

void LogManager::setLogDirectory(char* inputDirectory){
	isPathDefined = true;
	sprintf(directoryPath, "%s", inputDirectory);
}

std::string LogManager::getLogFileName(){
#ifdef windows
  std::string location = "c:";
#else
  std::string location = "/tmp";
#endif
  if (isPathDefined)
    location = LogManager::directoryPath;
  
#ifdef windows
  char fileseparator[5]  = "\\";
#else
  char fileseparator[5]  = "/";
#endif
  
#ifdef textMode
  location = location + fileseparator + "log_data.txt";
#else
  location = location + fileseparator + "log_data.html";
#endif
  return location;
}

bool LogManager::allowImageDisplay(std::string className){
  
  return false;
}

bool LogManager::createImageDuringLogging(std::string className){
  
  return false;
}

bool LogManager::isLogDisabled(std::string className, int logLevel){
  if (logLevel == LogManager::Critical)
    return false;

  if (className.find("UniqueInt::processFile") == 0
      && logLevel >= LogManager::Status)
    return true;
  if (className.find("UniqueInt::getRandomInt") == 0
      && logLevel >= LogManager::Status)
    return true;
  
  return false;
}
