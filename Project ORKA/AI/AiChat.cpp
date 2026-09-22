#include "AiChat.hpp"

#include "../Debug.hpp"
#include "../ResourceManager.hpp"
#include "../Settings.hpp"
#include "../UIElement.hpp"
#include "../UserInterface.hpp"
#include "../Window.hpp"
#include "../WindowAPI.hpp"
#include "../UI/Authoring/UiAuthoredHost.hpp"
#include "../UI/Theme/UiAppHelpers.hpp"
#include "../UI/Theme/UiAppTheme.hpp"
#include "AiConfig.hpp"
#include "AiConsent.hpp"
#include "AiLocalRuntime.hpp"
#include "AiChatAgent.hpp"
#include "AiToolRegistry.hpp"

namespace
{
	using namespace Ui;

	struct AiChatUiMessage
	{
		String id;
		String role;
		String roleLabel;
		String text;
	};

	struct AiChatUiState
	{
		Bool showConsent = true;
		Bool toolsEnabled = true;
		String draft;
		String statusLine = "Waiting for setup…";
		String consentBody;
	};

	UiValue chatMessagesBinding(const Vector<AiChatUiMessage>& messages)
	{
		Vector<UiValue> rows;
		rows.reserve(messages.size());
		for (const AiChatUiMessage& message : messages)
		{
			rows.push_back(UiValue::fromObject({
				{"id", UiValue::fromString(message.id)},
				{"roleLabel", UiValue::fromString(message.roleLabel)},
				{"text", UiValue::fromString(message.text)}
				}));
		}
		return UiValue::fromArray(std::move(rows));
	}

	String buildConsentBody()
	{
		String body =
			"The engine uses " + String(AiConfig::runtimeProviderName) +
			" to download and run AI models locally on your machine.\n\n"
			"By clicking \"Yes, install and continue\" you accept that ORKA may:\n"
			"• Download open-source inference binaries from GitHub (llama.cpp; CUDA build if NVIDIA is detected)\n"
			"• Download Qwen3.5 9B GGUF weights from Hugging Face (via llama-server)\n"
			"• Store files under your ORKA cache folder and start a local server on 127.0.0.1:"
			+ toString(static_cast<Int>(AiConfig::defaultServerPort)) + "\n\n"
			"Default model: " + String(AiConfig::defaultModelDisplayName) +
			". Enabled tools can read/list/edit text files under Project ORKA.\n\n"
			"Are you sure?";

		const Vector<AiToolDescriptor> tools = aiRegisteredTools();
		if (!tools.empty())
		{
			body += "\n\nTools:";
			for (const AiToolDescriptor& tool : tools)
				body += "\n• " + tool.name + (tool.implemented ? "" : " (planned)") + " — " + tool.description;
		}
		return body;
	}

	void publishState(UiPropertyRegistry& properties, const AiChatUiState& state, const Vector<AiChatUiMessage>& messages)
	{
		(void)properties.set("state.showConsent", UiValue::fromBool(state.showConsent));
		(void)properties.set("state.toolsEnabled", UiValue::fromBool(state.toolsEnabled));
		(void)properties.set("state.draft", UiValue::fromString(state.draft));
		(void)properties.set("state.statusLine", UiValue::fromString(state.statusLine));
		(void)properties.set("state.consentBody", UiValue::fromString(state.consentBody));
		(void)properties.set("chat.messages", chatMessagesBinding(messages));
	}

	void publishUiCopy(UiPropertyRegistry& properties, const UiAppTheme& theme)
	{
		(void)properties.set("ui.themeToggleLabel",
			UiValue::fromString(theme.getMode() == UiThemeMode::Light ? "Dark mode" : "Light mode"));
		(void)properties.set("ui.subtitle",
			UiValue::fromString("Baseline local chat for NPCs, tools, and future voice — powered by Qwen-class models."));
	}

	struct AiChatSurface final : UIElement
	{
		UiAppTheme theme = UiAppTheme::light();
		UiHost host;
		UniquePointer<UiAuthoredHost> authored;
		UniquePointer<AiLocalRuntime> runtime;
		AiChatUiState uiState;
		Vector<AiChatUiMessage> messages;
		Vector<AiChatMessage> transcript;
		Int nextMessageId = 0;
		Int nextTabIndex = 0;
		Bool authoredReady = false;
		Bool chatBusy = false;
		Window* owningWindow = nullptr;
		Thread chatWorker;
		Atomic<Bool> chatWorkerRunning{false};

