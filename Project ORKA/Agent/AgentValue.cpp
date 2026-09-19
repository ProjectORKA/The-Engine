#include "AgentValue.hpp"

#include <cctype>
#include <charconv>
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <stdexcept>

AgentValue::AgentValue(std::nullptr_t) : storage(nullptr) {}
AgentValue::AgentValue(const Bool value) : storage(value) {}
AgentValue::AgentValue(const Int value) : storage(static_cast<I64>(value)) {}
AgentValue::AgentValue(const UInt value) : storage(static_cast<I64>(value)) {}
AgentValue::AgentValue(const I64 value) : storage(value) {}
AgentValue::AgentValue(const U64 value)
	: storage(value <= static_cast<U64>(std::numeric_limits<I64>::max())
		? Storage(static_cast<I64>(value))
		: Storage(static_cast<Double>(value))) {}
AgentValue::AgentValue(const Float value) : storage(static_cast<Double>(value)) {}
AgentValue::AgentValue(const Double value) : storage(value) {}
AgentValue::AgentValue(const char* value) : storage(String(value ? value : "")) {}
AgentValue::AgentValue(String value) : storage(std::move(value)) {}
AgentValue::AgentValue(Array value) : storage(std::move(value)) {}
AgentValue::AgentValue(Object value) : storage(std::move(value)) {}

Bool AgentValue::isNull() const { return std::holds_alternative<std::nullptr_t>(storage); }
Bool AgentValue::isBool() const { return std::holds_alternative<Bool>(storage); }
Bool AgentValue::isInteger() const { return std::holds_alternative<I64>(storage); }
Bool AgentValue::isNumber() const { return isInteger() || std::holds_alternative<Double>(storage); }
Bool AgentValue::isString() const { return std::holds_alternative<String>(storage); }
Bool AgentValue::isArray() const { return std::holds_alternative<Array>(storage); }
Bool AgentValue::isObject() const { return std::holds_alternative<Object>(storage); }
Bool AgentValue::asBool(const Bool fallback) const
{
	const auto value = std::get_if<Bool>(&storage);
	return value ? *value : fallback;
}
I64 AgentValue::asInteger(const I64 fallback) const
{
	if(const auto value = std::get_if<I64>(&storage)) return *value;
	return fallback;
}
Double AgentValue::asNumber(const Double fallback) const
{
	if(const auto value = std::get_if<Double>(&storage)) return *value;
	if(const auto value = std::get_if<I64>(&storage)) return static_cast<Double>(*value);
	return fallback;
}
const String& AgentValue::asString() const { return std::get<String>(storage); }
const AgentValue::Array& AgentValue::asArray() const { return std::get<Array>(storage); }
const AgentValue::Object& AgentValue::asObject() const { return std::get<Object>(storage); }
AgentValue::Array& AgentValue::asArray() { return std::get<Array>(storage); }
AgentValue::Object& AgentValue::asObject() { return std::get<Object>(storage); }
const AgentValue* AgentValue::find(const String& key) const
{
	if(!isObject()) return nullptr;
	const auto iterator = asObject().find(key);
	return iterator == asObject().end() ? nullptr : &iterator->second;
}

namespace
{
	Bool validUtf8(const String& text)
	{
		for(SizeT i = 0; i < text.size();)
		{
			const Byte first = static_cast<Byte>(text[i++]);
			if(first <= 0x7f) continue;
			U32 codePoint = 0;
			SizeT continuationCount = 0;
			U32 minimum = 0;
			if((first & 0xe0) == 0xc0) { codePoint = first & 0x1f; continuationCount = 1; minimum = 0x80; }
			else if((first & 0xf0) == 0xe0) { codePoint = first & 0x0f; continuationCount = 2; minimum = 0x800; }
			else if((first & 0xf8) == 0xf0) { codePoint = first & 0x07; continuationCount = 3; minimum = 0x10000; }
			else return false;
			if(i + continuationCount > text.size()) return false;
			for(SizeT j = 0; j < continuationCount; ++j)
			{
				const Byte next = static_cast<Byte>(text[i++]);
				if((next & 0xc0) != 0x80) return false;
				codePoint = (codePoint << 6) | (next & 0x3f);
			}
			if(codePoint < minimum || codePoint > 0x10ffff
				|| (codePoint >= 0xd800 && codePoint <= 0xdfff)) return false;
		}
		return true;
	}

