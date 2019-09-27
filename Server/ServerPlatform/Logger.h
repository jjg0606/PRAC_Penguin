#pragma once
#include <string>
#include <fstream>


class Logger
{
private:
	std::ofstream ofs;
	const static std::string contextPath;
	static Logger instance;
	bool isOK;
	Logger();
	void innerLog(std::string& str);
public:
	static void Log(std::string str);
	~Logger();
};

