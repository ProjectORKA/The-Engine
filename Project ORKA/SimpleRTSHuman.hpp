#pragma once

#include "Basics.hpp"
#include "File.hpp"
#include "FileSystem.hpp"


enum class SimpleRTSHumanState {
	LookingForWood,
	Delivering,
	HuntingRabbit,
};

struct SimpleRTSHuman {
	String name;
	String surname;
	Vec3 position;
	Vec3 targetPosition;
	SimpleRTSHumanState state;

	void spawn(Vec3 position);
};

struct SimpleRTSHumanSystem {
	Vector<SimpleRTSHuman> humans;
	Vector<String> simpleRTSHumanMaleNames;
	
	void create();
};