	class JsonParser
	{
	public:
		JsonParser(const String& input, const AgentJsonLimits& limits)
			: input(input), limits(limits) {}

		AgentJsonParseResult parse()
		{
			if(input.size() > limits.maxInputBytes) fail("JSON input exceeds limit");
			if(error.empty() && !validUtf8(input)) fail("JSON input is not valid UTF-8");
			skipWhitespace();
			AgentValue value;
			if(error.empty()) value = parseValue(0);
			skipWhitespace();
			if(error.empty() && position != input.size()) fail("Unexpected trailing data");
			return {std::move(value), std::move(error), errorOffset};
		}

	private:
		const String& input;
		const AgentJsonLimits& limits;
		SizeT position = 0;
		SizeT nodes = 0;
		String error;
		SizeT errorOffset = 0;

		void fail(const char* message)
		{
			if(error.empty())
			{
				error = message;
				errorOffset = position;
			}
		}

		void skipWhitespace()
		{
			while(position < input.size())
			{
				const char c = input[position];
				if(c != ' ' && c != '\t' && c != '\r' && c != '\n') break;
				++position;
			}
		}

		Bool consume(const char c)
		{
			if(position < input.size() && input[position] == c)
			{
				++position;
				return true;
			}
			return false;
		}

		AgentValue parseValue(const SizeT depth)
		{
			if(depth > limits.maxDepth)
			{
				fail("JSON nesting exceeds limit");
				return {};
			}
			if(++nodes > limits.maxNodes)
			{
				fail("JSON node count exceeds limit");
				return {};
			}
			if(position >= input.size())
			{
				fail("Expected JSON value");
				return {};
			}

			switch(input[position])
			{
			case 'n': return parseLiteral("null", AgentValue());
			case 't': return parseLiteral("true", AgentValue(true));
			case 'f': return parseLiteral("false", AgentValue(false));
			case '"': return AgentValue(parseString());
			case '[': return parseArray(depth + 1);
			case '{': return parseObject(depth + 1);
			default:
				if(input[position] == '-' || (input[position] >= '0' && input[position] <= '9'))
					return parseNumber();
				fail("Invalid JSON value");
				return {};
			}
		}

		AgentValue parseLiteral(const char* literal, AgentValue value)
		{
			const SizeT length = std::strlen(literal);
			if(input.compare(position, length, literal) != 0)
			{
				fail("Invalid JSON literal");
				return {};
			}
			position += length;
			return value;
		}

		static void appendUtf8(String& output, const U32 codePoint)
		{
			if(codePoint <= 0x7f) output.push_back(static_cast<char>(codePoint));
			else if(codePoint <= 0x7ff)
			{
				output.push_back(static_cast<char>(0xc0 | (codePoint >> 6)));
				output.push_back(static_cast<char>(0x80 | (codePoint & 0x3f)));
			}
			else if(codePoint <= 0xffff)
			{
				output.push_back(static_cast<char>(0xe0 | (codePoint >> 12)));
				output.push_back(static_cast<char>(0x80 | ((codePoint >> 6) & 0x3f)));
				output.push_back(static_cast<char>(0x80 | (codePoint & 0x3f)));
			}
			else
			{
				output.push_back(static_cast<char>(0xf0 | (codePoint >> 18)));
				output.push_back(static_cast<char>(0x80 | ((codePoint >> 12) & 0x3f)));
				output.push_back(static_cast<char>(0x80 | ((codePoint >> 6) & 0x3f)));
				output.push_back(static_cast<char>(0x80 | (codePoint & 0x3f)));
			}
		}

		U32 parseHex4()
		{
			if(position + 4 > input.size())
			{
				fail("Incomplete Unicode escape");
				return 0;
			}
			U32 value = 0;
			for(Int i = 0; i < 4; ++i)
			{
				const char c = input[position++];
				value <<= 4;
				if(c >= '0' && c <= '9') value |= static_cast<U32>(c - '0');
				else if(c >= 'a' && c <= 'f') value |= static_cast<U32>(c - 'a' + 10);
				else if(c >= 'A' && c <= 'F') value |= static_cast<U32>(c - 'A' + 10);
				else
				{
					fail("Invalid Unicode escape");
					return 0;
				}
			}
			return value;
		}

