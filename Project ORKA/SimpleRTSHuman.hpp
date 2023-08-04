#pragma once

#include "Basics.hpp"

enum class SimpleRTSHumanState
{
	LookingForWood,
	Delivering,
	HuntingRabbit,
};

struct SimpleRTSHuman
{
	String              name;
	SimpleRTSHumanState state;
	String              surname;
	Vec3                position;
	Vec3                targetPosition;

	void spawn(Vec3 position);
};

struct SimpleRTSHumanSystem
{
	Vector<SimpleRTSHuman> humans;
	Vector<String>         simpleRTSHumanMaleNames;

	void create();
};
