#pragma once

#include "UiBundleManager.hpp"
#include "../Core/UiDocument.hpp"
#include "../Input/UiEventRouter.hpp"

namespace Ui
{
	class UiWidgetBridge;

	struct UiTemplateRuntimeBudgets
	{
		SizeT maximumInstances = UiDocument::maxNodes;
		SizeT maximumLoopItems = 4096;
		SizeT expressionInstructionBudget = UiExpressionVm::defaultInstructionBudget;
	};

	class UiTemplateInstantiator
	{
	public:
		UiTemplateInstantiator(UiDocument& document, UiEventRouter& router,
			UiPropertyRegistry& properties, UiCommandRegistry& commands);
		~UiTemplateInstantiator();

		[[nodiscard]] Bool reconcile(const UiCompiledBundle& bundle,
			UiNodeHandle parent = {}, const UiTemplateRuntimeBudgets& budgets = {});
		[[nodiscard]] const Vector<UiDiagnostic>& diagnostics() const { return runtimeDiagnostics; }
		void setWidgetBridge(UiWidgetBridge* bridge) { widgetBridge = bridge; }
		void clear();

	private:
		struct Instance
		{
			UiNodeHandle node;
			UInt blueprintIndex = 0;
			Vector<UiListenerToken> listeners;
			Map<String, Vector<U64>> bindingVersions;
			Map<String, UiValue> bindingValues;
			U64 seenGeneration = 0;
		};

		UiDocument& document;
		UiEventRouter& router;
		UiPropertyRegistry& properties;
		UiCommandRegistry& commands;
		UiWidgetBridge* widgetBridge = nullptr;
		Map<String, Instance> instances;
		Vector<UiDiagnostic> runtimeDiagnostics;
		UiTemplateRuntimeBudgets limits;
		U64 reconcileSerial = 0;

		void instantiateNode(const UiCompiledBundle& bundle, UInt blueprintIndex,
			UiNodeHandle parent, const String& parentKey, SizeT siblingIndex,
			const UiPropertyRegistry& scope);
		void applyBindings(const UiCompiledBundle& bundle, Instance& instance,
			const UiPropertyRegistry& scope);
		[[nodiscard]] UiExpressionResult evaluate(const UiExpressionProgram& expression,
			const UiPropertyRegistry& scope);
		void diagnose(String message, const SourceSpan& span);
	};
}
