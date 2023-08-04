#include "SimpleRTSHuman.hpp"
#include "FileSystem.hpp"

void SimpleRTSHumanSystem::create()
{
	simpleRTSHumanMaleNames = loadStringVector("simpleRTSHumanNames.txt");
}

void SimpleRTSHuman::spawn(const Vec3 position)
{
	this->position = position;
}
