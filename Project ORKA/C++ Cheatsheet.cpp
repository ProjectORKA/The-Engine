
#include "Basics.hpp" //allows you to use the most basic functionalities and datatypes used in ORKA

//COLORS:
					//comment					//this is a comment
void				doThis() {};				//this is a function
struct				ObjectType;					//this is a data type
					static const bool;			//this is a base type
int					variable;					//this is a variable
#define				PREPROCESSOR_DEFINITION		//this is a preprocessor definition
const char* t =		"String Literal";			//this is a string literal

//BASE DATA STRUCTURES:
bool dataBoolean;					// 1 byte  | 0 to 1
char dataCharacter;					// 1 byte  | 0 to 255
short dataShort;					// 2 bytes | -32768 to 32767
unsigned short dataUnsignedShort;	// 2 bytes | 0 to 65535
int	dataInteger;					// 4 bytes | -2147483648 to 2147483647
unsigned int dataUnsignedInteger;	// 4 bytes | 0 to 4294967295
float dataFloat;					// 4 bytes
//etc.

void cppCheatsheet() {



	//ORKA BASE DATA STRUCTURES
	Bool;
	Int;
	UInt;
	Float;
	Vec3;
	DVec3;
	Matrix;
	Name;

	//MATHEMATICAL OPERATORS
	{
		//float operators
		Float a = 5;
		Float b = 10;
		Float c = 0;

		c = a + b;	//add
		c = a - b;	//subtract
		c = a * b;	//multiply
		c = a / b;	//divide
	}
	{
		//this gets executed
		if (true) doThis();

		
		//this does not
		if (false) doThis();

		//you can use variables for if statements
		Boolean b = true;
		if (b) doThis();

		//this works too
		if (5 > 4) doThis();
		
		//
	}
	{
		//integer operators
		Int a = 8;
		Int b = 3;
		Int c = 0;

		c = a % b;	//modulo
		c = a & b;
	}
	{
		//bit manipulation
		UInt a = 3;
		UInt b = 2;
		UInt c = 0;

		c = a << 1;	//bitshift left  (multiply by 2,4,8,16,etc)
		c = a >> 1;	//bitshift right (divide by 2,4,8,16,etc)
		c = a & b;	//is 1 if both numbers are 1 at that position
		c = a | b;	//is 1 if any number is 1 at that position
		c = a ^ b;	//is 1 if both bits are different at position
	}
}