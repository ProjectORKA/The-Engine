#pragma once

#include "AgentValue.hpp"

struct Window;

enum class AgentThreadAffinity : Byte
{
	Io,
	Main,
	Render
};

struct AgentBridgeConfig
{
	UShort port = 42123;
	String token;
	SizeT maxFrameBytes = 1024 * 1024;
	SizeT maxQueuedMainCommands = 256;
	SizeT maxQueuedRenderCommands = 256;
	SizeT maxLogEntries = 4096;
	SizeT maxProfileFrames = 4096;
	UInt commandTimeoutMilliseconds = 5000;
};

struct AgentCommandContext
{
	U64 requestSequence = 0;
	U16 targetWindowId = 0;
	Bool hasTargetWindow = false;
	Window* window = nullptr;
};

using AgentCommandHandler = Function<AgentValue(const AgentCommandContext&, const AgentValue&)>;

struct AgentCommandDescription
{
	String method;
	String summary;
	AgentThreadAffinity affinity = AgentThreadAffinity::Io;
	AgentValue params = AgentValue::Object{};
	AgentValue result = AgentValue::Object{};
	Bool requiresWindow = false;
};

struct AgentCommand
{
	AgentCommandDescription description;
	AgentCommandHandler handler;
};

enum class AgentLogLevel : Byte
{
	Trace,
	Debug,
	Info,
	Warning,
	Error,
	Fatal
};

struct AgentFrameMetrics
{
	U16 windowId = 0;
	U64 frameIndex = 0;
	Double frameMilliseconds = 0.0;
	Double layoutMilliseconds = 0.0;
	Double updateMilliseconds = 0.0;
	Double renderMilliseconds = 0.0;
	Double finalizeMilliseconds = 0.0;
	Double gpuMilliseconds = 0.0;
	U64 drawCalls = 0;
	U64 triangles = 0;
	U64 timestampMicroseconds = 0;
};

class AgentBridge
{
public:
	AgentBridge();
	~AgentBridge();
	AgentBridge(const AgentBridge&) = delete;
	AgentBridge& operator=(const AgentBridge&) = delete;

	[[nodiscard]] Bool start(const AgentBridgeConfig& config = {});
	void stop();
	[[nodiscard]] Bool isRunning() const;
	[[nodiscard]] UShort port() const;
	[[nodiscard]] String token() const;

	SizeT drainMain(SizeT maxCommands = 64);
	SizeT drainRender(Window& window, SizeT maxCommands = 64);

	void recordFrame(const AgentFrameMetrics& metrics);
	void writeLog(AgentLogLevel level, const String& category, const String& message,
		const AgentValue::Object& fields = {});

	[[nodiscard]] Bool registerCommand(const AgentCommand& command, String* error = nullptr);
	[[nodiscard]] Bool unregisterCommand(const String& method);
	[[nodiscard]] Vector<AgentCommandDescription> listMethods() const;

private:
	struct Implementation;
	UniquePointer<Implementation> implementation;
};

extern AgentBridge agentBridge;

[[nodiscard]] Bool runAgentBridgeSelfTests();

