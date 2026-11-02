#pragma once

#include "UiMarkupParser.hpp"
#include "../Binding/UiExpression.hpp"
#include "../Style/UiStyleCompiler.hpp"

namespace Ui
{
	struct UiBundleSources
	{
		String markup;
		String markupName;
		Path markupRoot;
		String style;
		String styleName;
		UiStyleCompileOptions styleOptions;
	};

	struct UiCompiledBinding
	{
		UInt nodeIndex = 0;
		UiAttributeKind kind = UiAttributeKind::PropertyBinding;
		String target;
		UiExpressionProgram expression;
	};

	struct UiCompiledForBinding
	{
		UInt nodeIndex = 0;
		String itemName;
		UiExpressionProgram collection;
		UiExpressionProgram key;
	};

	struct UiCompiledEventBinding
	{
		UInt nodeIndex = 0;
		String eventName;
		String commandName;
		U64 commandVersion = 0;
	};

	struct UiCompiledBundle
	{
		U64 generation = 0;
		UiMarkupDocument blueprint;
		Vector<UiCompiledBinding> bindings;
		Vector<UiCompiledForBinding> loops;
		Vector<UiCompiledEventBinding> events;
		UiCompiledStyleSheet styles;
		Vector<UiDiagnostic> diagnostics;
		Bool valid = false;
	};

	struct UiBundleLoadResult
	{
		UiBundleSources sources;
		Vector<UiDiagnostic> diagnostics;
		Bool valid = false;
	};

	class UiBundleCompiler
	{
	public:
		[[nodiscard]] static UiCompiledBundle compile(
			U64 generation,
			const UiBundleSources& sources,
			const UiCommandRegistrySnapshot& commands = {},
			const UiTemplateBudgets& budgets = UiMarkupParser::defaultBudgets);

		// Pure bounded file loading; safe to call on a worker before compile().
		[[nodiscard]] static UiBundleLoadResult loadFile(
			const Path& markupPath,
			const Path& stylePath,
			const UiTemplateBudgets& budgets = UiMarkupParser::defaultBudgets);
	};

	class UiBundleManager
	{
	public:
		[[nodiscard]] U64 requestGeneration();
		[[nodiscard]] Bool publish(SharedPointer<const UiCompiledBundle> candidate);
		[[nodiscard]] SharedPointer<const UiCompiledBundle> activeBundle() const;
		[[nodiscard]] U64 requestedGeneration() const;

	private:
		mutable Mutex mutex;
		U64 requested = 0;
		SharedPointer<const UiCompiledBundle> active;
	};
}
