#include "UiTemplateInstantiator.hpp"
#include "UiWidgetBridge.hpp"

#include <cctype>

namespace Ui
{
	namespace
	{
		String lower(String value)
		{
			for (Char& character : value)
				character = static_cast<Char>(std::tolower(static_cast<unsigned char>(character)));
			return value;
		}

		UiRoutedEventType eventType(const String& name, Bool& valid)
		{
			const String value = lower(name);
			valid = true;
			if (value == "activate" || value == "click") return UiRoutedEventType::Activate;
			if (value == "pointerdown") return UiRoutedEventType::PointerDown;
			if (value == "pointerup") return UiRoutedEventType::PointerUp;
			if (value == "keydown") return UiRoutedEventType::KeyDown;
			if (value == "keyup") return UiRoutedEventType::KeyUp;
			if (value == "focusin") return UiRoutedEventType::FocusIn;
			if (value == "focusout") return UiRoutedEventType::FocusOut;
			if (value == "cancel") return UiRoutedEventType::Cancel;
			valid = false;
			return UiRoutedEventType::Activate;
		}

		String typeName(const UiMarkupNode& node)
		{
			if (node.kind == UiMarkupNodeKind::Text) return "Text";
			if (node.kind == UiMarkupNodeKind::Component) return node.name;
			const String name = lower(node.name);
			if (name == "button") return "Button";
			if (name == "window") return "LogicalWindow";
			if (name == "div" || name == "panel") return "Box";
			if (name == "label" || name == "span" || name == "p") return "Text";
			if (name == "input") return "TextField";
			if (name == "progress") return "Progress";
			if (name == "slot") return "Slot";
			return "Box";
		}

		void exposeObject(UiPropertyRegistry& scope, const String& prefix, const UiValue& value)
		{
			(void)scope.set(prefix, value);
			if (value.type != UiValueType::Object) return;
			for (const auto& [name, child] : value.object)
				exposeObject(scope, prefix + "." + name, child);
		}

		Bool hasBinding(const UiCompiledBundle& bundle, const UInt blueprintIndex, const String& target)
		{
			const String folded = lower(target);
			for (const UiCompiledBinding& binding : bundle.bindings)
			{
				if (binding.nodeIndex != blueprintIndex ||
					binding.kind == UiAttributeKind::IfDirective) continue;
				if (lower(binding.target) == folded) return true;
			}
			return false;
		}

		void applyLiteralText(const UiMarkupNode& blueprint, String& text)
		{
			text = blueprint.text;
			for (const UiMarkupAttribute& attribute : blueprint.attributes)
				if (attribute.kind == UiAttributeKind::Literal && lower(attribute.name) == "text")
					text = attribute.value;
		}

		Bool boundValueApplied(const UiNode& node, const String& target, const UiValue& value)
		{
			const String folded = lower(target);
			if (folded == "text") return node.authored.text == value.toString();
			if (folded == "id") return node.authored.id == value.toString();
			if (folded == "visible") return node.state.visible == value.truthy();
			if (folded == "enabled") return node.state.enabled == value.truthy();
			if (folded == "class" || folded == "classes")
				return node.authored.classes.size() == 1 && node.authored.classes.front() == value.toString();
			if (folded == "value") return node.state.semantics.value == value.toString();
			if (folded == "checked") return node.state.semantics.checked == value.truthy();
			if (folded == "selected") return node.state.semantics.selected == value.truthy();
			if (folded == "label") return node.state.semantics.label == value.toString();
			return false;
		}
	}

	UiTemplateInstantiator::UiTemplateInstantiator(
		UiDocument& newDocument, UiEventRouter& newRouter,
		UiPropertyRegistry& newProperties, UiCommandRegistry& newCommands)
		: document(newDocument), router(newRouter), properties(newProperties), commands(newCommands)
	{
	}

	UiTemplateInstantiator::~UiTemplateInstantiator()
	{
		clear();
	}

	void UiTemplateInstantiator::diagnose(String message, const SourceSpan& span)
	{
		if (runtimeDiagnostics.size() < DiagnosticSink::maximumDiagnostics)
			runtimeDiagnostics.push_back({ DiagnosticSeverity::Error, std::move(message), span });
	}