		AiChatSurface()
		{
			addElement(host);
			growX(true);
			growY(true);
			background(Color(0, 0, 0, 0));
			uiState.consentBody = buildConsentBody();
			uiState.showConsent = !hasValidAiConsent();
		}

		void refreshBoundState()
		{
			if (!authored) return;
			LockGuard lock(mutex);
			publishUiCopy(authored->properties(), theme);
			publishState(authored->properties(), uiState, messages);
			authored->syncFromProperties();
		}

		void applyTheme()
		{
			host.useAppTheme(theme);
			if (!authoredReady) return;
			refreshBoundState();
		}

		Mutex mutex;

		void appendMessage(const String& role, const String& roleLabel, const String& body)
		{
			LockGuard lock(mutex);
			AiChatUiMessage row;
			row.id = "msg-" + toString(nextMessageId++);
			row.role = role;
			row.roleLabel = roleLabel;
			row.text = body;
			messages.push_back(std::move(row));
			AiChatMessage entry;
			entry.role = role;
			entry.content = body;
			transcript.push_back(std::move(entry));
		}

		void pollRuntimeStatus()
		{
			if (!runtime || uiState.showConsent) return;
			const AiRuntimeSnapshot snapshot = runtime->snapshot();
			switch (snapshot.phase)
			{
			case AiRuntimePhase::DownloadingRuntime:
			case AiRuntimePhase::DownloadingModel:
				uiState.statusLine = snapshot.statusLine + " "
					+ toString(static_cast<Int>(snapshot.downloadProgress * 100.0)) + "%";
				break;
			case AiRuntimePhase::Ready:
				uiState.statusLine = chatBusy ? "Model is replying…" : snapshot.statusLine;
				break;
			case AiRuntimePhase::Error:
				uiState.statusLine = snapshot.error.empty() ? snapshot.statusLine : snapshot.error;
				break;
			default:
				uiState.statusLine = snapshot.statusLine;
				break;
			}
		}

		void beginSetupAfterConsent()
		{
			uiState.showConsent = false;
			uiState.statusLine = "Preparing local AI runtime…";
			runtime = std::make_unique<AiLocalRuntime>();
			runtime->ensureReadyAsync();
			appendMessage("assistant", "Assistant", "Thanks — I'll set up the local model. Large downloads can take a while on first launch.");
			refreshBoundState();
		}

		void sendDraft()
		{
			const String trimmed = uiState.draft;
			if (trimmed.empty() || chatBusy || uiState.showConsent) return;

			const AiRuntimeSnapshot snapshot = runtime ? runtime->snapshot() : AiRuntimeSnapshot{};
			if (snapshot.phase != AiRuntimePhase::Ready)
			{
				uiState.statusLine = "Wait until the local server status is Ready.";
				refreshBoundState();
				return;
			}

			appendMessage("user", "You", trimmed);
			uiState.draft.clear();
			chatBusy = true;
			refreshBoundState();

			if (chatWorker.joinable()) chatWorker.join();
			chatWorkerRunning.store(true);
			const Vector<AiChatMessage> requestMessages = transcript;
			const Bool toolsEnabled = uiState.toolsEnabled;
			chatWorker = Thread([this, requestMessages, toolsEnabled]()
				{
					AiOpenAiClientConfig config;
					const String systemPrompt =
						"You are a helpful assistant embedded in Project ORKA, a C++ game engine. "
						"Keep answers concise unless the user asks for detail. "
						"When tools are enabled, use read_file/list_directory/write_file for workspace tasks. "
						"Only modify files when the user explicitly asks.";
					const AiChatAgentResult result = runAiChatAgent(config, requestMessages, systemPrompt, toolsEnabled);

					for (const String& trace : result.toolTrace)
						appendMessage("tool", "Tool", trace);
					if (result.succeeded)
						appendMessage("assistant", "Assistant", result.assistantText);
					else
						appendMessage("assistant", "Assistant", "Error: " + result.error);
					chatBusy = false;
					chatWorkerRunning.store(false);
				});
		}

