#include "Engine.hpp"
#include "Debug.hpp"
#include "FileSystem.hpp"
#include "FileTypes.hpp"
#include "JobSystem.hpp"
#include "ResourceManager.hpp"

void Engine::create(const Int argc, Char* argv[], const Bool useJobSystem)
{
	this->useJobSystem = useJobSystem;

	//print debug launch parameters if required
	if (debugLaunchParameters) logDebugLaunchParameters(argc, argv);

	// scan launch parameters for current executable position
	// this is important for things like file viewers which are executed on a file somewhere else
	for (Int i = 0; i < argc; i++)
	{
		if (Path path = argv[i]; isExecutableFile(path))
		{
			executablePath = getDirectory(path);
			setCurrentPath(executablePath);
			break;
		}
	}

	resourceManager.create();
	if (useJobSystem) jobSystem.start();
}

Engine::~Engine()
{
	if (useJobSystem) jobSystem.waitStopAndJoin();
}
