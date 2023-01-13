
#pragma once

//written by Omarito
//modified by
//  Blendurian

#include "Velox_Common.hpp"

template<typename T>
class Singleton
{
protected:
	Singleton() = default;

	Singleton(const Singleton&) = delete;

	Singleton &operator=(const Singleton&) = delete;


public:
	static T& instance()
	{
		static T instance;
		return instance;
	}
};
