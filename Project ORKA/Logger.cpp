#include "Logger.hpp"

Logger::~Logger()
{
	if (logFile.is_open()) logFile.close();
}

std::string Logger::getCurrentTime() const
{
	const auto        now         = std::chrono::system_clock::now();
	const std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
	std::stringstream ss;
	ss << std::put_time(localtime(&currentTime), "%Y-%m-%d_%H-%M-%S");
	return ss.str();
}

Logger::Logger(const std::string& logName, const Path& logDirectory)
{
	this->logName      = logName;
	this->logDirectory = logDirectory;
}

void Logger::openNewFile()
{
	openFile();
}

const Path& Logger::getPath() const
{
	return logPath;
}

void Logger::openFile()
{
	if (logFile.is_open()) logFile.close();
	std::filesystem::create_directories(logDirectory);
	logPath = logDirectory / (logName + "-" + getCurrentTime() + ".log");
	logFile.open(logPath, std::ios::out | std::ios::trunc);
}

void Logger::write(const std::string& line)
{
	if (!logFile.is_open())
	{
		openFile();
	}
	if (logFile.is_open())
	{
		logFile << line << "\n";
		logFile.flush();
	}
}