		String parseString()
		{
			String output;
			if(!consume('"'))
			{
				fail("Expected string");
				return output;
			}
			while(position < input.size())
			{
				const unsigned char c = static_cast<unsigned char>(input[position++]);
				if(c == '"') return output;
				if(c < 0x20)
				{
					fail("Unescaped control character");
					return {};
				}
				if(c != '\\') output.push_back(static_cast<char>(c));
				else
				{
					if(position >= input.size())
					{
						fail("Incomplete escape");
						return {};
					}
					switch(input[position++])
					{
					case '"': output.push_back('"'); break;
					case '\\': output.push_back('\\'); break;
					case '/': output.push_back('/'); break;
					case 'b': output.push_back('\b'); break;
					case 'f': output.push_back('\f'); break;
					case 'n': output.push_back('\n'); break;
					case 'r': output.push_back('\r'); break;
					case 't': output.push_back('\t'); break;
					case 'u':
					{
						U32 codePoint = parseHex4();
						if(error.empty() && codePoint >= 0xd800 && codePoint <= 0xdbff)
						{
							if(position + 2 > input.size() || input[position] != '\\' || input[position + 1] != 'u')
								fail("Missing low Unicode surrogate");
							else
							{
								position += 2;
								const U32 low = parseHex4();
								if(low < 0xdc00 || low > 0xdfff) fail("Invalid low Unicode surrogate");
								else codePoint = 0x10000 + ((codePoint - 0xd800) << 10) + (low - 0xdc00);
							}
						}
						else if(codePoint >= 0xdc00 && codePoint <= 0xdfff) fail("Unexpected low Unicode surrogate");
						if(error.empty()) appendUtf8(output, codePoint);
						break;
					}
					default: fail("Invalid string escape"); return {};
					}
				}
				if(output.size() > limits.maxStringBytes)
				{
					fail("JSON string exceeds limit");
					return {};
				}
			}
			fail("Unterminated string");
			return {};
		}

		AgentValue parseNumber()
		{
			const SizeT start = position;
			consume('-');
			if(consume('0'))
			{
				if(position < input.size() && std::isdigit(static_cast<unsigned char>(input[position])))
					fail("Leading zero in number");
			}
			else
			{
				if(position >= input.size() || !std::isdigit(static_cast<unsigned char>(input[position])))
					fail("Invalid number");
				while(position < input.size() && std::isdigit(static_cast<unsigned char>(input[position]))) ++position;
			}
			Bool floating = false;
			if(consume('.'))
			{
				floating = true;
				if(position >= input.size() || !std::isdigit(static_cast<unsigned char>(input[position])))
					fail("Invalid number fraction");
				while(position < input.size() && std::isdigit(static_cast<unsigned char>(input[position]))) ++position;
			}
			if(position < input.size() && (input[position] == 'e' || input[position] == 'E'))
			{
				floating = true;
				++position;
				if(position < input.size() && (input[position] == '+' || input[position] == '-')) ++position;
				if(position >= input.size() || !std::isdigit(static_cast<unsigned char>(input[position])))
					fail("Invalid number exponent");
				while(position < input.size() && std::isdigit(static_cast<unsigned char>(input[position]))) ++position;
			}
			if(!error.empty()) return {};
			const String token = input.substr(start, position - start);
			if(!floating)
			{
				I64 integer = 0;
				const auto result = std::from_chars(token.data(), token.data() + token.size(), integer);
				if(result.ec == std::errc()) return AgentValue(integer);
			}
			char* end = nullptr;
			const Double number = std::strtod(token.c_str(), &end);
			if(end != token.c_str() + token.size() || !std::isfinite(number))
			{
				fail("Number is outside supported range");
				return {};
			}
			return AgentValue(number);
		}

		AgentValue parseArray(const SizeT depth)
		{
			AgentValue::Array array;
			++position;
			skipWhitespace();
			if(consume(']')) return AgentValue(std::move(array));
			while(error.empty())
			{
				array.push_back(parseValue(depth));
				skipWhitespace();
				if(consume(']')) break;
				if(!consume(','))
				{
					fail("Expected ',' or ']'");
					break;
				}
				skipWhitespace();
			}
			return AgentValue(std::move(array));
		}

