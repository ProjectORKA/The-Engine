#include "UiExpression.hpp"

#include <cctype>
#include <cstdlib>

namespace Ui
{
	namespace
	{
		enum class TokenKind : Byte
		{
			End, Value, Property, LeftParen, RightParen,
			Plus, Minus, Star, Slash, Percent, Bang,
			EqualEqual, BangEqual, Less, LessEqual, Greater, GreaterEqual,
			AndAnd, OrOr, QuestionQuestion, Invalid,
		};

		struct Token
		{
			TokenKind kind = TokenKind::Invalid;
			UiValue value;
			String text;
			SourceSpan span;
		};

		SourceSpan makeSpan(const String& sourceName, const String& source, const SizeT offset, const SizeT length)
		{
			SourceSpan span{ sourceName, offset, length, 1, 1 };
			for (SizeT i = 0; i < offset && i < source.size(); ++i)
			{
				if (source[i] == '\n')
				{
					++span.line;
					span.column = 1;
				}
				else ++span.column;
			}
			return span;
		}

		Bool isIdentifierStart(const Char c)
		{
			return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
		}

		Bool isIdentifierPart(const Char c)
		{
			return isIdentifierStart(c) || (c >= '0' && c <= '9');
		}

		Vector<Token> tokenize(const String& source, const String& sourceName, DiagnosticSink& diagnostics)
		{
			Vector<Token> result;
			SizeT cursor = 0;
			const auto addSimple = [&](const TokenKind kind, const SizeT start, const SizeT length)
				{
					result.push_back({ kind, {}, {}, makeSpan(sourceName, source, start, length) });
				};

			while (cursor < source.size() && result.size() < UiExpressionCompiler::maximumTokens)
			{
				if (std::isspace(static_cast<unsigned char>(source[cursor])) != 0)
				{
					++cursor;
					continue;
				}
				const SizeT start = cursor;
				const Char c = source[cursor++];

				if (std::isdigit(static_cast<unsigned char>(c)) != 0 || (c == '.' && cursor < source.size() && std::isdigit(static_cast<unsigned char>(source[cursor])) != 0))
				{
					while (cursor < source.size() && std::isdigit(static_cast<unsigned char>(source[cursor])) != 0) ++cursor;
					if (cursor < source.size() && source[cursor] == '.')
					{
						++cursor;
						while (cursor < source.size() && std::isdigit(static_cast<unsigned char>(source[cursor])) != 0) ++cursor;
					}
					if (cursor < source.size() && (source[cursor] == 'e' || source[cursor] == 'E'))
					{
						++cursor;
						if (cursor < source.size() && (source[cursor] == '+' || source[cursor] == '-')) ++cursor;
						while (cursor < source.size() && std::isdigit(static_cast<unsigned char>(source[cursor])) != 0) ++cursor;
					}
					const String numberText = source.substr(start, cursor - start);
					Char* end = nullptr;
					const Double value = std::strtod(numberText.c_str(), &end);
					if (!end || *end != '\0' || !std::isfinite(value))
						diagnostics.error("Invalid or out-of-range number literal.", makeSpan(sourceName, source, start, cursor - start));
					result.push_back({ TokenKind::Value, UiValue::fromNumber(value), {}, makeSpan(sourceName, source, start, cursor - start) });
					continue;
				}

				if (c == '"' || c == '\'')
				{
					String value;
					Bool closed = false;
					while (cursor < source.size())
					{
						const Char current = source[cursor++];
						if (current == c) { closed = true; break; }
						if (current == '\\' && cursor < source.size())
						{
							const Char escaped = source[cursor++];
							switch (escaped)
							{
							case 'n': value += '\n'; break;
							case 't': value += '\t'; break;
							default: value += escaped; break;
							}
						}
						else value += current;
						if (value.size() > UiExpressionCompiler::maximumConstantStringBytes)
						{
							diagnostics.error("String literal budget exceeded.", makeSpan(sourceName, source, start, cursor - start));
							while (cursor < source.size() && source[cursor] != c) ++cursor;
							if (cursor < source.size()) { ++cursor; closed = true; }
							break;
						}
					}
					if (!closed) diagnostics.error("Unterminated string literal.", makeSpan(sourceName, source, start, cursor - start));
					result.push_back({ TokenKind::Value, UiValue::fromString(std::move(value)), {}, makeSpan(sourceName, source, start, cursor - start) });
					continue;
				}

				if (isIdentifierStart(c))
				{
					while (cursor < source.size() && isIdentifierPart(source[cursor])) ++cursor;
					while (cursor < source.size() && source[cursor] == '.')
					{
						const SizeT dot = cursor++;
						if (cursor >= source.size() || !isIdentifierStart(source[cursor]))
						{
							diagnostics.error("Property path requires a name after '.'.", makeSpan(sourceName, source, dot, 1));
							break;
						}
						while (cursor < source.size() && isIdentifierPart(source[cursor])) ++cursor;
					}
					const String word = source.substr(start, cursor - start);
					const SourceSpan span = makeSpan(sourceName, source, start, cursor - start);
					if (word == "true") result.push_back({ TokenKind::Value, UiValue::fromBool(true), {}, span });
					else if (word == "false") result.push_back({ TokenKind::Value, UiValue::fromBool(false), {}, span });
					else if (word == "null") result.push_back({ TokenKind::Value, UiValue::null(), {}, span });
					else result.push_back({ TokenKind::Property, {}, word, span });
					continue;
				}

				const auto match = [&](const Char expected)
					{
						if (cursor < source.size() && source[cursor] == expected) { ++cursor; return true; }
						return false;
					};
				switch (c)
				{
				case '(': addSimple(TokenKind::LeftParen, start, 1); break;
				case ')': addSimple(TokenKind::RightParen, start, 1); break;
				case '+': addSimple(TokenKind::Plus, start, 1); break;
				case '-': addSimple(TokenKind::Minus, start, 1); break;
				case '*': addSimple(TokenKind::Star, start, 1); break;
				case '/': addSimple(TokenKind::Slash, start, 1); break;
				case '%': addSimple(TokenKind::Percent, start, 1); break;
				case '!': addSimple(match('=') ? TokenKind::BangEqual : TokenKind::Bang, start, cursor - start); break;
				case '=': if (match('=')) addSimple(TokenKind::EqualEqual, start, 2); else diagnostics.error("Use '==' for equality.", makeSpan(sourceName, source, start, 1)); break;
				case '<': addSimple(match('=') ? TokenKind::LessEqual : TokenKind::Less, start, cursor - start); break;
				case '>': addSimple(match('=') ? TokenKind::GreaterEqual : TokenKind::Greater, start, cursor - start); break;
				case '&': if (match('&')) addSimple(TokenKind::AndAnd, start, 2); else diagnostics.error("Use '&&' for boolean and.", makeSpan(sourceName, source, start, 1)); break;
				case '|': if (match('|')) addSimple(TokenKind::OrOr, start, 2); else diagnostics.error("Use '||' for boolean or.", makeSpan(sourceName, source, start, 1)); break;
				case '?': if (match('?')) addSimple(TokenKind::QuestionQuestion, start, 2); else diagnostics.error("Use '??' for null coalescing.", makeSpan(sourceName, source, start, 1)); break;
				default: diagnostics.error("Unexpected character in expression.", makeSpan(sourceName, source, start, 1)); break;
				}
			}
			if (result.size() >= UiExpressionCompiler::maximumTokens && cursor < source.size())
				diagnostics.error("Expression token budget exceeded.", makeSpan(sourceName, source, cursor, 0));
			result.push_back({ TokenKind::End, {}, {}, makeSpan(sourceName, source, source.size(), 0) });
			return result;
		}

