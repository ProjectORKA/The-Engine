#include "Engine.hpp"
#include "Debug.hpp"
#include "FileSystem.hpp"
#include "FileTypes.hpp"
#include "AudioEngine.hpp"
#include "JobSystem.hpp"
#include "ResourceManager.hpp"
#include "Agent/AgentBridge.hpp"
#include "Agent/AgentEngineCommands.hpp"

namespace
{
	Bool readAgentOption(const Int argc, Char* argv[], Int& index, const String& name, String& value)
	{
		const String argument = argv[index];
		const String prefix = name + "=";
		if(argument.starts_with(prefix))
		{
			value = argument.substr(prefix.size());
			return true;
		}
		if(argument == name && index + 1 < argc)
		{
			value = argv[++index];
			return true;
		}
		return false;
	}
}

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

	audioEngine.create();
	resourceManager.create();
	if (useJobSystem) jobSystem.start();

	Bool enableAgentBridge = false;
	AgentBridgeConfig agentConfig;
	for(Int i = 1; i < argc; ++i)
	{
		const String argument = argv[i];
		if(argument == "--agent-bridge") enableAgentBridge = true;
		String value;
		if(readAgentOption(argc, argv, i, "--agent-port", value))
		{
			const Int port = std::stoi(value);
			if(port < 1 || port > 65535) throw std::runtime_error("--agent-port must be between 1 and 65535");
			agentConfig.port = static_cast<UShort>(port);
		}
		else if(readAgentOption(argc, argv, i, "--agent-token", value))
			agentConfig.token = value;
	}
	if(enableAgentBridge)
	{
		registerAgentEngineCommands();
		if(!agentBridge.start(agentConfig))
			throw std::runtime_error("Could not start ORKA agent bridge");
		std::cout << "ORKA_AGENT_ENDPOINT=127.0.0.1:" << agentBridge.port() << '\n';
		std::cout << "ORKA_AGENT_TOKEN=" << agentBridge.token() << '\n';
	}
}

Engine::~Engine()
{
	agentBridge.stop();
	resourceManager.destroy();
	audioEngine.destroy();
	if (useJobSystem) jobSystem.waitStopAndJoin();
}