		AgentValue parseObject(const SizeT depth)
		{
			AgentValue::Object object;
			++position;
			skipWhitespace();
			if(consume('}')) return AgentValue(std::move(object));
			while(error.empty())
			{
				if(position >= input.size() || input[position] != '"')
				{
					fail("Expected object key");
					break;
				}
				String key = parseString();
				skipWhitespace();
				if(!consume(':'))
				{
					fail("Expected ':'");
					break;
				}
				skipWhitespace();
				auto [iterator, inserted] = object.emplace(std::move(key), parseValue(depth));
				(void)iterator;
				if(!inserted) fail("Duplicate object key");
				skipWhitespace();
				if(consume('}')) break;
				if(!consume(','))
				{
					fail("Expected ',' or '}'");
					break;
				}
				skipWhitespace();
			}
			return AgentValue(std::move(object));
		}
	};

	struct JsonSerializer
	{
		const AgentJsonLimits& limits;
		SizeT nodes = 0;
		String output;

		void ensureOutputLimit() const
		{
			if(output.size() > limits.maxInputBytes)
				throw std::length_error("Serialized JSON exceeds limit");
		}

		void serializeString(const String& value)
		{
			if(value.size() > limits.maxStringBytes)
				throw std::length_error("JSON string exceeds limit");
			if(!validUtf8(value))
				throw std::invalid_argument("AgentValue string is not valid UTF-8");
			output.push_back('"');
			const char hex[] = "0123456789abcdef";
			for(const unsigned char c : value)
			{
				switch(c)
				{
				case '"': output += "\\\""; break;
				case '\\': output += "\\\\"; break;
				case '\b': output += "\\b"; break;
				case '\f': output += "\\f"; break;
				case '\n': output += "\\n"; break;
				case '\r': output += "\\r"; break;
				case '\t': output += "\\t"; break;
				default:
					if(c < 0x20)
					{
						output += "\\u00";
						output.push_back(hex[c >> 4]);
						output.push_back(hex[c & 15]);
					}
					else output.push_back(static_cast<char>(c));
				}
				ensureOutputLimit();
			}
			output.push_back('"');
			ensureOutputLimit();
		}

		void serializeValue(const AgentValue& value, const SizeT depth)
		{
			if(depth > limits.maxDepth) throw std::length_error("JSON nesting exceeds limit");
			if(++nodes > limits.maxNodes) throw std::length_error("JSON node count exceeds limit");
			if(value.isNull()) output += "null";
			else if(value.isBool()) output += value.asBool() ? "true" : "false";
			else if(value.isInteger()) output += std::to_string(value.asInteger());
			else if(std::holds_alternative<Double>(value.storage))
			{
				const Double number = std::get<Double>(value.storage);
				if(!std::isfinite(number))
					throw std::invalid_argument("AgentValue contains a non-finite number");
				char buffer[64]{};
				const auto result = std::to_chars(buffer, buffer + sizeof(buffer), number,
					std::chars_format::general, std::numeric_limits<Double>::max_digits10);
				if(result.ec != std::errc()) throw std::runtime_error("Could not serialize number");
				output.append(buffer, result.ptr);
			}
			else if(value.isString()) serializeString(value.asString());
			else if(value.isArray())
			{
				output.push_back('[');
				Bool first = true;
				for(const auto& item : value.asArray())
				{
					if(!first) output.push_back(',');
					first = false;
					serializeValue(item, depth + 1);
				}
				output.push_back(']');
			}
			else
			{
				output.push_back('{');
				Bool first = true;
				for(const auto& [key, item] : value.asObject())
				{
					if(!first) output.push_back(',');
					first = false;
					serializeString(key);
					output.push_back(':');
					serializeValue(item, depth + 1);
				}
				output.push_back('}');
			}
			ensureOutputLimit();
		}
	};
}

AgentJsonParseResult parseAgentJson(const String& text, const AgentJsonLimits& limits)
{
	return JsonParser(text, limits).parse();
}

String serializeAgentJson(const AgentValue& value, const AgentJsonLimits& limits)
{
	JsonSerializer serializer{limits};
	serializer.output.reserve((std::min)(limits.maxInputBytes, static_cast<SizeT>(256)));
	serializer.serializeValue(value, 0);
	return serializer.output;
}

