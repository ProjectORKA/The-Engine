#include "Logger.hpp"

Logger::~Logger()
{
	if(logFile.is_open()) logFile.close();
}

std::string Logger::getCurrentTime() const
{
	const auto        now         = std::chrono::system_clock::now();
	const std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
	std::stringstream ss;
	ss << std::put_time(localtime(&currentTime), "%Y-%m-%d %H-%M-%S");
	return ss.str();
}

Logger::Logger(const std::string& logName)
{
	logFile.open(logName + getCurrentTime() + ".log");
}

void Logger::write(const std::string& line)
{
	if(logFile.is_open())
	{
		logFile << line << "\n";
		logFile.flush();
	}
}
