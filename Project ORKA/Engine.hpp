#pragma once

#include "Basics.hpp"
#include "Debug.hpp"
#include "FileSystem.hpp"
#include "FileTypes.hpp"
#include "JobSystem.hpp"
#include "ResourceManager.hpp"
#include "Settings.hpp"

struct Engine
{
	void create(const Int argc, Char* argv[]) const
	{
		//print debug launch parameters if required
		if(debugLaunchParameters) logDebugLaunchParameters(argc, argv);

		// scan launch parameters for current executable position
		// this is important for things like file viewers which are executed on a file somewhere else
		for(Int i = 0; i < argc; i++)
		{
			Path path = argv[i];
			if(isExecutableFile(path))
			{
				executablePath = getDirectory(path);
				setCurrentPath(executablePath);
				break;
			}
		}

		resourceManager.create();
		jobSystem.start();
	}

	~Engine()
	{
		jobSystem.waitStopAndJoin();
	}
};
