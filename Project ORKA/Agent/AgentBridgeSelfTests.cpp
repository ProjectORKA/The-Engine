#include "AgentBridge.hpp"
#include "../ASIONetworking.hpp"

#include <atomic>

namespace
{
	Bool check(const Bool condition, Bool& result)
	{
		result = result && condition;
		return condition;
	}

	AgentJsonParseResult callBridge(const UShort bridgePort, const String& request)
	{
		NetworkingContext context;
		asio::ip::tcp::socket socket(context);
		socket.connect({asio::ip::make_address_v4("127.0.0.1"), bridgePort});
		const U32 size = static_cast<U32>(request.size());
		const Array<Byte, 4> header{
			static_cast<Byte>(size), static_cast<Byte>(size >> 8),
			static_cast<Byte>(size >> 16), static_cast<Byte>(size >> 24)
		};
		asio::write(socket, asio::buffer(header));
		asio::write(socket, asio::buffer(request));
		Array<Byte, 4> responseHeader{};
		asio::read(socket, asio::buffer(responseHeader));
		const U32 responseSize = responseHeader[0] | (static_cast<U32>(responseHeader[1]) << 8)
			| (static_cast<U32>(responseHeader[2]) << 16)
			| (static_cast<U32>(responseHeader[3]) << 24);
		String response(responseSize, '\0');
		asio::read(socket, asio::buffer(response));
		return parseAgentJson(response);
	}
}

Bool runAgentBridgeSelfTests()
{
	Bool passed = true;

	const String source =
		R"({"array":[null,true,-17,2.5],"escaped":"line\n\u20ac","object":{"value":"ok"}})";
	const AgentJsonParseResult parsed = parseAgentJson(source);
	check(parsed.succeeded(), passed);
	if(parsed.succeeded())
	{
		check(parsed.value.isObject(), passed);
		check(parsed.value.find("array") && parsed.value.find("array")->isArray(), passed);
		check(parsed.value.find("escaped") && parsed.value.find("escaped")->asString() == "line\n\xe2\x82\xac", passed);

		const AgentJsonParseResult roundTrip = parseAgentJson(serializeAgentJson(parsed.value));
		check(roundTrip.succeeded(), passed);
		check(roundTrip.value.find("object") && roundTrip.value.find("object")->isObject(), passed);
	}

	check(!parseAgentJson(R"({"duplicate":1,"duplicate":2})").succeeded(), passed);
	check(!parseAgentJson("[[[0]]]", AgentJsonLimits{1024, 1, 100, 1024}).succeeded(), passed);
	check(!parseAgentJson(R"("\ud800")").succeeded(), passed);
	check(!parseAgentJson("01").succeeded(), passed);

	AgentBridge bridge;
	const SizeT initialMethodCount = bridge.listMethods().size();
	std::atomic<Int> calls = 0;
	AgentCommand command;
	command.description.method = "selfTest.echo";
	command.description.summary = "Self-test command";
	command.description.affinity = AgentThreadAffinity::Main;
	command.description.params = AgentValue::Object{{"type", "object"}};
	command.description.result = AgentValue::Object{{"type", "object"}};
	command.handler = [&calls](const AgentCommandContext&, const AgentValue& params)
	{
		++calls;
		return params;
	};
	String error;
	check(bridge.registerCommand(command, &error), passed);
	check(error.empty(), passed);
	check(!bridge.registerCommand(command, &error), passed);
	check(!error.empty(), passed);
	check(bridge.listMethods().size() == initialMethodCount + 1, passed);
	check(bridge.unregisterCommand("selfTest.echo"), passed);
	check(bridge.listMethods().size() == initialMethodCount, passed);
	check(calls.load() == 0, passed);

	AgentCommand invalid;
	invalid.description.method = "invalid method";
	invalid.handler = [](const AgentCommandContext&, const AgentValue&) { return AgentValue(); };
	check(!bridge.registerCommand(invalid), passed);

	AgentBridgeConfig config;
	config.port = 0;
	config.token = "self-test-token";
	check(bridge.start(config), passed);
	if(bridge.isRunning())
	{
		const AgentJsonParseResult authorized = callBridge(bridge.port(),
			R"({"version":1,"id":"ok","token":"self-test-token","method":"agent.health","params":{}})");
		check(authorized.succeeded(), passed);
		check(authorized.value.find("ok") && authorized.value.find("ok")->asBool(), passed);
		const AgentJsonParseResult rejected = callBridge(bridge.port(),
			R"({"version":1,"id":"bad","token":"wrong","method":"agent.health","params":{}})");
		check(rejected.succeeded(), passed);
		check(rejected.value.find("ok") && !rejected.value.find("ok")->asBool(true), passed);
		bridge.stop();
		check(!bridge.isRunning(), passed);
	}

	return passed;
}

