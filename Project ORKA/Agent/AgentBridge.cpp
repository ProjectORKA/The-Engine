#include "AgentBridge.hpp"

#include "../ASIONetworking.hpp"
#include "../Debug.hpp"
#include "../Window.hpp"

#include <atomic>
#include <cctype>
#include <random>
#include <unordered_map>

namespace
{
	constexpr SizeT hardMaximumFrameBytes = 1024 * 1024;

	U64 nowMicroseconds()
	{
		return static_cast<U64>(std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::system_clock::now().time_since_epoch()).count());
	}

	Path validatedArtifactPath(const String& requested, const String& fallbackName)
	{
		const Path root = std::filesystem::absolute(Path("AgentArtifacts")).lexically_normal();
		std::filesystem::create_directories(root);
		Path candidate = requested.empty() ? root / fallbackName : Path(requested);
		if(candidate.is_relative()) candidate = root / candidate;
		candidate = candidate.lexically_normal();
		String rootText = root.generic_string();
		String candidateText = candidate.generic_string();
		std::transform(rootText.begin(), rootText.end(), rootText.begin(),
			[](const unsigned char value) { return static_cast<Char>(std::tolower(value)); });
		std::transform(candidateText.begin(), candidateText.end(), candidateText.begin(),
			[](const unsigned char value) { return static_cast<Char>(std::tolower(value)); });
		const String rootBase = rootText;
		if(!rootText.ends_with('/')) rootText.push_back('/');
		if(candidateText != rootBase && !candidateText.starts_with(rootText))
			throw std::runtime_error("Artifact path must be inside AgentArtifacts");
		return candidate;
	}

	String affinityName(const AgentThreadAffinity affinity)
	{
		switch(affinity)
		{
		case AgentThreadAffinity::Io: return "io";
		case AgentThreadAffinity::Main: return "main";
		case AgentThreadAffinity::Render: return "render";
		}
		return "unknown";
	}

	String logLevelName(const AgentLogLevel level)
	{
		switch(level)
		{
		case AgentLogLevel::Trace: return "trace";
		case AgentLogLevel::Debug: return "debug";
		case AgentLogLevel::Info: return "info";
		case AgentLogLevel::Warning: return "warning";
		case AgentLogLevel::Error: return "error";
		case AgentLogLevel::Fatal: return "fatal";
		}
		return "unknown";
	}

	AgentValue successEnvelope(const AgentValue& id, AgentValue result)
	{
		return AgentValue::Object{
			{"version", 1},
			{"id", id},
			{"ok", true},
			{"result", std::move(result)}
		};
	}

	AgentValue errorEnvelope(const AgentValue& id, const String& code, const String& message)
	{
		return AgentValue::Object{
			{"version", 1},
			{"id", id},
			{"ok", false},
			{"error", AgentValue::Object{{"code", code}, {"message", message}}}
		};
	}

	Bool constantTimeEqual(const String& left, const String& right)
	{
		SizeT difference = left.size() ^ right.size();
		const SizeT count = (std::max)(left.size(), right.size());
		for(SizeT i = 0; i < count; ++i)
		{
			const Byte a = i < left.size() ? static_cast<Byte>(left[i]) : 0;
			const Byte b = i < right.size() ? static_cast<Byte>(right[i]) : 0;
			difference |= static_cast<SizeT>(a ^ b);
		}
		return difference == 0;
	}

	String randomToken()
	{
		std::random_device random;
		const char hex[] = "0123456789abcdef";
		String result(64, '0');
		for(SizeT i = 0; i < 32; ++i)
		{
			const UInt value = random();
			const Byte byte = static_cast<Byte>(value ^ (value >> 8) ^ (value >> 16) ^ (value >> 24));
			result[i * 2] = hex[byte >> 4];
			result[i * 2 + 1] = hex[byte & 15];
		}
		return result;
	}

	AgentValue frameToValue(const AgentFrameMetrics& frame)
	{
		return AgentValue::Object{
			{"windowId", frame.windowId},
			{"frameIndex", frame.frameIndex},
			{"frameMs", frame.frameMilliseconds},
			{"layoutMs", frame.layoutMilliseconds},
			{"updateMs", frame.updateMilliseconds},
			{"renderMs", frame.renderMilliseconds},
			{"finalizeMs", frame.finalizeMilliseconds},
			{"gpuMs", frame.gpuMilliseconds},
			{"drawCalls", frame.drawCalls},
			{"triangles", frame.triangles},
			{"timestampUs", frame.timestampMicroseconds}
		};
	}
}

