
#pragma once

//written by Omarito
//modified by
//  Blendurian

#include "Velox_Common.hpp"

TRE_NS_START

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

TRE_NS_END