		Int precedence(const TokenKind kind)
		{
			switch (kind)
			{
			case TokenKind::QuestionQuestion: return 1;
			case TokenKind::OrOr: return 2;
			case TokenKind::AndAnd: return 3;
			case TokenKind::EqualEqual: case TokenKind::BangEqual: return 4;
			case TokenKind::Less: case TokenKind::LessEqual: case TokenKind::Greater: case TokenKind::GreaterEqual: return 5;
			case TokenKind::Plus: case TokenKind::Minus: return 6;
			case TokenKind::Star: case TokenKind::Slash: case TokenKind::Percent: return 7;
			default: return -1;
			}
		}

		UiOpCode binaryOp(const TokenKind kind)
		{
			switch (kind)
			{
			case TokenKind::Plus: return UiOpCode::Add;
			case TokenKind::Minus: return UiOpCode::Subtract;
			case TokenKind::Star: return UiOpCode::Multiply;
			case TokenKind::Slash: return UiOpCode::Divide;
			case TokenKind::Percent: return UiOpCode::Modulo;
			case TokenKind::EqualEqual: return UiOpCode::Equal;
			case TokenKind::BangEqual: return UiOpCode::NotEqual;
			case TokenKind::Less: return UiOpCode::Less;
			case TokenKind::LessEqual: return UiOpCode::LessEqual;
			case TokenKind::Greater: return UiOpCode::Greater;
			case TokenKind::GreaterEqual: return UiOpCode::GreaterEqual;
			case TokenKind::AndAnd: return UiOpCode::And;
			case TokenKind::OrOr: return UiOpCode::Or;
			case TokenKind::QuestionQuestion: return UiOpCode::NullCoalesce;
			default: return UiOpCode::Add;
			}
		}