struct AgentBridge::Implementation
{
	struct Session;

	struct Pending
	{
		std::atomic<Bool> completed = false;
		U64 sequence = 0;
		U16 windowId = 0;
		Bool hasWindowId = false;
		AgentValue id;
		AgentValue params;
		AgentCommand command;
		UInt remainingFrames = 0;
		SharedPointer<Session> session;
		SharedPointer<asio::steady_timer> timer;
	};

	AgentBridgeConfig config;
	mutable Mutex stateMutex;
	Mutex drainMutex;
	std::unordered_map<String, AgentCommand> commands;
	std::deque<SharedPointer<Pending>> mainQueue;
	std::deque<SharedPointer<Pending>> renderQueue;
	std::deque<AgentValue> logs;
	std::deque<AgentFrameMetrics> profileFrames;
	std::unordered_map<String, std::deque<Double>> methodLatencies;
	Bool profiling = false;
	std::atomic<Bool> running = false;
	std::atomic<U64> nextSequence = 1;
	std::atomic<U64> nextLogId = 1;
	UniquePointer<NetworkingContext> io;
	UniquePointer<asio::ip::tcp::acceptor> acceptor;
	Thread ioThread;
	UShort boundPort = 0;

	void recordMethodLatency(const String& method, const std::chrono::steady_clock::time_point start)
	{
		const Double elapsed = std::chrono::duration<Double, std::milli>(
			std::chrono::steady_clock::now() - start).count();
		std::lock_guard lock(stateMutex);
		if(!profiling) return;
		auto& samples = methodLatencies[method];
		samples.push_back(elapsed);
		while(samples.size() > 1024) samples.pop_front();
	}

	struct Session final : std::enable_shared_from_this<Session>
	{
		explicit Session(Implementation& owner, asio::ip::tcp::socket socket)
			: owner(owner), socket(std::move(socket)) {}

		void start() { readHeader(); }

		void close()
		{
			asio::error_code ignored;
			socket.shutdown(asio::ip::tcp::socket::shutdown_both, ignored);
			socket.close(ignored);
		}

		void send(const AgentValue& envelope)
		{
			String responsePayload;
			try
			{
				responsePayload = serializeAgentJson(envelope,
					AgentJsonLimits{owner.config.maxFrameBytes, 64, 100000, owner.config.maxFrameBytes});
			}
			catch(...)
			{
				responsePayload = serializeAgentJson(errorEnvelope({}, "response_too_large", "Response exceeds frame limit"));
			}

			String frame(4, '\0');
			const U32 length = static_cast<U32>(responsePayload.size());
			frame[0] = static_cast<char>(length & 0xff);
			frame[1] = static_cast<char>((length >> 8) & 0xff);
			frame[2] = static_cast<char>((length >> 16) & 0xff);
			frame[3] = static_cast<char>((length >> 24) & 0xff);
			frame += responsePayload;

			auto self = shared_from_this();
			asio::post(socket.get_executor(), [self, frame = std::move(frame)]() mutable
			{
				const Bool writing = !self->outgoing.empty();
				self->outgoing.push_back(std::move(frame));
				if(!writing) self->writeNext();
			});
		}

	private:
		Implementation& owner;
		asio::ip::tcp::socket socket;
		std::array<Byte, 4> header{};
		String payload;
		std::deque<String> outgoing;

		void readHeader()
		{
			auto self = shared_from_this();
			asio::async_read(socket, asio::buffer(header), [self](const asio::error_code& error, SizeT)
			{
				if(error) return;
				const U32 length = static_cast<U32>(self->header[0])
					| (static_cast<U32>(self->header[1]) << 8)
					| (static_cast<U32>(self->header[2]) << 16)
					| (static_cast<U32>(self->header[3]) << 24);
				if(length == 0 || length > self->owner.config.maxFrameBytes)
				{
					self->close();
					return;
				}
				self->payload.assign(length, '\0');
				self->readPayload();
			});
		}

