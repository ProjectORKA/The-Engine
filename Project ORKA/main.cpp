//	Welcome to Project ORKA!
//	My name is Sam and im the creator of Project ORKA.
//	This is the open source code for the engine, systems and some projects build with Project ORKA
//	Feel free to take a look at the code, play around

//	If you want to report bugs, request features or add your own code take a look at our GitHub:
//	https://github.com/ProjectORKA/Project-ORKA

//	If you want to know what Project ORKA is all about please visit:
//	https://project-orka.com

//  If you want to chat with us or ask questions here's a Discord link, were always happy to help:
//  https://discord.gg/NUH4bRzqwJ

//	Legal stuff:
//	Project ORKA is licensed under CC BY-NC-SA 4.0 (see LICENSE in the repository root).
//	Modify, hack and tinker around as you please, just give credit, keep it non-commercial
//	and share your changes under the same license. Third-party parts: see THIRD_PARTY.md.
//	If you've got something that you would like to share, you can upload your work to Project ORKA
//  (There's no simple process yet, so if you don't know how to make pull requests, just get in touch with us.)

//	Disclaimer:
//	This engine uses a left-handed coordinate system like Blender: +X = right +Y = forward +Z = up
//  This engine has clear conventions for objects, functions, variables, files, etc.. please try to respect them
//	The following code is meant to be as clean and readable as possible adhering to that standard is advised
//  Base types (float, int, bool) have been overridden as a design choice and to clearly differentiate between data and functions

#include "Engine.hpp"
#include "GameLoader.hpp"
#include "ORKAIntroSequence.hpp"
#include "UI/Testing/UiBenchmarks.hpp"
#include "UI/Testing/UiValidation.hpp"
#include "Agent/AgentBridge.hpp"
#include "UserInterface.hpp"

int main(const int argc, char* argv[])
{
	for (Int i = 1; i < argc; ++i)
	{
		const String argument = argv[i];
		if (argument == "--agent-bridge-test")
		{
			const Bool passed = runAgentBridgeSelfTests();
			std::cout << "Agent bridge self-tests: " << (passed ? "passed" : "failed") << '\n';
			return passed ? 0 : 1;
		}
		if (argument == "--modern-ui-test")
		{
			const Ui::Testing::UiValidationReport report = Ui::Testing::runModernUiValidation();
			std::cout << "Modern UI validation: " << report.passed << " passed, "
				<< report.failed << " failed, " << report.budgetExceeded << " over budget.\n";
			for (const auto& test : report.cases)
			{
				if (test.status == Ui::Testing::UiValidationStatus::Passed) continue;
				std::cout << "  " << test.name << ": " << static_cast<Int>(test.status) << '\n';
				for (const auto& failure : test.failures)
					std::cout << "    " << failure.check << " expected " << failure.expected
						<< ", got " << failure.actual << '\n';
			}
			return report.succeeded() ? 0 : 1;
		}
		if (argument == "--modern-ui-benchmark")
		{
			const Ui::Testing::UiBenchmarkReport report = Ui::Testing::runModernUiBenchmarks();
			for (const auto& sample : report.samples)
				std::cout << sample.name << ": " << sample.medianIterationMicroseconds
					<< " us median (" << sample.operations << " operations)\n";
			return report.completed() ? 0 : 1;
		}
	}

	Engine engine;
	engine.create(argc, argv, false);

	ORKAIntroSequence intro;
	GameLoader       gameLoader;

	intro.init(gameLoader);
	ui.window("Project ORKA", settings.defaultWindowWidth, settings.defaultWindowHeight, true, true, WindowState::Windowed, intro);
	ui.run();

	if (Function<void()> launch = gameLoader.consumeSelectedGame())
	{
		launch();
	}

	return 0;
}