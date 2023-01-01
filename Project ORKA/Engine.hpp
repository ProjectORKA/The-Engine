
#pragma once

#include "FileSystem.hpp"
#include "ResourceManager.hpp"

struct Engine {
	FileSystem fileSystem;
	ResourceManager resourceManager = ResourceManager(fileSystem);
};