		void readPayload()
		{
			auto self = shared_from_this();
			asio::async_read(socket, asio::buffer(payload), [self](const asio::error_code& error, SizeT)
			{
				if(error) return;
				const auto parsed = parseAgentJson(self->payload,
					AgentJsonLimits{self->owner.config.maxFrameBytes, 64, 100000, self->owner.config.maxFrameBytes});
				if(!parsed.succeeded())
					self->send(errorEnvelope({}, "invalid_json", parsed.error));
				else
					self->owner.dispatch(self, parsed.value);
				self->readHeader();
			});
		}

		void writeNext()
		{
			auto self = shared_from_this();
			asio::async_write(socket, asio::buffer(outgoing.front()),
				[self](const asio::error_code& error, SizeT)
				{
					if(error)
					{
						self->outgoing.clear();
						return;
					}
					self->outgoing.pop_front();
					if(!self->outgoing.empty()) self->writeNext();
				});
		}
	};

	void installBuiltins()
	{
		const AgentValue emptySchema = AgentValue::Object{{"type", "object"}};
		auto add = [this, &emptySchema](const String& method, const String& summary,
			AgentCommandHandler handler, AgentValue params = AgentValue::Object{{"type", "object"}})
		{
			AgentCommand command;
			command.description = {method, summary, AgentThreadAffinity::Io,
				std::move(params), AgentValue::Object{}, false};
			command.handler = std::move(handler);
			commands.emplace(method, std::move(command));
		};

		add("agent.capabilities", "Reports bridge protocol capabilities",
			[this](const AgentCommandContext&, const AgentValue&)
			{
				return AgentValue::Object{
					{"protocol", "orka-agent-bridge"},
					{"version", 1},
					{"transport", "tcp-length-prefixed-json"},
					{"bindAddress", "127.0.0.1"},
					{"maxFrameBytes", static_cast<U64>(config.maxFrameBytes)},
					{"affinities", AgentValue::Array{"io", "main", "render"}},
					{"features", AgentValue::Array{
						"screenshots", "input", "ui-inspection", "ui-semantic", "logs", "profiling", "registered-functions"}},
					{"coordinates", AgentValue::Object{
						{"origin", "top-left"}, {"x", "right"}, {"y", "down"}, {"units", "logical-pixels"}}},
					{"artifactRoot", std::filesystem::absolute(Path("AgentArtifacts")).lexically_normal().string()}
				};
			}, emptySchema);

		add("agent.listMethods", "Lists registered command descriptions",
			[this](const AgentCommandContext&, const AgentValue&)
			{
				AgentValue::Array result;
				for(const auto& description : descriptions())
				{
					result.emplace_back(AgentValue::Object{
						{"method", description.method},
						{"summary", description.summary},
						{"affinity", affinityName(description.affinity)},
						{"requiresWindow", description.requiresWindow},
						{"params", description.params},
						{"result", description.result}
					});
				}
				return result;
			}, emptySchema);

		add("agent.health", "Reports bridge health and queue depths",
			[this](const AgentCommandContext&, const AgentValue&)
			{
				std::lock_guard lock(stateMutex);
				return AgentValue::Object{
					{"status", running.load() ? "ok" : "stopping"},
					{"port", boundPort},
					{"mainQueueDepth", static_cast<U64>(mainQueue.size())},
					{"renderQueueDepth", static_cast<U64>(renderQueue.size())},
					{"logEntries", static_cast<U64>(logs.size())},
					{"profiling", profiling}
				};
			}, emptySchema);

		add("logs.read", "Reads recent structured log entries",
			[this](const AgentCommandContext&, const AgentValue& params)
			{
				SizeT limit = 200;
				U64 afterId = 0;
				String level;
				String category;
				if(const auto* value = params.find("limit"); value && value->isInteger() && value->asInteger() >= 0)
					limit = (std::min)(static_cast<SizeT>(value->asInteger()), config.maxLogEntries);
				if(const auto* value = params.find("afterId"); value && value->isInteger() && value->asInteger() >= 0)
					afterId = static_cast<U64>(value->asInteger());
				if(const auto* value = params.find("level"); value && value->isString()) level = value->asString();
				if(const auto* value = params.find("category"); value && value->isString()) category = value->asString();
				AgentValue::Array result;
				std::lock_guard lock(stateMutex);
				const SizeT begin = logs.size() > limit ? logs.size() - limit : 0;
				for(SizeT i = begin; i < logs.size(); ++i)
				{
					const AgentValue* id = logs[i].find("id");
					const AgentValue* entryLevel = logs[i].find("level");
					const AgentValue* entryCategory = logs[i].find("category");
					if(!id || !id->isInteger() || static_cast<U64>(id->asInteger()) <= afterId) continue;
					if(!level.empty() && (!entryLevel || entryLevel->asString() != level)) continue;
					if(!category.empty() && (!entryCategory || entryCategory->asString() != category)) continue;
					result.push_back(logs[i]);
				}
				return AgentValue::Object{{"entries", std::move(result)}};
			}, AgentValue::Object{{"type", "object"}, {"properties", AgentValue::Object{
				{"afterId", AgentValue::Object{{"type", "integer"}}},
				{"level", AgentValue::Object{{"type", "string"}}},
				{"category", AgentValue::Object{{"type", "string"}}},
				{"limit", AgentValue::Object{{"type", "integer"}}}}}});

		add("profile.start", "Starts bounded frame metric capture",
			[this](const AgentCommandContext&, const AgentValue&)
			{
				std::lock_guard lock(stateMutex);
				profileFrames.clear();
				methodLatencies.clear();
				profiling = true;
				return AgentValue::Object{{"profiling", true}};
			}, emptySchema);

		add("profile.stop", "Stops frame metric capture",
			[this](const AgentCommandContext&, const AgentValue&)
			{
				std::lock_guard lock(stateMutex);
				profiling = false;
				return AgentValue::Object{
					{"profiling", false},
					{"frameCount", static_cast<U64>(profileFrames.size())}
				};
			}, emptySchema);

		add("profile.read", "Reads captured frame metrics",
			[this](const AgentCommandContext&, const AgentValue& params) -> AgentValue
			{
				U16 windowId = 0;
				Bool filterWindow = false;
				if(const auto* value = params.find("windowId"); value && value->isInteger()
					&& value->asInteger() >= 0 && value->asInteger() <= std::numeric_limits<U16>::max())
				{
					windowId = static_cast<U16>(value->asInteger());
					filterWindow = true;
				}
				AgentValue::Array frames;
				Vector<Double> frameTimes;
				Double layoutTotal = 0.0;
				Double updateTotal = 0.0;
				Double renderTotal = 0.0;
				Double finalizeTotal = 0.0;
				std::lock_guard lock(stateMutex);
				for(const auto& frame : profileFrames)
				{
					if(filterWindow && frame.windowId != windowId) continue;
					frames.push_back(frameToValue(frame));
					frameTimes.push_back(frame.frameMilliseconds);
					layoutTotal += frame.layoutMilliseconds;
					updateTotal += frame.updateMilliseconds;
					renderTotal += frame.renderMilliseconds;
					finalizeTotal += frame.finalizeMilliseconds;
				}
				AgentValue::Object summary{{"count", static_cast<U64>(frameTimes.size())}};
				if(!frameTimes.empty())
				{
					std::sort(frameTimes.begin(), frameTimes.end());
					Double total = 0.0;
					for(const Double value : frameTimes) total += value;
					const auto percentile = [&frameTimes](const Double amount)
					{
						const SizeT index = static_cast<SizeT>(
							amount * static_cast<Double>(frameTimes.size() - 1));
						return frameTimes[index];
					};
					summary["minMs"] = frameTimes.front();
					summary["meanMs"] = total / static_cast<Double>(frameTimes.size());
					summary["medianMs"] = percentile(0.5);
					summary["p95Ms"] = percentile(0.95);
					summary["maxMs"] = frameTimes.back();
					summary["meanFps"] = total > 0.0
						? 1000.0 * static_cast<Double>(frameTimes.size()) / total : 0.0;
					summary["meanLayoutMs"] = layoutTotal / static_cast<Double>(frameTimes.size());
					summary["meanUpdateMs"] = updateTotal / static_cast<Double>(frameTimes.size());
					summary["meanRenderMs"] = renderTotal / static_cast<Double>(frameTimes.size());
					summary["meanFinalizeMs"] = finalizeTotal / static_cast<Double>(frameTimes.size());
				}
				AgentValue::Object methods;
				for(const auto& [method, samples] : methodLatencies)
				{
					if(samples.empty()) continue;
					Vector<Double> sorted(samples.begin(), samples.end());
					std::sort(sorted.begin(), sorted.end());
					Double total = 0.0;
					for(const Double sample : sorted) total += sample;
					methods[method] = AgentValue::Object{
						{"count", static_cast<U64>(sorted.size())},
						{"meanMs", total / static_cast<Double>(sorted.size())},
						{"p95Ms", sorted[static_cast<SizeT>(0.95 * static_cast<Double>(sorted.size() - 1))]},
						{"maxMs", sorted.back()}
					};
				}
				AgentValue report = AgentValue::Object{
					{"profiling", profiling},
					{"summary", std::move(summary)},
					{"methods", std::move(methods)},
					{"frames", std::move(frames)}
				};
				if(const AgentValue* path = params.find("path"); path && path->isString())
				{
					const Path outputPath = validatedArtifactPath(path->asString(), "profile.json");
					if(outputPath.extension() != ".json")
						throw std::runtime_error("Profile artifact path must end in .json");
					std::filesystem::create_directories(outputPath.parent_path());
					const String json = serializeAgentJson(report,
						AgentJsonLimits{16 * 1024 * 1024, 64, 500000, 16 * 1024 * 1024});
					std::ofstream output(outputPath, std::ios::binary | std::ios::trunc);
					output.write(json.data(), static_cast<std::streamsize>(json.size()));
					if(!output) throw std::runtime_error("Could not write profile artifact");
					return AgentValue::Object{
						{"path", outputPath.string()},
						{"bytes", static_cast<U64>(json.size())},
						{"summary", report.find("summary") ? *report.find("summary") : AgentValue()}
					};
				}
				return report;
			}, AgentValue::Object{{"type", "object"}, {"properties", AgentValue::Object{
				{"windowId", AgentValue::Object{{"type", "integer"}}}}}});
	}

