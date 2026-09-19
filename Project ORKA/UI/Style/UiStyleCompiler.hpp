#pragma once

#include "../Authoring/UiDiagnostics.hpp"

namespace Ui
{
	enum class UiStyleUnit : Byte
	{
		None, Px, Dp, Percent, Em, Rem, Vw, Vh, Fr,
	};

	enum class UiStyleValueKind : Byte
	{
		Invalid, Number, Auto, Content, Color, String, Identifier, Expression,
	};

	struct UiStyleValue
	{
		UiStyleValueKind kind = UiStyleValueKind::Invalid;
		Double number = 0.0;
		UiStyleUnit unit = UiStyleUnit::None;
		Color color = Color(0);
		String text;
		Vector<String> tokens;
	};

	enum class UiStyleCombinator : Byte
	{
		None, Descendant, Child,
	};

	enum class UiStylePseudoState : UInt
	{
		None = 0,
		Hover = 1 << 0,
		Active = 1 << 1,
		Focus = 1 << 2,
		Disabled = 1 << 3,
		Checked = 1 << 4,
		Selected = 1 << 5,
	};

	struct UiStyleSelectorPart
	{
		String type;
		String id;
		Vector<String> classes;
		String componentScope;
		UInt pseudoStates = 0;
		UiStyleCombinator combinatorToLeft = UiStyleCombinator::None;
	};

	struct UiStyleSelector
	{
		String text;
		Vector<UiStyleSelectorPart> parts; // Left to right.
		UInt specificity = 0;
	};

	struct UiStyleDeclaration
	{
		String property;
		String value;
		UiStyleValue typedValue;
		SourceSpan span;
	};

	struct UiStyleRule
	{
		String selector;
		UiStyleSelector parsedSelector;
		Vector<UiStyleDeclaration> declarations;
		UInt specificity = 0;
		UInt sourceOrder = 0;
		SourceSpan span;
	};

	struct UiStyleNodeDescriptor
	{
		String type;
		String id;
		Vector<String> classes;
		String componentScope;
		UInt pseudoStates = 0;
		const UiStyleNodeDescriptor* parent = nullptr;
	};

	struct UiCompiledStyleSheet
	{
		Vector<UiStyleRule> rules;
		Map<String, Vector<UInt>> rightmostSelectorIndex;
		Vector<UInt> universalRuleIndex;

		[[nodiscard]] Vector<UInt> matchingRules(const UiStyleNodeDescriptor& node) const;
		[[nodiscard]] Bool matches(const UiStyleRule& rule, const UiStyleNodeDescriptor& node) const;
	};

	struct UiResolvedStyle
	{
		Map<String, UiStyleDeclaration> declarations;
		Map<String, String> customProperties;

		[[nodiscard]] const UiStyleDeclaration* find(const String& property) const;
	};

	struct UiStyleCompileResult
	{
		UiCompiledStyleSheet sheet;
		Vector<UiDiagnostic> diagnostics;
		Bool valid = false;
	};

	struct UiStyleCompileOptions
	{
		// Returning false rejects the import. The callback is the approval boundary.
		Function<Bool(const String& requested, const String& fromSource, String& source, String& sourceName)> importResolver;
		SizeT maximumImportDepth = 16;
		SizeT maximumImportedBytes = 4 * 1024 * 1024;
	};

	struct UiStyleCascadeOptions
	{
		const UiResolvedStyle* parent = nullptr;
		Function<Bool(const String& property)> inherits;
	};

	class UiStyleCompiler
	{
	public:
		static constexpr SizeT maximumSourceBytes = 1024 * 1024;
		static constexpr SizeT maximumRules = 4096;
		static constexpr SizeT maximumDeclarationsPerRule = 64;
		static constexpr SizeT maximumVariables = 256;
		static constexpr SizeT maximumMixins = 128;
		static constexpr SizeT maximumMixinParameters = 16;
		static constexpr SizeT maximumTokens = 131072;
		static constexpr SizeT maximumDepth = 64;

		[[nodiscard]] static UiStyleCompileResult compile(const String& source, const String& sourceName = {});
		[[nodiscard]] static UiStyleCompileResult compile(
			const String& source, const String& sourceName, const UiStyleCompileOptions& options);
		[[nodiscard]] static UiResolvedStyle resolve(
			const UiCompiledStyleSheet& sheet, const UiStyleNodeDescriptor& node,
			const UiStyleCascadeOptions& options = {});
	};
}
