#include "UiBindingRegistry.hpp"

#include <cmath>

namespace Ui
{
	namespace
	{
		Bool isIdentifierStart(const Char value)
		{
			return (value >= 'a' && value <= 'z') || (value >= 'A' && value <= 'Z') || value == '_';
		}

		Bool isIdentifierPart(const Char value)
		{
			return isIdentifierStart(value) || (value >= '0' && value <= '9');
		}

		Bool isDottedName(const String& value, const SizeT maximumBytes)
		{
			if (value.empty() || value.size() > maximumBytes) return false;
			Bool segmentStart = true;
			for (const Char character : value)
			{
				if (segmentStart)
				{
					if (!isIdentifierStart(character)) return false;
					segmentStart = false;
				}
				else if (character == '.') segmentStart = true;
				else if (!isIdentifierPart(character)) return false;
			}
			return !segmentStart;
		}

		Bool isValueTypeValid(const UiValueType type)
		{
			return type == UiValueType::Null || type == UiValueType::Bool ||
				type == UiValueType::Number || type == UiValueType::String ||
				type == UiValueType::Array || type == UiValueType::Object;
		}
	}

	Bool UiValue::truthy() const
	{
		switch (type)
		{
		case UiValueType::Bool: return boolean;
		case UiValueType::Number: return number != 0.0;
		case UiValueType::String: return !string.empty();
		case UiValueType::Array: return !array.empty();
		case UiValueType::Object: return !object.empty();
		case UiValueType::Null: return false;
		}
		return false;
	}

	String UiValue::toString() const
	{
		switch (type)
		{
		case UiValueType::Bool: return boolean ? "true" : "false";
		case UiValueType::Number: return std::to_string(number);
		case UiValueType::String: return string;
		case UiValueType::Array: return "[array]";
		case UiValueType::Object: return "[object]";
		case UiValueType::Null: return "null";
		}
		return {};
	}

	Bool UiValue::isValid() const
	{
		if (!isValueTypeValid(type)) return false;
		if (type == UiValueType::Number) return std::isfinite(number);
		if (type == UiValueType::String) return string.size() <= UiBindingLimits::maximumValueStringBytes;
		if (type == UiValueType::Array)
		{
			if (array.size() > UiBindingLimits::maximumCommandArguments * 1024) return false;
			for (const UiValue& value : array) if (!value.isValid()) return false;
		}
		if (type == UiValueType::Object)
		{
			if (object.size() > 256) return false;
			for (const auto& [name, value] : object)
				if (name.empty() || name.size() > UiBindingLimits::maximumPropertyPathBytes || !value.isValid()) return false;
		}
		return true;
	}

	Bool UiValue::operator==(const UiValue& other) const
	{
		if (type != other.type) return false;
		switch (type)
		{
		case UiValueType::Null: return true;
		case UiValueType::Bool: return boolean == other.boolean;
		case UiValueType::Number: return number == other.number;
		case UiValueType::String: return string == other.string;
		case UiValueType::Array: return array == other.array;
		case UiValueType::Object: return object == other.object;
		}
		return false;
	}

	Bool UiPropertyRegistry::set(String path, UiValue value)
	{
		if (!isDottedName(path, maximumPathBytes) || !value.isValid() ||
			(properties.size() >= maximumProperties && !properties.contains(path))) return false;
		const auto existing = properties.find(path);
		if (existing != properties.end() && existing->second.value == value) return true;
		++registryVersion;
		properties[std::move(path)] = { std::move(value), registryVersion };
		return true;
	}

	const UiPropertyEntry* UiPropertyRegistry::find(const String& path) const
	{
		const auto found = properties.find(path);
		return found == properties.end() ? nullptr : &found->second;
	}

	Bool UiPropertyRegistry::read(const String& path, UiValue& value, U64* propertyVersion) const
	{
		const UiPropertyEntry* property = find(path);
		if (!property) return false;
		value = property->value;
		if (propertyVersion) *propertyVersion = property->version;
		return true;
	}

	Bool UiPropertyRegistry::erase(const String& path)
	{
		const auto found = properties.find(path);
		if (found == properties.end()) return false;
		properties.erase(found);
		++registryVersion;
		return true;
	}

	Bool UiCommandRegistry::registerCommand(String name, UiCommandSignature signature, Function<Bool(const Vector<UiValue>&)> callback)
	{
		if (!isDottedName(name, maximumNameBytes) || signature.arguments.size() > maximumArguments || !callback ||
			(commands.size() >= maximumCommands && !commands.contains(name))) return false;
		for (const UiValueType type : signature.arguments)
			if (!isValueTypeValid(type)) return false;
		++registryVersion;
		commands[std::move(name)] = { std::move(signature), std::move(callback), registryVersion };
		return true;
	}

	Bool UiCommandRegistry::unregisterCommand(const String& name)
	{
		const auto found = commands.find(name);
		if (found == commands.end()) return false;
		commands.erase(found);
		++registryVersion;
		return true;
	}

	const UiCommandEntry* UiCommandRegistry::find(const String& name) const
	{
		const auto found = commands.find(name);
		return found == commands.end() ? nullptr : &found->second;
	}

	Bool UiCommandRegistry::invoke(const String& name, const Vector<UiValue>& arguments) const
	{
		const UiCommandEntry* command = find(name);
		if (!command || arguments.size() != command->signature.arguments.size()) return false;
		for (SizeT i = 0; i < arguments.size(); ++i)
		{
			if (arguments[i].type != command->signature.arguments[i] || !arguments[i].isValid()) return false;
		}
		return command->callback(arguments);
	}

	UiCommandRegistrySnapshot UiCommandRegistry::snapshot() const
	{
		UiCommandRegistrySnapshot result;
		result.version = registryVersion;
		for (const auto& [name, command] : commands)
			result.commands[name] = { command.signature, command.version };
		return result;
	}
}