	Vector<AgentCommandDescription> descriptions() const
	{
		Vector<AgentCommandDescription> result;
		std::lock_guard lock(stateMutex);
		result.reserve(commands.size());
		for(const auto& [name, command] : commands)
		{
			(void)name;
			result.push_back(command.description);
		}
		std::sort(result.begin(), result.end(), [](const auto& left, const auto& right)
		{
			return left.method < right.method;
		});
		return result;
	}

	void acceptNext()
	{
		if(!acceptor || !running.load()) return;
		acceptor->async_accept([this](const asio::error_code& error, asio::ip::tcp::socket socket)
		{
			if(!error) std::make_shared<Session>(*this, std::move(socket))->start();
			if(running.load()) acceptNext();
		});
	}

	void complete(const SharedPointer<Pending>& pending, AgentValue response)
	{
		if(pending->completed.exchange(true)) return;
		if(pending->timer && io)
		{
			const auto timer = pending->timer;
			asio::post(*io, [timer]()
			{
				asio::error_code ignored;
				timer->cancel(ignored);
			});
		}
		pending->session->send(response);
	}

	void dispatch(const SharedPointer<Session>& session, const AgentValue& request)
	{
		AgentValue id;
		if(!request.isObject())
		{
			session->send(errorEnvelope(id, "invalid_request", "Request must be an object"));
			return;
		}
		if(const auto* value = request.find("id")) id = *value;
		const auto* tokenValue = request.find("token");
		if(!tokenValue || !tokenValue->isString() || !constantTimeEqual(tokenValue->asString(), config.token))
		{
			session->send(errorEnvelope(id, "unauthorized", "Invalid authentication token"));
			return;
		}
		const auto* versionValue = request.find("version");
		if(versionValue && (!versionValue->isInteger() || versionValue->asInteger() != 1))
		{
			session->send(errorEnvelope(id, "unsupported_version", "Only protocol version 1 is supported"));
			return;
		}
		const auto* methodValue = request.find("method");
		if(!methodValue || !methodValue->isString() || methodValue->asString().empty())
		{
			session->send(errorEnvelope(id, "invalid_request", "A string method is required"));
			return;
		}

		String method = methodValue->asString();
		AgentValue params = AgentValue::Object{};
		if(const auto* value = request.find("params")) params = *value;
		if(!params.isObject())
		{
			session->send(errorEnvelope(id, "invalid_params", "Params must be an object"));
			return;
		}
		if(method == "agent.invoke")
		{
			const AgentValue* nestedMethod = params.find("method");
			if(!nestedMethod || !nestedMethod->isString() || nestedMethod->asString() == "agent.invoke")
			{
				session->send(errorEnvelope(id, "invalid_params", "agent.invoke requires a non-recursive method"));
				return;
			}
			method = nestedMethod->asString();
			const AgentValue* nestedParams = params.find("params");
			params = nestedParams ? *nestedParams : AgentValue::Object{};
			if(!params.isObject())
			{
				session->send(errorEnvelope(id, "invalid_params", "Nested params must be an object"));
				return;
			}
		}

		AgentCommand command;
		{
			std::lock_guard lock(stateMutex);
			const auto iterator = commands.find(method);
			if(iterator == commands.end())
			{
				session->send(errorEnvelope(id, "method_not_found", "Unknown method"));
				return;
			}
			command = iterator->second;
		}

		U16 windowId = 0;
		Bool hasWindowId = false;
		const AgentValue* windowValue = request.find("windowId");
		if(!windowValue) windowValue = params.find("windowId");
		if(const auto* value = windowValue)
		{
			if(!value->isInteger() || value->asInteger() < 0
				|| value->asInteger() > std::numeric_limits<U16>::max())
			{
				session->send(errorEnvelope(id, "invalid_request", "windowId must be an unsigned 16-bit integer"));
				return;
			}
			windowId = static_cast<U16>(value->asInteger());
			hasWindowId = true;
		}
		if((command.description.requiresWindow || command.description.affinity == AgentThreadAffinity::Render)
			&& !hasWindowId)
		{
			session->send(errorEnvelope(id, "window_required", "This method requires an explicit windowId"));
			return;
		}

		const U64 sequence = nextSequence.fetch_add(1);
		if(command.description.affinity == AgentThreadAffinity::Io)
		{
			const auto methodStart = std::chrono::steady_clock::now();
			try
			{
				session->send(successEnvelope(id,
					command.handler({sequence, windowId, hasWindowId, nullptr}, params)));
			}
			catch(const std::exception& exception)
			{
				session->send(errorEnvelope(id, "command_failed", exception.what()));
			}
			catch(...)
			{
				session->send(errorEnvelope(id, "command_failed", "Command threw an unknown exception"));
			}
			recordMethodLatency(method, methodStart);
			return;
		}

		auto pending = std::make_shared<Pending>();
		pending->sequence = sequence;
		pending->windowId = windowId;
		pending->hasWindowId = hasWindowId;
		pending->id = id;
		pending->params = std::move(params);
		pending->command = std::move(command);
		if(method == "agent.waitFrames")
		{
			const AgentValue* frames = pending->params.find("frames");
			if(!frames || !frames->isInteger() || frames->asInteger() < 1 || frames->asInteger() > 100000)
			{
				session->send(errorEnvelope(id, "invalid_params", "frames must be between 1 and 100000"));
				return;
			}
			pending->remainingFrames = static_cast<UInt>(frames->asInteger());
		}
		pending->session = session;
		UInt timeoutMilliseconds = config.commandTimeoutMilliseconds;
		if(const AgentValue* timeout = request.find("timeoutMs"); timeout && timeout->isInteger())
		{
			if(timeout->asInteger() < 1 || timeout->asInteger() > 60000)
			{
				session->send(errorEnvelope(id, "invalid_request", "timeoutMs must be between 1 and 60000"));
				return;
			}
			timeoutMilliseconds = static_cast<UInt>(timeout->asInteger());
		}
		pending->timer = std::make_shared<asio::steady_timer>(*io,
			std::chrono::milliseconds(timeoutMilliseconds));

		{
			std::lock_guard lock(stateMutex);
			auto& queue = pending->command.description.affinity == AgentThreadAffinity::Main
				? mainQueue : renderQueue;
			const SizeT limit = pending->command.description.affinity == AgentThreadAffinity::Main
				? config.maxQueuedMainCommands : config.maxQueuedRenderCommands;
			while(!queue.empty() && queue.front()->completed.load()) queue.pop_front();
			if(queue.size() >= limit)
			{
				session->send(errorEnvelope(id, "queue_full", "Target thread command queue is full"));
				return;
			}
			queue.push_back(pending);
		}

		pending->timer->async_wait([this, pending](const asio::error_code& error)
		{
			if(!error) complete(pending,
				errorEnvelope(pending->id, "timeout", "Command timed out before completion"));
		});
	}

