
#pragma once

#include "Basics.hpp"
#include "Random.hpp"

extern Float dnaInput;

struct DNAResult {
	enum{
		DNAinput,
		DNArandom
		//pi
		//e
	}type = DNAinput;
	Float variable = randomFloat(-2,2);

	virtual void print() {
		if (type == DNAinput) std::cout << "X";
		if (type == DNArandom) std::cout << variable;
	};

	virtual Float calculate() {
		if (type == DNAinput) variable = dnaInput;
		return variable;
	};
};

struct DNAFunction : public DNAResult {
	
	enum {
		DNAadd,
		DNAsub,
		DNAmul,
		DNAdiv,
		DNApow
	}type = DNAadd;
	
	DNAResult * a = nullptr;
	DNAResult * b = nullptr;

	Float calculate() override {
		a->calculate();
		b->calculate();

		if (type == DNAadd) variable = a->variable + b->variable;
		if (type == DNAsub) variable = a->variable - b->variable;
		if (type == DNAmul) variable = a->variable * b->variable;
		if (type == DNAdiv) variable = a->variable / b->variable;
		if (type == DNApow) variable = pow(a->variable,b->variable);

		return variable;
	};

	void print() override {
		if (type == DNAadd){std::cout << "add("; a->print();std::cout << ","; b->print(); std::cout << ")";}
		if (type == DNAsub){std::cout << "sub("; a->print();std::cout << ","; b->print(); std::cout << ")";}
		if (type == DNAmul){std::cout << "mul("; a->print();std::cout << ","; b->print(); std::cout << ")";}
		if (type == DNAdiv){std::cout << "div("; a->print();std::cout << ","; b->print(); std::cout << ")";}
		if (type == DNApow){std::cout << "pow("; a->print();std::cout << ","; b->print(); std::cout << ")";}
	}
};