		class Parser
		{
		public:
			Parser(Vector<Token> newTokens, DiagnosticSink& newDiagnostics)
				: tokens(std::move(newTokens)), diagnostics(newDiagnostics) {}

			Bool run(UiExpressionProgram& output)
			{
				program = &output;
				if (!parseExpression(1)) return false;
				if (current().kind != TokenKind::End)
				{
					diagnostics.error("Unexpected token after expression.", current().span);
					return false;
				}
				return true;
			}

		private:
			Vector<Token> tokens;
			SizeT cursor = 0;
			SizeT nestingDepth = 0;
			DiagnosticSink& diagnostics;
			UiExpressionProgram* program = nullptr;

			const Token& current() const { return tokens[cursor]; }
			SizeT emit(const UiOpCode code, const UInt operand = 0)
			{
				if (program->instructions.size() >= UiExpressionCompiler::maximumInstructions)
				{
					diagnostics.error("Expression instruction budget exceeded.", current().span);
					return program->instructions.size();
				}
				const SizeT index = program->instructions.size();
				program->instructions.push_back({ code, operand });
				return index;
			}

			void patchJump(const SizeT instruction)
			{
				if (instruction >= program->instructions.size() ||
					program->instructions.size() > static_cast<SizeT>(std::numeric_limits<UInt>::max()))
				{
					diagnostics.error("Expression jump target is invalid.", current().span);
					return;
				}
				program->instructions[instruction].operand = static_cast<UInt>(program->instructions.size());
			}

			Bool parsePrimary()
			{
				const Token token = current();
				if (token.kind == TokenKind::Value)
				{
					++cursor;
					if (program->constants.size() >= UiExpressionCompiler::maximumConstants)
					{
						diagnostics.error("Expression constant budget exceeded.", token.span);
						return false;
					}
					const UInt index = static_cast<UInt>(program->constants.size());
					program->constants.push_back(token.value);
					emit(UiOpCode::PushConstant, index);
					return true;
				}
				if (token.kind == TokenKind::Property)
				{
					++cursor;
					if (token.text.size() > UiBindingLimits::maximumPropertyPathBytes)
					{
						diagnostics.error("Property path budget exceeded.", token.span);
						return false;
					}
					if (program->properties.size() >= UiExpressionCompiler::maximumProperties)
					{
						diagnostics.error("Expression property budget exceeded.", token.span);
						return false;
					}
					const UInt index = static_cast<UInt>(program->properties.size());
					program->properties.push_back(token.text);
					emit(UiOpCode::LoadProperty, index);
					return true;
				}
				if (token.kind == TokenKind::LeftParen)
				{
					++cursor;
					if (++nestingDepth > UiExpressionCompiler::maximumNestingDepth)
					{
						diagnostics.error("Expression nesting budget exceeded.", token.span);
						return false;
					}
					if (!parseExpression(1)) return false;
					if (current().kind != TokenKind::RightParen)
					{
						diagnostics.error("Expected ')'.", current().span);
						return false;
					}
					++cursor;
					--nestingDepth;
					return true;
				}
				diagnostics.error("Expected literal, property path, or '('.", token.span);
				return false;
			}

			Bool parseUnary()
			{
				if (current().kind == TokenKind::Bang || current().kind == TokenKind::Minus)
				{
					const TokenKind operation = current().kind;
					const Token token = current();
					++cursor;
					if (++nestingDepth > UiExpressionCompiler::maximumNestingDepth)
					{
						diagnostics.error("Expression nesting budget exceeded.", token.span);
						return false;
					}
					if (!parseUnary()) return false;
					--nestingDepth;
					emit(operation == TokenKind::Bang ? UiOpCode::Not : UiOpCode::Negate);
					return true;
				}
				return parsePrimary();
			}