	UiExpressionResult UiTemplateInstantiator::evaluate(
		const UiExpressionProgram& expression, const UiPropertyRegistry& scope)
	{
		UiExpressionResult result = UiExpressionVm::evaluate(
			expression, scope, limits.expressionInstructionBudget);
		if (!result.valid) diagnose(result.error, expression.span);
		return result;
	}

	void UiTemplateInstantiator::applyBindings(
		const UiCompiledBundle& bundle, Instance& instance, const UiPropertyRegistry& scope)
	{
		UiNode* node = document.get(instance.node);
		if (!node) return;
		for (const UiCompiledBinding& binding : bundle.bindings)
		{
			if (binding.nodeIndex != instance.blueprintIndex ||
				binding.kind == UiAttributeKind::IfDirective) continue;

			Vector<U64> versions;
			versions.reserve(binding.expression.properties.size());
			for (const String& path : binding.expression.properties)
			{
				const UiPropertyEntry* property = scope.find(path);
				versions.push_back(property ? property->version : 0);
			}
			const String cacheKey = binding.target + "#" + toString(binding.nodeIndex);
			const UiExpressionResult result = evaluate(binding.expression, scope);
			if (!result.valid) continue;

			const auto previous = instance.bindingValues.find(cacheKey);
			const Bool cachedValue = previous != instance.bindingValues.end() &&
				previous->second == result.value;
			const Bool cachedVersions = instance.bindingVersions[cacheKey] == versions;
			if (cachedVersions && cachedValue && boundValueApplied(*node, binding.target, result.value))
				continue;

			instance.bindingVersions[cacheKey] = std::move(versions);
			instance.bindingValues[cacheKey] = result.value;

			const String target = lower(binding.target);
			if (target == "text")
			{
				node->authored.text = result.value.toString();
				document.markDirty(instance.node, DirtyFlag::Measure);
			}
			else if (target == "id")
			{
				node->authored.id = result.value.toString();
				document.markDirty(instance.node, DirtyFlag::Style);
			}
			else if (target == "visible")
			{
				node->state.visible = result.value.truthy();
				document.markDirty(instance.node, DirtyFlag::Measure);
			}
			else if (target == "enabled")
			{
				node->state.enabled = result.value.truthy();
				document.markDirty(instance.node, DirtyFlag::Paint);
			}
			else if (target == "class" || target == "classes")
			{
				node->authored.classes = { result.value.toString() };
				document.markDirty(instance.node, DirtyFlag::Style);
			}
			else if (target == "value")
			{
				node->state.semantics.value = result.value.toString();
				document.markDirty(instance.node, DirtyFlag::Paint);
			}
			else if (target == "checked")
			{
				node->state.semantics.checked = result.value.truthy();
				document.markDirty(instance.node, DirtyFlag::Paint);
			}
			else if (target == "selected")
			{
				node->state.semantics.selected = result.value.truthy();
				document.markDirty(instance.node, DirtyFlag::Paint);
			}
			else if (target == "label")
			{
				node->state.semantics.label = result.value.toString();
				document.markDirty(instance.node, DirtyFlag::Paint);
			}
			else if (widgetBridge && widgetBridge->ownsNode(instance.node))
			{
				// Widget bridge syncs interactive bindings directly.
			}
			else diagnose("Unsupported authored property binding: " + binding.target,
				binding.expression.span);
		}
	}