		void create(Window& window) override
		{
			owningWindow = &window;
			const IVec2 size = window.getSize();
			setWidth(size.x);
			setHeight(size.y);
			host.setWidth(size.x);
			host.setHeight(size.y);

			host.useAppTheme(theme);
			UIElement::create(window);

			authored = std::make_unique<UiAuthoredHost>(host);

			(void)authored->commands().registerCommand("AiChat.ToggleTheme", {},
				[this](const Vector<UiValue>&)
				{
					theme.setMode(theme.getMode() == UiThemeMode::Light ? UiThemeMode::Dark : UiThemeMode::Light);
					applyTheme();
					return true;
				});
			(void)authored->commands().registerCommand("AiChat.AcceptConsent", {},
				[this](const Vector<UiValue>&)
				{
					if (!acceptAiConsent())
					{
						uiState.statusLine = "Could not save consent record.";
						refreshBoundState();
						return true;
					}
					beginSetupAfterConsent();
					return true;
				});
			(void)authored->commands().registerCommand("AiChat.DeclineConsent", {},
				[this](const Vector<UiValue>&)
				{
					if (owningWindow)
					{
						owningWindow->keepRunning = false;
						apiSetWindowShouldClose(owningWindow->apiWindow, true);
					}
					return true;
				});
			(void)authored->commands().registerCommand("AiChat.Send", {},
				[this](const Vector<UiValue>&)
				{
					sendDraft();
					return true;
				});

			const Path uiDirectory = resourceManager.getOrkaDataPath() / "UI";
			if (!authored->loadFromFiles(uiDirectory / "ai-chat.ui.html", uiDirectory / "ai-chat.ui.scss"))
			{
				logError("AiChat: failed to load authored UI bundle.");
				for (const UiDiagnostic& diagnostic : authored->diagnostics())
					logError("  " + diagnostic.message);
				return;
			}

			authored->widgetBridge().setTabIndexAllocator([this]() { return nextTabIndex++; });
			authored->widgetBridge().setPropertyChangeHandler(
				[this](const String& path, const UiValue& value)
				{
					if (path == "state.draft" && value.type == UiValueType::String)
						uiState.draft = value.string;
					else if (path == "state.toolsEnabled" && value.type == UiValueType::Bool)
						uiState.toolsEnabled = value.boolean;
				});

			if (!authored->mount())
			{
				logError("AiChat: failed to mount authored UI.");
				return;
			}

			authored->ensureNodeLayout("ai-chat-root", [](UiLayoutStyle& style)
				{
					style.width = UiSizeSpec::percent(1);
					style.height = UiSizeSpec::percent(1);
				});
			authored->ensureNodeLayout("ai-chat-shell", [](UiLayoutStyle& style)
				{
					style.width = UiSizeSpec::percent(1);
					style.height = UiSizeSpec::percent(1);
				});
			authored->ensureNodeLayout("ai-chat-main", [](UiLayoutStyle& style)
				{
					style.flexGrow = 1;
					style.flexShrink = 1;
				});
			authored->ensureNodeLayout("ai-chat-scroll", [](UiLayoutStyle& style)
				{
					style.flexGrow = 1;
					style.flexShrink = 1;
				});

			authoredReady = true;

			if (!uiState.showConsent)
			{
				runtime = std::make_unique<AiLocalRuntime>();
				runtime->ensureReadyAsync();
				appendMessage("assistant", "Assistant", "Reconnecting to your local model server…");
			}

			refreshBoundState();
		}

		void update(Window& window) override
		{
			const IVec2 size = window.getSize();
			setWidth(size.x);
			setHeight(size.y);
			host.setWidth(size.x);
			host.setHeight(size.y);

			pollRuntimeStatus();
			if (authoredReady)
			{
				if (!chatWorkerRunning.load() && chatBusy && chatWorker.joinable())
				{
					chatWorker.join();
					chatBusy = false;
				}
				refreshBoundState();
				authored->syncControlLayouts();
			}
			host.update(window);
		}

		void destroy(Window& window) override
		{
			if (chatWorker.joinable()) chatWorker.join();
			if (runtime) runtime->shutdown();
			runtime.reset();
			authored.reset();
			authoredReady = false;
			host.destroy(window);
		}
	};
}

void AiChat::run() const
{
	AiChatSurface surface;
	Window& window = ui.window("ORKA AI Chat", settings.defaultWindowWidth,
		settings.defaultWindowHeight, true, true, WindowState::Windowed);
	window.addElement(surface);
	ui.run();
}