	SizeT drain(std::deque<SharedPointer<Pending>>& queue, const SizeT maximum, Window* window)
	{
		std::lock_guard drainLock(drainMutex);
		if(!running.load()) return 0;
		Vector<SharedPointer<Pending>> work;
		{
			std::lock_guard lock(stateMutex);
			for(auto iterator = queue.begin(); iterator != queue.end() && work.size() < maximum;)
			{
				const auto& pending = *iterator;
				const Bool matches = !window || (pending->hasWindowId && pending->windowId == window->id);
				if(matches && window && pending->remainingFrames > 1 && !pending->completed.load())
				{
					--pending->remainingFrames;
					++iterator;
					continue;
				}
				if(pending->completed.load() || matches)
				{
					if(matches && !pending->completed.load()) work.push_back(pending);
					iterator = queue.erase(iterator);
				}
				else ++iterator;
			}
		}

		for(const auto& pending : work)
		{
			const auto methodStart = std::chrono::steady_clock::now();
			AgentValue response;
			try
			{
				response = successEnvelope(pending->id, pending->command.handler(
					{pending->sequence, pending->windowId, pending->hasWindowId, window}, pending->params));
			}
			catch(const std::exception& exception)
			{
				response = errorEnvelope(pending->id, "command_failed", exception.what());
			}
			catch(...)
			{
				response = errorEnvelope(pending->id, "command_failed", "Command threw an unknown exception");
			}
			recordMethodLatency(pending->command.description.method, methodStart);
			complete(pending, std::move(response));
		}
		return work.size();
	}
};