	void UiTemplateInstantiator::instantiateNode(
		const UiCompiledBundle& bundle, const UInt blueprintIndex,
		const UiNodeHandle parent, const String& parentKey, const SizeT siblingIndex,
		const UiPropertyRegistry& scope)
	{
		if (static_cast<SizeT>(blueprintIndex) >= bundle.blueprint.nodes.size() ||
			instances.size() >= limits.maximumInstances) return;
		const UiMarkupNode& blueprint = bundle.blueprint.nodes[blueprintIndex];
		if (blueprint.kind == UiMarkupNodeKind::Comment) return;

		const UiCompiledBinding* condition = nullptr;
		for (const UiCompiledBinding& binding : bundle.bindings)
			if (binding.nodeIndex == blueprintIndex && binding.kind == UiAttributeKind::IfDirective)
				condition = &binding;

		const UiCompiledForBinding* loop = nullptr;
		for (const UiCompiledForBinding& binding : bundle.loops)
			if (binding.nodeIndex == blueprintIndex) loop = &binding;

		if (blueprint.kind == UiMarkupNodeKind::IfBlock)
		{
			if (!condition)
			{
				diagnose("@if block is missing a condition binding.", blueprint.span);
				return;
			}
			const UiExpressionResult value = evaluate(condition->expression, scope);
			if (!value.valid || !value.value.truthy()) return;
			for (SizeT child = 0; child < blueprint.children.size(); ++child)
				instantiateNode(bundle, blueprint.children[child], parent, parentKey, child, scope);
			return;
		}

		if (blueprint.kind == UiMarkupNodeKind::ForBlock)
		{
			if (!loop)
			{
				diagnose("@for block is missing a loop binding.", blueprint.span);
				return;
			}
			const UiExpressionResult collection = evaluate(loop->collection, scope);
			if (!collection.valid) return;
			if (collection.value.type != UiValueType::Array)
			{
				diagnose("@for collection must evaluate to an array.", loop->collection.span);
				return;
			}
			if (collection.value.array.size() > limits.maximumLoopItems)
			{
				diagnose("@for item budget exceeded.", loop->collection.span);
				return;
			}
			for (SizeT index = 0; index < collection.value.array.size(); ++index)
			{
				UiPropertyRegistry itemScope = scope;
				exposeObject(itemScope, loop->itemName, collection.value.array[index]);
				const UiExpressionResult key = evaluate(loop->key, itemScope);
				if (!key.valid) continue;
				const String loopKey = key.value.toString();
				if (loopKey.empty() || loopKey == "null")
				{
					diagnose("@for track expression must produce a non-null stable value.", loop->key.span);
					continue;
				}
				const String loopParentKey = parentKey + "[" + loopKey + "]";
				for (SizeT child = 0; child < blueprint.children.size(); ++child)
					instantiateNode(bundle, blueprint.children[child], parent, loopParentKey, child, itemScope);
			}
			return;
		}
		if (blueprint.kind == UiMarkupNodeKind::Text &&
			std::all_of(blueprint.text.begin(), blueprint.text.end(),
				[](const Char value) { return std::isspace(static_cast<unsigned char>(value)) != 0; }))
			return;

		const String identity = !blueprint.id.empty() ? "#" + blueprint.id :
			blueprint.name + ":" + toString(siblingIndex);
		const String stableKey = parentKey + "/" + identity;
		Instance& instance = instances[stableKey];
		const Bool existing = document.isValid(instance.node);
		if (widgetBridge && widgetBridge->handles(blueprint))
		{
			if (!widgetBridge->ensureInstance(
				bundle, blueprintIndex, parent, stableKey, scope, instance.node, reconcileSerial))
			{
				diagnose("Could not create authored widget.", blueprint.span);
				instances.erase(stableKey);
				return;
			}
		}
		else if (!existing)
		{
			UiAuthoredData authored;
			authored.typeName = typeName(blueprint);
			authored.styleType = lower(blueprint.name);
			authored.id = blueprint.id;
			authored.classes = blueprint.classes;
			applyLiteralText(blueprint, authored.text);
			instance.node = document.createNode(parent, std::move(authored));
			if (!instance.node.isValid())
			{
				diagnose("Could not create retained UI node.", blueprint.span);
				instances.erase(stableKey);
				return;
			}
		}
		else
		{
			UiNode& retained = *document.get(instance.node);
			const UiStateData localState = retained.state;
			retained.authored.typeName = typeName(blueprint);
			retained.authored.styleType = lower(blueprint.name);
			if (!hasBinding(bundle, blueprintIndex, "id"))
				retained.authored.id = blueprint.id;
			retained.authored.classes = blueprint.classes;
			if (!hasBinding(bundle, blueprintIndex, "text"))
				applyLiteralText(blueprint, retained.authored.text);
			retained.state = localState;
			if (retained.parentHandle() != parent) (void)document.reparentNode(instance.node, parent);
		}
		instance.blueprintIndex = blueprintIndex;
		instance.seenGeneration = reconcileSerial;

		for (const UiListenerToken token : instance.listeners) (void)router.removeListener(token);
		instance.listeners.clear();
		for (const UiCompiledEventBinding& event : bundle.events)
		{
			if (event.nodeIndex != blueprintIndex) continue;
			const UiCommandEntry* command = commands.find(event.commandName);
			if (!command || command->version != event.commandVersion)
			{
				diagnose("Event command allowlist changed: " + event.commandName, blueprint.span);
				continue;
			}
			Bool validEvent = false;
			const UiRoutedEventType type = eventType(event.eventName, validEvent);
			if (!validEvent)
			{
				diagnose("Unsupported routed event: " + event.eventName, blueprint.span);
				continue;
			}
			instance.listeners.push_back(router.addListener(instance.node, type,
				[this, name = event.commandName](UiRoutedEvent& routed)
				{
					if (commands.invoke(name, {})) routed.handled = true;
				}));
		}

		applyBindings(bundle, instance, scope);
		if (widgetBridge && widgetBridge->ownsNode(instance.node))
			widgetBridge->applyInstanceBindings(bundle, blueprintIndex, stableKey, scope);
		for (SizeT child = 0; child < blueprint.children.size(); ++child)
			instantiateNode(bundle, blueprint.children[child], instance.node, stableKey, child, scope);
	}