			Bool parseExpression(const Int minimumPrecedence)
			{
				if (!parseUnary()) return false;
				while (precedence(current().kind) >= minimumPrecedence)
				{
					const TokenKind operation = current().kind;
					const Int operationPrecedence = precedence(operation);
					++cursor;
					const Int nextPrecedence = operation == TokenKind::QuestionQuestion ? operationPrecedence : operationPrecedence + 1;
					if (operation == TokenKind::AndAnd || operation == TokenKind::OrOr || operation == TokenKind::QuestionQuestion)
					{
						const UiOpCode jumpCode = operation == TokenKind::AndAnd ? UiOpCode::JumpIfFalse :
							(operation == TokenKind::OrOr ? UiOpCode::JumpIfTrue : UiOpCode::JumpIfNotNull);
						const SizeT jump = emit(jumpCode);
						if (!parseExpression(nextPrecedence)) return false;
						if (operation != TokenKind::QuestionQuestion) emit(UiOpCode::ToBool);
						patchJump(jump);
						continue;
					}
					if (!parseExpression(nextPrecedence)) return false;
					emit(binaryOp(operation));
				}
				return true;
			}
		};

		Bool equalValues(const UiValue& left, const UiValue& right)
		{
			if (left.type != right.type) return false;
			switch (left.type)
			{
			case UiValueType::Null: return true;
			case UiValueType::Bool: return left.boolean == right.boolean;
			case UiValueType::Number: return left.number == right.number;
			case UiValueType::String: return left.string == right.string;
			case UiValueType::Array: return left.array == right.array;
			case UiValueType::Object: return left.object == right.object;
			}
			return false;
		}
	}

	UiExpressionCompileResult UiExpressionCompiler::compile(const String& source, const String& sourceName)
	{
		UiExpressionCompileResult result;
		result.program.span = { sourceName, 0, source.size(), 1, 1 };
		DiagnosticSink diagnostics;
		if (source.empty()) diagnostics.error("Expression cannot be empty.", result.program.span);
		else if (source.size() > maximumSourceBytes) diagnostics.error("Expression source budget exceeded.", result.program.span);
		else
		{
			Parser parser(tokenize(source, sourceName, diagnostics), diagnostics);
			(void)parser.run(result.program);
		}
		result.diagnostics = std::move(diagnostics.items);
		result.valid = !diagnostics.hasErrors() && !result.program.instructions.empty();
		return result;
	}

