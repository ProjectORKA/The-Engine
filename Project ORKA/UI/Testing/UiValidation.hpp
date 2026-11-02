#pragma once

#include "../Authoring/UiBundleManager.hpp"
#include "../Authoring/UiStyleApplicationAdapter.hpp"
#include "../Authoring/UiTemplateInstantiator.hpp"
#include "../Input/UiEventRouter.hpp"
#include "../Input/UiTextEditModel.hpp"
#include "../Layout/UiLayoutEngine.hpp"
#include "../Layout/UiScroll.hpp"
#include "../Render/UiDisplayList.hpp"
#include "../Style/UiStyleCompiler.hpp"
#include "../Text/UiGlyphCache.hpp"
#include "../Text/UiFontService.hpp"
#include "../Text/UiTextLayout.hpp"
#include "../Text/UiTextPainter.hpp"

namespace Ui::Testing
{
	enum class UiValidationStatus : Byte
	{
		Passed,
		Failed,
		Skipped,
		BudgetExceeded,
	};

	struct UiValidationFailure
	{
		String check;
		String expected;
		String actual;
	};

	struct UiValidationCaseResult
	{
		String name;
		UiValidationStatus status = UiValidationStatus::Passed;
		U64 seed = 0;
		U64 operations = 0;
		U64 elapsedMicroseconds = 0;
		Vector<UiValidationFailure> failures;
		Vector<String> notes;
	};

	struct UiValidationConfig
	{
		U64 seed = 0x4f524b415549ULL;
		U64 maximumOperationsPerCase = 250000;
		U64 maximumMicrosecondsPerCase = 2'000'000;
		Bool includeExpensiveCases = true;
	};

	struct UiValidationReport
	{
		U64 seed = 0;
		UInt passed = 0;
		UInt failed = 0;
		UInt skipped = 0;
		UInt budgetExceeded = 0;
		Vector<UiValidationCaseResult> cases;

		[[nodiscard]] Bool succeeded() const
		{
			return failed == 0 && budgetExceeded == 0;
		}
	};

	// Dependency-free CPU validation. No assertion, window, graphics context,
	// or process-global test registration is used. The component expansion case
	// writes a temporary fragment under the OS temp directory and deletes it.
	[[nodiscard]] UiValidationReport runModernUiValidation(
		const UiValidationConfig& config = {});
}
