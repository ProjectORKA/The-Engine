#pragma once

#include "../Authoring/UiDiagnostics.hpp"

namespace Ui
{
	struct UiBindingLimits
	{
		static constexpr SizeT maximumValueStringBytes = 4096;
		static constexpr SizeT maximumPropertyPathBytes = 256;
		static constexpr SizeT maximumCommandNameBytes = 128;
		static constexpr SizeT maximumCommandArguments = 16;
	};

	enum class UiValueType : Byte
	{
		Null,
		Bool,
		Number,
		String,
		Array,
		Object,
	};

	struct UiValue
	{
		UiValueType type = UiValueType::Null;
		Bool boolean = false;
		Double number = 0.0;
		String string;
		Vector<UiValue> array;
		Map<String, UiValue> object;

		static UiValue null() { return {}; }
		static UiValue fromBool(Bool value) { UiValue result; result.type = UiValueType::Bool; result.boolean = value; return result; }
		static UiValue fromNumber(Double value) { UiValue result; result.type = UiValueType::Number; result.number = value; return result; }
		static UiValue fromString(String value) { UiValue result; result.type = UiValueType::String; result.string = std::move(value); return result; }
		static UiValue fromArray(Vector<UiValue> value) { UiValue result; result.type = UiValueType::Array; result.array = std::move(value); return result; }
		static UiValue fromObject(Map<String, UiValue> value) { UiValue result; result.type = UiValueType::Object; result.object = std::move(value); return result; }
		[[nodiscard]] Bool truthy() const;
		[[nodiscard]] String toString() const;
		[[nodiscard]] Bool isValid() const;
		[[nodiscard]] Bool operator==(const UiValue& other) const;
	};

	struct UiPropertyEntry
	{
		UiValue value;
		U64 version = 0;
	};

	struct UiPropertyDescriptor
	{
		UiValue value;
		U64 version = 0;
	};

	struct UiPropertyRegistrySnapshot
	{
		Map<String, UiPropertyDescriptor> properties;
		U64 version = 0;
	};

	class UiPropertyRegistry
	{
	public:
		static constexpr SizeT maximumProperties = 16384;
		static constexpr SizeT maximumPathBytes = UiBindingLimits::maximumPropertyPathBytes;

		[[nodiscard]] Bool set(String path, UiValue value);
		[[nodiscard]] const UiPropertyEntry* find(const String& path) const;
		[[nodiscard]] Bool read(const String& path, UiValue& value, U64* propertyVersion = nullptr) const;
		[[nodiscard]] Bool erase(const String& path);
		[[nodiscard]] SizeT size() const { return properties.size(); }
		[[nodiscard]] U64 version() const { return registryVersion; }
		[[nodiscard]] UiPropertyRegistrySnapshot snapshot() const;

	private:
		Map<String, UiPropertyEntry> properties;
		U64 registryVersion = 0;
	};

	struct UiCommandSignature
	{
		Vector<UiValueType> arguments;
	};

	struct UiCommandEntry
	{
		UiCommandSignature signature;
		Function<Bool(const Vector<UiValue>&)> callback;
		U64 version = 0;
	};

	struct UiCommandDescriptor
	{
		UiCommandSignature signature;
		U64 version = 0;
	};

	struct UiCommandRegistrySnapshot
	{
		Map<String, UiCommandDescriptor> commands;
		U64 version = 0;
	};

	class UiCommandRegistry
	{
	public:
		static constexpr SizeT maximumCommands = 1024;
		static constexpr SizeT maximumNameBytes = UiBindingLimits::maximumCommandNameBytes;
		static constexpr SizeT maximumArguments = UiBindingLimits::maximumCommandArguments;

		[[nodiscard]] Bool registerCommand(String name, UiCommandSignature signature, Function<Bool(const Vector<UiValue>&)> callback);
		[[nodiscard]] Bool unregisterCommand(const String& name);
		[[nodiscard]] const UiCommandEntry* find(const String& name) const;
		[[nodiscard]] Bool invoke(const String& name, const Vector<UiValue>& arguments) const;
		[[nodiscard]] UiCommandRegistrySnapshot snapshot() const;
		[[nodiscard]] SizeT size() const { return commands.size(); }
		[[nodiscard]] U64 version() const { return registryVersion; }

	private:
		Map<String, UiCommandEntry> commands;
		U64 registryVersion = 0;
	};
}
