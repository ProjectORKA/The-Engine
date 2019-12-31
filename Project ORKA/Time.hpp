
#include <chrono>

struct Time {
	bool paused = false;

	void resetTime();
	void processTime();
	float delta();
	float total();

private:
	std::chrono::steady_clock::time_point lastTime;
	std::chrono::steady_clock::time_point currentTime;
	std::chrono::duration<double, std::ratio< 1 / 1 >> deltaDuration = std::chrono::seconds::zero();
	std::chrono::duration<double, std::ratio< 1 / 1 >> totalDuration = std::chrono::seconds::zero();
};