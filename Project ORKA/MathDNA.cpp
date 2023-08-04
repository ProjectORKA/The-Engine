#include "MathDNA.hpp"

Float dnaInput = 0.5;

void DNAResult::print()
{
	if(type == DNAinput) std::cout << "X";
	if(type == DNArandom) std::cout << variable;
}

void DNAFunction::print()
{
	if(type == DNAadd)
	{
		std::cout << "add(";
		a->print();
		std::cout << ",";
		b->print();
		std::cout << ")";
	}
	if(type == DNAsub)
	{
		std::cout << "sub(";
		a->print();
		std::cout << ",";
		b->print();
		std::cout << ")";
	}
	if(type == DNAmul)
	{
		std::cout << "mul(";
		a->print();
		std::cout << ",";
		b->print();
		std::cout << ")";
	}
	if(type == DNAdiv)
	{
		std::cout << "div(";
		a->print();
		std::cout << ",";
		b->print();
		std::cout << ")";
	}
	if(type == DNApow)
	{
		std::cout << "pow(";
		a->print();
		std::cout << ",";
		b->print();
		std::cout << ")";
	}
}

Float DNAResult::calculate()
{
	if(type == DNAinput) variable = dnaInput;
	return variable;
}

Float DNAFunction::calculate()
{
	a->calculate();
	b->calculate();

	if(type == DNAadd) variable = a->variable + b->variable;
	if(type == DNAsub) variable = a->variable - b->variable;
	if(type == DNAmul) variable = a->variable * b->variable;
	if(type == DNAdiv) variable = a->variable / b->variable;
	if(type == DNApow) variable = pow(a->variable, b->variable);

	return variable;
}