	UiExpressionResult UiExpressionVm::evaluate(const UiExpressionProgram& program, const UiPropertyRegistry& properties, SizeT instructionBudget)
	{
		UiExpressionResult result;
		Vector<UiValue> stack;
		stack.reserve(std::min(maximumStack, program.instructions.size()));
		const auto fail = [&](String error)
			{
				result.error = std::move(error);
				return result;
			};
		const auto pop = [&](UiValue& value)
			{
				if (stack.empty()) return false;
				value = std::move(stack.back());
				stack.pop_back();
				return true;
			};

		if (program.instructions.size() > UiExpressionCompiler::maximumInstructions ||
			program.constants.size() > UiExpressionCompiler::maximumConstants ||
			program.properties.size() > UiExpressionCompiler::maximumProperties)
			return fail("Expression program exceeds sandbox limits.");
		for (const UiValue& constant : program.constants)
			if (!constant.isValid()) return fail("Expression contains an invalid constant.");
		for (const String& path : program.properties)
			if (path.empty() || path.size() > UiBindingLimits::maximumPropertyPathBytes)
				return fail("Expression contains an invalid property path.");

		SizeT instructionPointer = 0;
		while (instructionPointer < program.instructions.size())
		{
			if (instructionBudget-- == 0) return fail("Expression instruction budget exceeded.");
			const UiInstruction& instruction = program.instructions[instructionPointer++];
			if (stack.size() >= maximumStack &&
				(instruction.code == UiOpCode::PushConstant || instruction.code == UiOpCode::LoadProperty))
				return fail("Expression stack budget exceeded.");
			if (instruction.code == UiOpCode::PushConstant)
			{
				if (static_cast<SizeT>(instruction.operand) >= program.constants.size()) return fail("Invalid constant operand.");
				stack.push_back(program.constants[instruction.operand]);
				continue;
			}
			if (instruction.code == UiOpCode::LoadProperty)
			{
				if (static_cast<SizeT>(instruction.operand) >= program.properties.size()) return fail("Invalid property operand.");
				const UiPropertyEntry* property = properties.find(program.properties[instruction.operand]);
				if (!property) return fail("Unknown property: " + program.properties[instruction.operand]);
				stack.push_back(property->value);
				continue;
			}
			if (instruction.code == UiOpCode::JumpIfFalse ||
				instruction.code == UiOpCode::JumpIfTrue ||
				instruction.code == UiOpCode::JumpIfNotNull)
			{
				if (stack.empty()) return fail("Expression stack underflow.");
				if (static_cast<SizeT>(instruction.operand) <= instructionPointer - 1 ||
					static_cast<SizeT>(instruction.operand) > program.instructions.size())
					return fail("Invalid expression jump target.");
				Bool takeJump = false;
				if (instruction.code == UiOpCode::JumpIfFalse)
				{
					takeJump = !stack.back().truthy();
					if (takeJump) stack.back() = UiValue::fromBool(false);
				}
				else if (instruction.code == UiOpCode::JumpIfTrue)
				{
					takeJump = stack.back().truthy();
					if (takeJump) stack.back() = UiValue::fromBool(true);
				}
				else takeJump = stack.back().type != UiValueType::Null;
				if (takeJump) instructionPointer = instruction.operand;
				else stack.pop_back();
				continue;
			}

			UiValue right;
			if (!pop(right)) return fail("Expression stack underflow.");
			if (instruction.code == UiOpCode::Not) { stack.push_back(UiValue::fromBool(!right.truthy())); continue; }
			if (instruction.code == UiOpCode::ToBool) { stack.push_back(UiValue::fromBool(right.truthy())); continue; }
			if (instruction.code == UiOpCode::Negate)
			{
				if (right.type != UiValueType::Number) return fail("Unary '-' requires a number.");
				const Double value = -right.number;
				if (!std::isfinite(value)) return fail("Arithmetic result is out of range.");
				stack.push_back(UiValue::fromNumber(value));
				continue;
			}

			UiValue left;
			if (!pop(left)) return fail("Expression stack underflow.");
			switch (instruction.code)
			{
			case UiOpCode::Add:
				if (left.type == UiValueType::String || right.type == UiValueType::String)
				{
					String value = left.toString();
					const String suffix = right.toString();
					if (value.size() > maximumResultStringBytes || suffix.size() > maximumResultStringBytes - value.size())
						return fail("Expression string budget exceeded.");
					value += suffix;
					stack.push_back(UiValue::fromString(std::move(value)));
				}
				else if (left.type == UiValueType::Number && right.type == UiValueType::Number)
				{
					const Double value = left.number + right.number;
					if (!std::isfinite(value)) return fail("Arithmetic result is out of range.");
					stack.push_back(UiValue::fromNumber(value));
				}
				else return fail("'+' requires numbers or a string operand.");
				break;
			case UiOpCode::Subtract: case UiOpCode::Multiply: case UiOpCode::Divide: case UiOpCode::Modulo:
			{
				if (left.type != UiValueType::Number || right.type != UiValueType::Number) return fail("Arithmetic requires numbers.");
				if ((instruction.code == UiOpCode::Divide || instruction.code == UiOpCode::Modulo) && right.number == 0.0) return fail("Division by zero.");
				Double value = 0.0;
				if (instruction.code == UiOpCode::Subtract) value = left.number - right.number;
				else if (instruction.code == UiOpCode::Multiply) value = left.number * right.number;
				else if (instruction.code == UiOpCode::Divide) value = left.number / right.number;
				else value = std::fmod(left.number, right.number);
				if (!std::isfinite(value)) return fail("Arithmetic result is out of range.");
				stack.push_back(UiValue::fromNumber(value));
				break;
			}
			case UiOpCode::Equal: stack.push_back(UiValue::fromBool(equalValues(left, right))); break;
			case UiOpCode::NotEqual: stack.push_back(UiValue::fromBool(!equalValues(left, right))); break;
			case UiOpCode::Less: case UiOpCode::LessEqual: case UiOpCode::Greater: case UiOpCode::GreaterEqual:
				if (left.type != UiValueType::Number || right.type != UiValueType::Number) return fail("Comparison requires numbers.");
				if (instruction.code == UiOpCode::Less) stack.push_back(UiValue::fromBool(left.number < right.number));
				else if (instruction.code == UiOpCode::LessEqual) stack.push_back(UiValue::fromBool(left.number <= right.number));
				else if (instruction.code == UiOpCode::Greater) stack.push_back(UiValue::fromBool(left.number > right.number));
				else stack.push_back(UiValue::fromBool(left.number >= right.number));
				break;
			case UiOpCode::And: stack.push_back(UiValue::fromBool(left.truthy() && right.truthy())); break;
			case UiOpCode::Or: stack.push_back(UiValue::fromBool(left.truthy() || right.truthy())); break;
			case UiOpCode::NullCoalesce: stack.push_back(left.type == UiValueType::Null ? right : left); break;
			default: return fail("Invalid binary instruction.");
			}
		}
		if (stack.size() != 1) return fail("Expression did not produce one value.");
		result.value = std::move(stack.back());
		result.valid = true;
		return result;
	}
}
