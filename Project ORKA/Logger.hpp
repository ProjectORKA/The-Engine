#pragma once

#include "Basics.hpp"

struct Logger
{
	~Logger();
	Logger() = delete;
	void     write(const std::string& line);
	explicit Logger(const std::string& logName);
	bool     operator==(const Logger& other) const = delete;
private:
	std::ofstream logFile;

	[[nodiscard]] std::string getCurrentTime() const;
};
