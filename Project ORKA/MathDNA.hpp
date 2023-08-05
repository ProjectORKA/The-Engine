#pragma once

#include "Basics.hpp"
#include "Random.hpp"

extern Float dnaInput;

struct DNAResult
{
	enum
	{
		DNAinput,
		DNArandom
		// pi
		// e
	} type = DNAinput;

	Float variable = randomFloatFast(-2.0, 2.0);

	virtual void  print();
	virtual Float calculate();
};

struct DNAFunction : DNAResult
{
	enum
	{
		DNAadd,
		DNAsub,
		DNAmul,
		DNAdiv,
		DNApow
	} type = DNAadd;

	DNAResult* a = nullptr;
	DNAResult* b = nullptr;

	void  print() override;
	Float calculate() override;
};
