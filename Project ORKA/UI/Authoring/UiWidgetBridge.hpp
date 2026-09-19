#pragma once

#include "UiBundleManager.hpp"
#include "../Binding/UiBindingRegistry.hpp"
#include "../Core/UiDocument.hpp"
#include "../Widgets/UiWidgets.hpp"

namespace Ui
{
	// Maps authored markup tags to interactive UiWidgetFactory controls.
	class UiWidgetBridge
	{
	public:
		using PropertyChangeHandler = Function<void(const String& propertyPath, const UiValue& value)>;

		UiWidgetBridge(UiWidgetFactory& factory, UiPropertyRegistry& properties);
		~UiWidgetBridge();

		void setPropertyChangeHandler(PropertyChangeHandler handler);
		void setTabIndexAllocator(Function<Int()> allocator);

		[[nodiscard]] Bool handles(const UiMarkupNode& node) const;
		[[nodiscard]] Bool ownsNode(UiNodeHandle node) const;
		[[nodiscard]] UiWidgetHandle findById(const String& id) const;
		[[nodiscard]] UiNodeHandle nodeForId(const String& id) const;

		Bool ensureInstance(
			const UiCompiledBundle& bundle,
			UInt blueprintIndex,
			UiNodeHandle parent,
			const String& stableKey,
			const UiPropertyRegistry& scope,
			UiNodeHandle& outNode,
			U64 reconcileGeneration);

		void applyInstanceBindings(
			const UiCompiledBundle& bundle,
			UInt blueprintIndex,
			const String& stableKey,
			const UiPropertyRegistry& scope);

		void destroyStale(U64 reconcileGeneration);
		void destroyRecord(const String& stableKey);
		void clear();
		void syncControlLayouts();

	private:
		struct Record
		{
			UiWidgetHandle handle;
			UiWidgetKind kind = UiWidgetKind::Box;
			UiNodeHandle node;
			String stableKey;
			String valuePath;
			String textPath;
			String optionsPath;
			U64 seenGeneration = 0;
		};

		UiWidgetFactory& factory;
		UiPropertyRegistry& properties;
		PropertyChangeHandler onPropertyChange;
		Function<Int()> allocateTabIndex;
		Map<String, Record> records;
		Map<String, String> idToStableKey;

		[[nodiscard]] Record* findRecord(const String& stableKey);
		[[nodiscard]] const Record* findRecord(const String& stableKey) const;
		void writeProperty(const String& path, UiValue value);
		[[nodiscard]] UiValue readProperty(const String& path, const UiPropertyRegistry& scope) const;
		void bindPropertyPaths(const UiCompiledBundle& bundle, UInt blueprintIndex, Record& record);
		void syncRecordFromScope(Record& record, const UiPropertyRegistry& scope);
		void attachChangeHandlers(Record& record);
		[[nodiscard]] Vector<UiSelectOption> readSelectOptions(const UiValue& value) const;
	};
}
