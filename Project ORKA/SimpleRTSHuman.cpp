#include "SimpleRTSHuman.hpp"

void SimpleRTSHuman::spawn(const Vec3 position) { this->position = position; }

void SimpleRTSHumanSystem::create() { simpleRTSHumanMaleNames = loadStringVector("simpleRTSHumanNames.txt"); }
