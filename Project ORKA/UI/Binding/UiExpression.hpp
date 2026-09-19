#pragma once

#include "UiBindingRegistry.hpp"

namespace Ui
{
	enum class UiOpCode : Byte
	{
		PushConstant,
		LoadProperty,
		Negate,
		Not,
		Add,
		Subtract,
		Multiply,
		Divide,
		Modulo,
		Equal,
		NotEqual,
		Less,
		LessEqual,
		Greater,
		GreaterEqual,
		And,
		Or,
		NullCoalesce,
		ToBool,
		JumpIfFalse,
		JumpIfTrue,
		JumpIfNotNull,
	};

	struct UiInstruction
	{
		UiOpCode code = UiOpCode::PushConstant;
		UInt operand = 0;
	};

	struct UiExpressionProgram
	{
		Vector<UiInstruction> instructions;
		Vector<UiValue> constants;
		Vector<String> properties;
		SourceSpan span;
	};

	struct UiExpressionCompileResult
	{
		UiExpressionProgram program;
		Vector<UiDiagnostic> diagnostics;
		Bool valid = false;
	};

	struct UiExpressionResult
	{
		UiValue value;
		String error;
		Bool valid = false;
	};

	class UiExpressionCompiler
	{
	public:
		static constexpr SizeT maximumSourceBytes = 4096;
		static constexpr SizeT maximumTokens = 512;
		static constexpr SizeT maximumInstructions = 1024;
		static constexpr SizeT maximumNestingDepth = 64;
		static constexpr SizeT maximumConstants = 512;
		static constexpr SizeT maximumProperties = 512;
		static constexpr SizeT maximumConstantStringBytes = UiBindingLimits::maximumValueStringBytes;

		[[nodiscard]] static UiExpressionCompileResult compile(const String& source, const String& sourceName = {});
	};

	class UiExpressionVm
	{
	public:
		static constexpr SizeT maximumStack = 256;
		static constexpr SizeT defaultInstructionBudget = 2048;
		static constexpr SizeT maximumResultStringBytes = UiBindingLimits::maximumValueStringBytes;

		[[nodiscard]] static UiExpressionResult evaluate(
			const UiExpressionProgram& program,
			const UiPropertyRegistry& properties,
			SizeT instructionBudget = defaultInstructionBudget);
	};
}