AgentBridge agentBridge;

AgentBridge::AgentBridge() : implementation(std::make_unique<Implementation>())
{
	implementation->installBuiltins();
}
AgentBridge::~AgentBridge() { stop(); }

Bool AgentBridge::start(const AgentBridgeConfig& requestedConfig)
{
	if(implementation->running.load()) return false;
	if(requestedConfig.maxFrameBytes == 0 || requestedConfig.maxFrameBytes > hardMaximumFrameBytes
		|| requestedConfig.maxQueuedMainCommands == 0 || requestedConfig.maxQueuedRenderCommands == 0
		|| requestedConfig.maxLogEntries == 0 || requestedConfig.maxProfileFrames == 0
		|| requestedConfig.commandTimeoutMilliseconds == 0)
		return false;

	implementation->config = requestedConfig;
	if(implementation->config.token.empty()) implementation->config.token = randomToken();
	try
	{
		implementation->io = std::make_unique<NetworkingContext>();
		implementation->acceptor = std::make_unique<asio::ip::tcp::acceptor>(*implementation->io);
		const asio::ip::tcp::endpoint endpoint(asio::ip::make_address_v4("127.0.0.1"), requestedConfig.port);
		implementation->acceptor->open(endpoint.protocol());
		implementation->acceptor->set_option(asio::socket_base::reuse_address(true));
		implementation->acceptor->bind(endpoint);
		implementation->acceptor->listen(asio::socket_base::max_listen_connections);
		implementation->boundPort = implementation->acceptor->local_endpoint().port();
		implementation->running.store(true);
		implementation->acceptNext();
		implementation->ioThread = Thread([this]()
		{
			implementation->io->run();
		});
		setDebugLogSink([this](const String& level, const String& category, const String& message)
		{
			AgentLogLevel parsedLevel = AgentLogLevel::Info;
			if(level == "debug") parsedLevel = AgentLogLevel::Debug;
			else if(level == "warning") parsedLevel = AgentLogLevel::Warning;
			else if(level == "error") parsedLevel = AgentLogLevel::Error;
			writeLog(parsedLevel, category, message);
		});
		return true;
	}
	catch(...)
	{
		implementation->running.store(false);
		implementation->acceptor.reset();
		implementation->io.reset();
		implementation->boundPort = 0;
		return false;
	}
}

