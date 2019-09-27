#include "Logger.h"
#include <ctime>


using namespace std;

const string Logger::contextPath = "./log.txt";

Logger Logger::instance;

Logger::Logger()
{
	ofs.open(contextPath, ios::out);
	isOK = ofs.good();
}

Logger::~Logger()
{
	ofs.close();
}

void Logger::Log(string str)
{
	instance.innerLog(str);
}

void Logger::innerLog(string& str)
{
	if (!isOK)
	{
		return;
	}

	time_t t = time(NULL);
	struct tm timePtr;
	localtime_s(&timePtr, &t);

	ofs << '[' << (timePtr.tm_mon+1) << "-" << timePtr.tm_mday << ' '
		<< timePtr.tm_hour << ":" << timePtr.tm_min << ":"
		<< timePtr.tm_sec << "] ";

	ofs << str << '\n';
}