	Bool UiTemplateInstantiator::reconcileLogicalWindowSubtree(
		const UiCompiledBundle& bundle,
		const UInt blueprintIndex,
		UiNodeHandle parent,
		const UiTemplateRuntimeBudgets& budgets)
	{
		runtimeDiagnostics.clear();
		if (!bundle.valid)
		{
			diagnose("Cannot instantiate an invalid authored UI bundle.", {});
			return false;
		}
		if (blueprintIndex >= bundle.blueprint.nodes.size())
		{
			diagnose("Logical window blueprint index is out of range.", {});
			return false;
		}
		if (!parent.isValid()) parent = document.root();
		if (!document.isValid(parent) || document.isTraversing()) return false;
		limits = budgets;
		++reconcileSerial;
		instantiateNode(bundle, blueprintIndex, parent, "$", 0, properties);

		for (auto iterator = instances.begin(); iterator != instances.end();)
		{
			if (iterator->second.seenGeneration == reconcileSerial) { ++iterator; continue; }
			for (const UiListenerToken token : iterator->second.listeners) (void)router.removeListener(token);
			if (widgetBridge && widgetBridge->ownsNode(iterator->second.node))
				widgetBridge->destroyRecord(iterator->first);
			else if (document.isValid(iterator->second.node))
				(void)document.destroyNode(iterator->second.node);
			iterator = instances.erase(iterator);
		}
		if (widgetBridge) widgetBridge->destroyStale(reconcileSerial);
		return true;
	}

	Bool UiTemplateInstantiator::reconcile(
		const UiCompiledBundle& bundle, UiNodeHandle parent, const UiTemplateRuntimeBudgets& budgets)
	{
		runtimeDiagnostics.clear();
		if (!bundle.valid)
		{
			diagnose("Cannot instantiate an invalid authored UI bundle.", {});
			return false;
		}
		if (!parent.isValid()) parent = document.root();
		if (!document.isValid(parent) || document.isTraversing()) return false;
		limits = budgets;
		++reconcileSerial;
		const UiMarkupNode& root = bundle.blueprint.nodes[bundle.blueprint.rootIndex];
		for (SizeT child = 0; child < root.children.size(); ++child)
			instantiateNode(bundle, root.children[child], parent, "$", child, properties);

		for (auto iterator = instances.begin(); iterator != instances.end();)
		{
			if (iterator->second.seenGeneration == reconcileSerial) { ++iterator; continue; }
			for (const UiListenerToken token : iterator->second.listeners) (void)router.removeListener(token);
			if (widgetBridge && widgetBridge->ownsNode(iterator->second.node))
				widgetBridge->destroyRecord(iterator->first);
			else if (document.isValid(iterator->second.node))
				(void)document.destroyNode(iterator->second.node);
			iterator = instances.erase(iterator);
		}
		if (widgetBridge) widgetBridge->destroyStale(reconcileSerial);
		return true;
	}

	void UiTemplateInstantiator::clear()
	{
		for (auto& [key, instance] : instances)
		{
			for (const UiListenerToken token : instance.listeners) (void)router.removeListener(token);
			if (widgetBridge && widgetBridge->ownsNode(instance.node))
				widgetBridge->destroyRecord(key);
			else if (document.isValid(instance.node))
				(void)document.destroyNode(instance.node);
		}
		instances.clear();
		if (widgetBridge) widgetBridge->clear();
	}
}
