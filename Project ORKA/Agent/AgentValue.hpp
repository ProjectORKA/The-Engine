#pragma once

#include "../Basics.hpp"

#include <map>
#include <variant>

struct AgentJsonLimits
{
	SizeT maxInputBytes = 1024 * 1024;
	SizeT maxDepth = 64;
	SizeT maxNodes = 100000;
	SizeT maxStringBytes = 1024 * 1024;
};

struct AgentValue
{
	using Array = Vector<AgentValue>;
	using Object = std::map<String, AgentValue>;
	using Storage = std::variant<std::nullptr_t, Bool, I64, Double, String, Array, Object>;

	AgentValue() = default;
	AgentValue(std::nullptr_t);
	AgentValue(Bool value);
	AgentValue(Int value);
	AgentValue(UInt value);
	AgentValue(I64 value);
	AgentValue(U64 value);
	AgentValue(Float value);
	AgentValue(Double value);
	AgentValue(const char* value);
	AgentValue(String value);
	AgentValue(Array value);
	AgentValue(Object value);

	[[nodiscard]] Bool isNull() const;
	[[nodiscard]] Bool isBool() const;
	[[nodiscard]] Bool isInteger() const;
	[[nodiscard]] Bool isNumber() const;
	[[nodiscard]] Bool isString() const;
	[[nodiscard]] Bool isArray() const;
	[[nodiscard]] Bool isObject() const;

	[[nodiscard]] Bool asBool(Bool fallback = false) const;
	[[nodiscard]] I64 asInteger(I64 fallback = 0) const;
	[[nodiscard]] Double asNumber(Double fallback = 0.0) const;
	[[nodiscard]] const String& asString() const;
	[[nodiscard]] const Array& asArray() const;
	[[nodiscard]] const Object& asObject() const;
	[[nodiscard]] Array& asArray();
	[[nodiscard]] Object& asObject();
	[[nodiscard]] const AgentValue* find(const String& key) const;

	Storage storage = nullptr;
};

struct AgentJsonParseResult
{
	AgentValue value;
	String error;
	SizeT errorOffset = 0;

	[[nodiscard]] Bool succeeded() const { return error.empty(); }
};

[[nodiscard]] AgentJsonParseResult parseAgentJson(
	const String& text, const AgentJsonLimits& limits = {});
[[nodiscard]] String serializeAgentJson(
	const AgentValue& value, const AgentJsonLimits& limits = {});

