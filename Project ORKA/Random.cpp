#include "Random.hpp"

ULL RandomNumberGenerator::randomULL()
{
	c += c << 1;
	c += c >> 1;
	//c += a + b;
	//a = b;
	//b = c++;
	c += a;
	a = c++;
	return c;
}

RandomNumberGenerator::RandomNumberGenerator()
{
	for (int i = 0; i < 64; i++) randomULL();
}