void AgentBridge::stop()
{
	setDebugLogSink({});
	std::lock_guard drainLock(implementation->drainMutex);
	if(!implementation->running.exchange(false)) return;
	if(implementation->acceptor)
	{
		asio::error_code ignored;
		implementation->acceptor->close(ignored);
	}
	{
		std::lock_guard lock(implementation->stateMutex);
		for(auto& pending : implementation->mainQueue) pending->completed.store(true);
		for(auto& pending : implementation->renderQueue) pending->completed.store(true);
		implementation->mainQueue.clear();
		implementation->renderQueue.clear();
	}
	if(implementation->io) implementation->io->stop();
	if(implementation->ioThread.joinable() && implementation->ioThread.get_id() != std::this_thread::get_id())
		implementation->ioThread.join();
	implementation->acceptor.reset();
	implementation->io.reset();
	implementation->boundPort = 0;
	implementation->config.token.clear();
}

Bool AgentBridge::isRunning() const { return implementation->running.load(); }
UShort AgentBridge::port() const { return implementation->boundPort; }
String AgentBridge::token() const { return implementation->config.token; }

SizeT AgentBridge::drainMain(const SizeT maxCommands)
{
	return implementation->drain(implementation->mainQueue, maxCommands, nullptr);
}

SizeT AgentBridge::drainRender(Window& window, const SizeT maxCommands)
{
	return implementation->drain(implementation->renderQueue, maxCommands, &window);
}

