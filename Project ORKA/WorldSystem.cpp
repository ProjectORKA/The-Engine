
#include "Program.hpp"

Chunk::Chunk()
{
	addTriangle(entityComponentSystem);
	addTriangle(entityComponentSystem);
	debugPrint("|--Chunk was created!");
}
Chunk::~Chunk()
{
	debugPrint("|--Chunk was destroyed!");
}