#pragma once

#include <iostream>

#ifdef MODULES_EXPORTS
#define MODULES_API __declspec(dllexport)
#else
#define MODULES_API __declspec(dllimport)
#endif

extern "C" MODULES_API void test();