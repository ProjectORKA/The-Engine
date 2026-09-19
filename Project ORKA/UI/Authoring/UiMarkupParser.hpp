#pragma once

#include "UiDiagnostics.hpp"

namespace Ui
{
	enum class UiMarkupNodeKind : Byte
	{
		Document,
		Element,
		Component,
		Text,
		Comment,
		ForBlock,
		IfBlock,
	};

	enum class UiAttributeKind : Byte
	{
		Literal,
		PropertyBinding,
		EventBinding,
		IfDirective,
		ForDirective,
	};

	struct UiMarkupAttribute
	{
		UiAttributeKind kind = UiAttributeKind::Literal;
		String name;
		String value;
		SourceSpan span;
		SourceSpan nameSpan;
		SourceSpan valueSpan;
	};

	struct UiMarkupNode
	{
		UiMarkupNodeKind kind = UiMarkupNodeKind::Document;
		String name;
		String text;
		String id;
		Vector<String> classes;
		Vector<UiMarkupAttribute> attributes;
		Vector<UInt> children;
		SourceSpan span;
	};

	struct UiMarkupDocument
	{
		Vector<UiMarkupNode> nodes;
		UInt rootIndex = 0;
	};

	struct UiTemplateBudgets
	{
		SizeT maximumSourceBytes = 1024 * 1024;
		SizeT maximumNodes = 16384;
		SizeT maximumAttributesPerElement = 64;
		SizeT maximumDepth = 128;
		SizeT maximumNameBytes = 128;
		SizeT maximumAttributeValueBytes = 16384;
		SizeT maximumTextNodeBytes = 65536;
		SizeT maximumTotalTextBytes = 1024 * 1024;
		SizeT maximumExpressions = 8192;
		SizeT maximumExpressionSourceBytes = 1024 * 1024;
		SizeT maximumBindings = 16384;
	};

	struct UiMarkupParseResult
	{
		UiMarkupDocument document;
		Vector<UiDiagnostic> diagnostics;
		Bool valid = false;
	};

	class UiMarkupParser
	{
	public:
		static constexpr UiTemplateBudgets defaultBudgets{};

		[[nodiscard]] static UiMarkupParseResult parse(
			const String& source,
			const String& sourceName = {},
			const UiTemplateBudgets& budgets = defaultBudgets);
	};
}