void AgentBridge::recordFrame(const AgentFrameMetrics& suppliedMetrics)
{
	AgentFrameMetrics metrics = suppliedMetrics;
	if(metrics.timestampMicroseconds == 0) metrics.timestampMicroseconds = nowMicroseconds();
	std::lock_guard lock(implementation->stateMutex);
	if(!implementation->profiling) return;
	implementation->profileFrames.push_back(metrics);
	while(implementation->profileFrames.size() > implementation->config.maxProfileFrames)
		implementation->profileFrames.pop_front();
}

void AgentBridge::writeLog(const AgentLogLevel level, const String& category, const String& message,
	const AgentValue::Object& fields)
{
	const AgentValue entry = AgentValue::Object{
		{"id", implementation->nextLogId.fetch_add(1)},
		{"timestampUs", nowMicroseconds()},
		{"level", logLevelName(level)},
		{"category", category},
		{"message", message},
		{"fields", fields}
	};
	std::lock_guard lock(implementation->stateMutex);
	implementation->logs.push_back(entry);
	while(implementation->logs.size() > implementation->config.maxLogEntries)
		implementation->logs.pop_front();
}

Bool AgentBridge::registerCommand(const AgentCommand& command, String* error)
{
	if(command.description.method.empty() || !command.handler)
	{
		if(error) *error = "Command requires a method and handler";
		return false;
	}
	for(const unsigned char character : command.description.method)
	{
		const Bool valid = std::isalnum(character) || character == '.' || character == '_' || character == '-';
		if(!valid)
		{
			if(error) *error = "Method contains an invalid character";
			return false;
		}
	}
	std::lock_guard lock(implementation->stateMutex);
	if(implementation->commands.contains(command.description.method))
	{
		if(error) *error = "Method is already registered";
		return false;
	}
	implementation->commands.emplace(command.description.method, command);
	if(error) error->clear();
	return true;
}

Bool AgentBridge::unregisterCommand(const String& method)
{
	std::lock_guard lock(implementation->stateMutex);
	return implementation->commands.erase(method) != 0;
}

Vector<AgentCommandDescription> AgentBridge::listMethods() const
{
	return implementation->descriptions();
}

