#pragma once

#include "Basics.hpp"

struct Logger
{
	~Logger();
	Logger() = delete;
	void     write(const std::string& line);
	explicit Logger(const std::string& logName, const Path& logDirectory = "Logs");
	void     openNewFile();
	[[nodiscard]] const Path& getPath() const;
	bool     operator==(const Logger& other) const = delete;
private:
	std::ofstream logFile;
	std::string   logName;
	Path          logDirectory;
	Path          logPath;

	[[nodiscard]] std::string getCurrentTime() const;
	void                        openFile();
};
