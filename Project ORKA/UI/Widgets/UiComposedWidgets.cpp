#include "UiComposedWidgets.hpp"

namespace Ui
{
	namespace
	{
		constexpr Int keyEnter = 257;
		constexpr Int keyRight = 262;
		constexpr Int keyLeft = 263;

		Bool uniqueKeys(const Vector<UiItemKey>& keys)
		{
			std::set<UiItemKey> unique;
			for (const UiItemKey key : keys)
				if (key == 0 || !unique.insert(key).second) return false;
			return true;
		}

		String depthText(const UInt depth)
		{
			return std::to_string(std::min(depth, UInt(999)));
		}
	}

	UiComposedController::UiComposedController(UiDocument& document, UiEventRouter& router)
		: document(document), router(router)
	{
	}

	UiComposedController::~UiComposedController()
	{
		for (const UiListenerToken listener : listeners) (void)router.removeListener(listener);
		if (document.isValid(rootNode) && !document.isTraversing())
			(void)document.destroyNode(rootNode);
	}

	UiNodeHandle UiComposedController::makeNode(const UiNodeHandle parent, String type,
		String styleClass, String text, const UiSemanticRole role, String label)
	{
		if (ownedNodes.size() >= maximumOwnedNodes || document.isTraversing()) return {};
		UiAuthoredData authored;
		authored.typeName = std::move(type);
		authored.text = std::move(text);
		if (!styleClass.empty()) authored.classes.push_back(std::move(styleClass));
		const UiNodeHandle result = document.createNode(
			document.isValid(parent) ? parent : document.root(), std::move(authored));
		if (result.isValid())
		{
			ownedNodes.push_back(result);
			if (UiNode* node = document.get(result))
			{
				node->state.semantics.role = role;
				node->state.semantics.label = std::move(label);
			}
		}
		return result;
	}

	void UiComposedController::makeInteractive(const UiNodeHandle node, const Bool focusable,
		const Int tabIndex, const Bool focusScope, const Bool modal)
	{
		UiInteractionData data;
		data.focusable = focusable;
		data.tabIndex = focusable ? tabIndex : -1;
		data.focusScope = focusScope;
		data.modalScope = modal;
		router.setInteraction(node, data);
	}

	void UiComposedController::listen(const UiNodeHandle node, const UiRoutedEventType type,
		UiEventCallback callback)
	{
		const UiListenerToken token = router.addListener(node, type, std::move(callback));
		if (token.isValid())
		{
			listeners.push_back(token);
			listenerNodes.push_back(node);
		}
	}

	void UiComposedController::setClass(
		const UiNodeHandle handle, const String& styleClass, const Bool present)
	{
		UiNode* node = document.get(handle);
		if (!node) return;
		auto& classes = node->authored.classes;
		const auto found = std::find(classes.begin(), classes.end(), styleClass);
		if (present && found == classes.end()) classes.push_back(styleClass);
		else if (!present && found != classes.end()) classes.erase(found);
		document.markDirty(handle, DirtyFlag::Style);
		document.markDirty(handle, DirtyFlag::Paint);
	}

	void UiComposedController::destroyOwnedNode(const UiNodeHandle handle)
	{
		for (SizeT index = listeners.size(); index > 0; --index)
		{
			UiNodeHandle current = listenerNodes[index - 1];
			Bool belongsToSubtree = false;
			while (document.isValid(current))
			{
				if (current == handle) { belongsToSubtree = true; break; }
				const UiNode* node = document.get(current);
				current = node ? node->parentHandle() : UiNodeHandle{};
			}
			if (!belongsToSubtree) continue;
			(void)router.removeListener(listeners[index - 1]);
			listeners.erase(listeners.begin() + static_cast<std::ptrdiff_t>(index - 1));
			listenerNodes.erase(listenerNodes.begin() + static_cast<std::ptrdiff_t>(index - 1));
		}
		if (document.isValid(handle)) (void)document.destroyNode(handle);
		ownedNodes.erase(std::remove_if(ownedNodes.begin(), ownedNodes.end(),
			[this](const UiNodeHandle node) { return !document.isValid(node); }), ownedNodes.end());
	}

	void UiComposedController::clearOwnedChildren()
	{
		while (ownedNodes.size() > 1) destroyOwnedNode(ownedNodes.back());
	}

	UiMenuController::UiMenuController(
		UiDocument& document, UiEventRouter& router, UiMenuConfig config)
		: UiComposedController(document, router), config(std::move(config))
	{
		const Char* style = this->config.presentation == UiMenuPresentation::MenuBar ?
			"ui-menu-bar" : this->config.presentation == UiMenuPresentation::ContextMenu ?
			"ui-context-menu" : "ui-menu";
		rootNode = makeNode(this->config.parent, "Menu", style, {}, UiSemanticRole::Menu);
		makeInteractive(rootNode, false, -1, true);
		Vector<UiMenuItem> items = std::move(this->config.items);
		this->config.items.clear();
		(void)setItems(std::move(items));
		if (this->config.presentation == UiMenuPresentation::MenuBar) (void)open();
	}

	Bool UiMenuController::setItems(Vector<UiMenuItem> items)
	{
		if (items.size() > maximumItems) return false;
		Vector<UiItemKey> keys;
		for (const UiMenuItem& item : items) keys.push_back(item.key);
		if (!uniqueKeys(keys)) return false;
		config.items = std::move(items);
		rebuild();
		return true;
	}

	Bool UiMenuController::open(const UiItemKey parentKey)
	{
		if (parentKey != 0 && std::none_of(config.items.begin(), config.items.end(),
			[parentKey](const UiMenuItem& item) { return item.key == parentKey; })) return false;
		openParent = parentKey;
		opened = true;
		setClass(rootNode, UiWidgetClasses::open, true);
		for (SizeT index = 0; index < config.items.size() && index < itemNodes.size(); ++index)
			if (UiNode* node = document.get(itemNodes[index]))
				node->state.visible =
					(config.presentation == UiMenuPresentation::MenuBar &&
					 config.items[index].parentKey == 0) ||
					(opened && config.items[index].parentKey == openParent &&
					 !(config.presentation == UiMenuPresentation::MenuBar && openParent == 0));
		if (config.onOpenChanged) config.onOpenChanged(true);
		return true;
	}

	void UiMenuController::close()
	{
		if (!opened) return;
		opened = false;
		setClass(rootNode, UiWidgetClasses::open, false);
		for (SizeT index = 0; index < itemNodes.size(); ++index)
			if (UiNode* node = document.get(itemNodes[index]))
				node->state.visible = config.presentation == UiMenuPresentation::MenuBar &&
					index < config.items.size() && config.items[index].parentKey == 0;
		if (config.onOpenChanged) config.onOpenChanged(false);
	}

	void UiMenuController::rebuild()
	{
		if (!rootNode.isValid() || document.isTraversing()) return;
		for (const UiNodeHandle item : itemNodes)
			destroyOwnedNode(item);
		itemNodes.clear();
		if (config.items.size() > maximumItems) config.items.resize(maximumItems);
		for (SizeT index = 0; index < config.items.size(); ++index)
		{
			const UiMenuItem& item = config.items[index];
			const UiNodeHandle node = makeNode(rootNode,
				item.separator ? "MenuSeparator" : "MenuItem",
				item.separator ? "ui-menu-separator" : "ui-menu-item",
				item.label, item.separator ? UiSemanticRole::None : UiSemanticRole::MenuItem,
				item.label);
			itemNodes.push_back(node);
			if (UiNode* value = document.get(node))
			{
				value->state.enabled = item.enabled && !item.separator;
				value->state.visible =
					(config.presentation == UiMenuPresentation::MenuBar && item.parentKey == 0) ||
					(opened && item.parentKey == openParent &&
					 !(config.presentation == UiMenuPresentation::MenuBar && openParent == 0));
				value->state.semantics.value = item.command;
				value->state.semantics.checked = item.checked;
			}
			setClass(node, UiWidgetClasses::checked, item.checked);
			if (!item.separator)
			{
				makeInteractive(node, item.enabled, 0);
				listen(node, UiRoutedEventType::Activate,
					[this, index](UiRoutedEvent& event) { activate(index, event); });
			}
		}
	}

	void UiMenuController::activate(const SizeT index, UiRoutedEvent& event)
	{
		if (index >= config.items.size() || !config.items[index].enabled ||
			config.items[index].separator) return;
		const UiMenuItem& item = config.items[index];
		const Bool hasChildren = std::any_of(config.items.begin(), config.items.end(),
			[&item](const UiMenuItem& child) { return child.parentKey == item.key; });
		if (hasChildren) (void)open(item.key);
		else
		{
			if (config.onActivate) config.onActivate(item.key);
			if (config.presentation != UiMenuPresentation::MenuBar) close();
		}
		event.handled = true;
		event.stopPropagation();
	}

	UiPopupController::UiPopupController(
		UiDocument& document, UiEventRouter& router, UiPopupConfig config)
		: UiComposedController(document, router), config(std::move(config))
	{
		const Bool tooltip = this->config.kind == UiPopupKind::Tooltip;
		rootNode = makeNode(this->config.parent, tooltip ? "Tooltip" : "Popover",
			tooltip ? "ui-tooltip" : "ui-popover", {},
			tooltip ? UiSemanticRole::Text : UiSemanticRole::Dialog);
		content = makeNode(rootNode, "Content", UiWidgetClasses::content, this->config.text,
			UiSemanticRole::Text);
		makeInteractive(rootNode, false, -1, this->config.interactive);
		if (UiNode* node = document.get(rootNode)) node->state.visible = false;
	}

	void UiPopupController::show()
	{
		if (opened) return;
		opened = true;
		if (UiNode* node = document.get(rootNode))
		{
			node->state.visible = true;
			node->state.semantics.value = std::to_string(config.anchor.index);
		}
		setClass(rootNode, UiWidgetClasses::open, true);
		if (config.onOpenChanged) config.onOpenChanged(true);
	}

	void UiPopupController::hide()
	{
		if (!opened) return;
		opened = false;
		if (UiNode* node = document.get(rootNode)) node->state.visible = false;
		setClass(rootNode, UiWidgetClasses::open, false);
		if (config.onOpenChanged) config.onOpenChanged(false);
	}

	void UiPopupController::setText(String text)
	{
		config.text = std::move(text);
		if (UiNode* node = document.get(content))
		{
			node->authored.text = config.text;
			node->state.semantics.value = config.text;
			document.markDirty(content, DirtyFlag::Paint);
		}
	}

	UiDialogController::UiDialogController(
		UiDocument& document, UiEventRouter& router, UiDialogConfig config)
		: UiComposedController(document, router), config(std::move(config))
	{
		if (this->config.actions.size() > maximumActions)
			this->config.actions.resize(maximumActions);
		rootNode = makeNode(this->config.parent, "Dialog", "ui-dialog", {},
			UiSemanticRole::Dialog, this->config.title);
		makeInteractive(rootNode, false, -1, true, true);
		(void)makeNode(rootNode, "Title", "ui-dialog-title", this->config.title,
			UiSemanticRole::Text);
		content = makeNode(rootNode, "Content", "ui-dialog-content");
		const UiNodeHandle actions = makeNode(rootNode, "Actions", "ui-dialog-actions");
		std::set<UiItemKey> actionKeys;
		for (const UiDialogAction& action : this->config.actions)
		{
			if (action.key == 0 || !actionKeys.insert(action.key).second) continue;
			if (action.defaultAction && defaultAction == 0) defaultAction = action.key;
			if (action.cancelAction && cancelAction == 0) cancelAction = action.key;
			const UiNodeHandle node = makeNode(actions, "Button", "ui-dialog-action",
				action.label, UiSemanticRole::Button, action.label);
			actionNodes.push_back(node);
			if (UiNode* value = document.get(node)) value->state.enabled = action.enabled;
			setClass(node, "is-default", action.defaultAction);
			setClass(node, "is-cancel", action.cancelAction);
			makeInteractive(node, action.enabled, 0);
			listen(node, UiRoutedEventType::Activate,
				[this, key = action.key](UiRoutedEvent& event)
				{
					invoke(key);
					event.handled = true;
					event.stopPropagation();
				});
		}
		router.setDefaultAction(rootNode, UiRoutedEventType::KeyDown,
			[this](UiRoutedEvent& event)
			{
				if (event.key == keyEnter && defaultAction != 0)
				{ invoke(defaultAction); event.handled = true; }
			});
		router.setCancelAction(rootNode,
			[this](UiRoutedEvent& event)
			{
				if (cancelAction != 0) invoke(cancelAction);
				else close(UiDialogCloseReason::Cancelled);
				event.handled = true;
			});
		if (UiNode* node = document.get(rootNode)) node->state.visible = false;
	}

	UiDialogController::~UiDialogController()
	{
		if (opened) router.popModalScope(rootNode);
	}

	void UiDialogController::show()
	{
		if (opened) return;
		opened = true;
		if (UiNode* node = document.get(rootNode)) node->state.visible = true;
		setClass(rootNode, UiWidgetClasses::open, true);
		router.pushModalScope(rootNode);
	}

	void UiDialogController::close(const UiDialogCloseReason reason, const UiItemKey action)
	{
		if (!opened) return;
		opened = false;
		router.popModalScope(rootNode);
		if (UiNode* node = document.get(rootNode)) node->state.visible = false;
		setClass(rootNode, UiWidgetClasses::open, false);
		if (config.onClose) config.onClose(reason, action);
	}

	void UiDialogController::invoke(const UiItemKey key)
	{
		const auto found = std::find_if(config.actions.begin(), config.actions.end(),
			[key](const UiDialogAction& action) { return action.key == key; });
		if (found != config.actions.end() && found->enabled)
			close(found->cancelAction ? UiDialogCloseReason::Cancelled :
				UiDialogCloseReason::Action, key);
	}

	UiTabsController::UiTabsController(
		UiDocument& document, UiEventRouter& router, UiTabsConfig config)
		: UiComposedController(document, router), config(std::move(config)),
		  selectedKey(this->config.selected)
	{
		rootNode = makeNode(this->config.parent, "Tabs", "ui-tabs");
		tabList = makeNode(rootNode, "TabList", "ui-tab-list", {}, UiSemanticRole::List);
		panel = makeNode(rootNode, "TabPanel", "ui-tab-panel");
		Vector<UiTabItem> tabs = std::move(this->config.tabs);
		this->config.tabs.clear();
		(void)setTabs(std::move(tabs));
	}

	Bool UiTabsController::setTabs(Vector<UiTabItem> tabs)
	{
		if (tabs.size() > maximumTabs) return false;
		Vector<UiItemKey> keys;
		for (const auto& tab : tabs) keys.push_back(tab.key);
		if (!uniqueKeys(keys)) return false;
		config.tabs = std::move(tabs);
		if (std::none_of(config.tabs.begin(), config.tabs.end(),
			[this](const UiTabItem& tab) { return tab.key == selectedKey && tab.enabled; }))
			selectedKey = 0;
		rebuild();
		return true;
	}

	Bool UiTabsController::select(const UiItemKey key, const Bool notify)
	{
		const auto found = std::find_if(config.tabs.begin(), config.tabs.end(),
			[key](const UiTabItem& tab) { return tab.key == key && tab.enabled; });
		if (found == config.tabs.end()) return false;
		if (selectedKey == key) return true;
		selectedKey = key;
		sync();
		if (notify && config.onChange) config.onChange(key);
		return true;
	}

	void UiTabsController::rebuild()
	{
		for (const UiNodeHandle node : tabNodes)
			destroyOwnedNode(node);
		tabNodes.clear();
		if (config.tabs.size() > maximumTabs) config.tabs.resize(maximumTabs);
		for (SizeT index = 0; index < config.tabs.size(); ++index)
		{
			const UiTabItem& tab = config.tabs[index];
			const UiNodeHandle node = makeNode(tabList, "Tab", "ui-tab", tab.label,
				UiSemanticRole::Button, tab.label);
			tabNodes.push_back(node);
			if (UiNode* value = document.get(node)) value->state.enabled = tab.enabled;
			makeInteractive(node, tab.enabled, 0);
			listen(node, UiRoutedEventType::Activate,
				[this, key = tab.key](UiRoutedEvent& event)
				{ (void)select(key); event.handled = true; });
			if (tab.closable)
			{
				const UiNodeHandle close = makeNode(node, "TabClose", "ui-tab-close", {},
					UiSemanticRole::Button, "Close " + tab.label);
				makeInteractive(close);
				listen(close, UiRoutedEventType::Activate,
					[this, key = tab.key](UiRoutedEvent& event)
					{
						if (config.onClose) config.onClose(key);
						event.handled = true;
						event.stopPropagation();
					});
			}
		}
		if (selectedKey == 0)
			for (const UiTabItem& tab : config.tabs)
				if (tab.enabled) { selectedKey = tab.key; break; }
		sync();
	}

	void UiTabsController::sync()
	{
		for (SizeT index = 0; index < tabNodes.size(); ++index)
		{
			const Bool selected = index < config.tabs.size() &&
				config.tabs[index].key == selectedKey;
			setClass(tabNodes[index], UiWidgetClasses::selected, selected);
			if (UiNode* node = document.get(tabNodes[index]))
				node->state.semantics.selected = selected;
		}
		if (UiNode* node = document.get(panel))
			node->state.semantics.value = std::to_string(selectedKey);
	}

	UiTreeViewController::UiTreeViewController(
		UiDocument& document, UiEventRouter& router, UiTreeViewConfig config)
		: UiComposedController(document, router), config(std::move(config)),
		  virtualRows(this->config.estimatedRowHeight), overscan(this->config.overscanExtent)
	{
		rootNode = makeNode(this->config.parent, "TreeView", "ui-tree-view", {},
			UiSemanticRole::List);
		makeInteractive(rootNode, false, -1, true);
		(void)setItems(std::move(this->config.items));
	}

	Bool UiTreeViewController::setItems(Vector<UiTreeItem> items)
	{
		if (items.size() > maximumItems) return false;
		Vector<UiItemKey> keys;
		for (const auto& item : items) keys.push_back(item.key);
		if (!uniqueKeys(keys)) return false;
		Map<UiItemKey, UiTreeItem> next;
		for (const UiTreeItem& item : items) next[item.key] = item;
		for (const UiTreeItem& item : items)
			if (item.parentKey == item.key ||
				(item.parentKey != 0 && !next.contains(item.parentKey))) return false;
		for (const UiTreeItem& item : items)
		{
			std::set<UiItemKey> ancestors;
			UiItemKey current = item.key;
			UInt depth = 0;
			while (current != 0)
			{
				if (++depth > maximumDepth || !ancestors.insert(current).second) return false;
				current = next.at(current).parentKey;
			}
		}
		config.items = std::move(items);
		byKey = std::move(next);
		children.clear();
		for (const UiTreeItem& item : config.items) children[item.parentKey].push_back(item.key);
		for (auto& [parent, values] : children) std::sort(values.begin(), values.end());
		flatten();
		return true;
	}

	Bool UiTreeViewController::setExpanded(
		const UiItemKey key, const Bool expanded, const Bool notify)
	{
		if (!byKey.contains(key) || !children.contains(key)) return false;
		const Bool changed = expanded ? expandedKeys.insert(key).second :
			expandedKeys.erase(key) != 0;
		if (changed)
		{
			flatten();
			if (notify && config.onExpansionChanged) config.onExpansionChanged(key, expanded);
		}
		return true;
	}

	Bool UiTreeViewController::select(const UiItemKey key, const Bool notify)
	{
		const auto found = byKey.find(key);
		if (found == byKey.end() || !found->second.enabled) return false;
		selectedKey = key;
		materialize();
		if (notify && config.onSelectionChanged) config.onSelectionChanged(key);
		return true;
	}

	Bool UiTreeViewController::updateViewport(
		const Float scrollOffset, const Float viewportExtent)
	{
		scroll.setMetrics(Vec2(0, virtualRows.totalExtent()), Vec2(0, viewportExtent));
		scroll.setOffset(Vec2(0, scrollOffset));
		materialize();
		return true;
	}

	Bool UiTreeViewController::setMeasuredHeight(const UiItemKey key, const Float height)
	{
		if (!virtualRows.setMeasuredHeight(key, height)) return false;
		materialize();
		return true;
	}

	void UiTreeViewController::flatten()
	{
		rows.clear();
		append(0, 0);
		Vector<UiItemKey> keys;
		for (const UiTreeRow& row : rows) keys.push_back(row.key);
		(void)virtualRows.setKeys(std::move(keys));
		materialize();
	}

	void UiTreeViewController::append(const UiItemKey parent, const UInt depth)
	{
		if (depth >= maximumDepth || rows.size() >= maximumItems) return;
		const auto found = children.find(parent);
		if (found == children.end()) return;
		for (const UiItemKey key : found->second)
		{
			rows.push_back({ key, depth });
			if (rows.size() >= maximumItems) return;
			if (expandedKeys.contains(key)) append(key, depth + 1);
		}
	}

	void UiTreeViewController::materialize()
	{
		if (document.isTraversing()) return;
		for (const UiNodeHandle node : materialized)
			destroyOwnedNode(node);
		materialized.clear();
		const UiVirtualRange range = virtualRows.visibleRange(
			scroll.offset().y, scroll.viewport().y, overscan);
		for (UInt index = range.first; index < range.last && index < rows.size(); ++index)
		{
			const UiTreeRow row = rows[index];
			const UiTreeItem& item = byKey.at(row.key);
			const UiNodeHandle node = makeNode(rootNode, "TreeItem", "ui-tree-item",
				item.label, UiSemanticRole::ListItem, item.label);
			materialized.push_back(node);
			if (UiNode* value = document.get(node))
			{
				value->state.enabled = item.enabled;
				value->state.semantics.value = depthText(row.depth);
				value->state.semantics.selected = row.key == selectedKey;
			}
			setClass(node, UiWidgetClasses::selected, row.key == selectedKey);
			setClass(node, "is-expanded", expandedKeys.contains(row.key));
			setClass(node, "has-children", children.contains(row.key));
			makeInteractive(node, item.enabled);
			listen(node, UiRoutedEventType::Activate,
				[this, key = row.key](UiRoutedEvent& event)
				{ (void)select(key); event.handled = true; });
			listen(node, UiRoutedEventType::KeyDown,
				[this, key = row.key](UiRoutedEvent& event)
				{
					if (event.key == keyRight) (void)setExpanded(key, true);
					else if (event.key == keyLeft) (void)setExpanded(key, false);
					else return;
					event.handled = true;
					event.preventDefault();
				});
		}
	}

	UiDataTableController::UiDataTableController(
		UiDocument& document, UiEventRouter& router, UiDataTableConfig config)
		: UiComposedController(document, router), config(std::move(config)),
		  orderedRows(this->config.rows), virtualRows(this->config.estimatedRowHeight),
		  overscan(this->config.overscanExtent)
	{
		rootNode = makeNode(this->config.parent, "DataTable", "ui-data-table", {},
			UiSemanticRole::List);
		header = makeNode(rootNode, "TableHeader", "ui-table-header");
		body = makeNode(rootNode, "TableBody", "ui-table-body", {}, UiSemanticRole::List);
		(void)setColumns(std::move(this->config.columns));
		(void)setRows(std::move(this->config.rows));
	}

	Bool UiDataTableController::setRows(Vector<UiItemKey> rows)
	{
		if (rows.size() > maximumRows || !uniqueKeys(rows)) return false;
		config.rows = rows;
		orderedRows = std::move(rows);
		sortRows();
		(void)virtualRows.setKeys(orderedRows);
		materialize();
		return true;
	}

	Bool UiDataTableController::setColumns(Vector<UiTableColumn> columns)
	{
		if (columns.size() > maximumColumns) return false;
		Vector<UiItemKey> keys;
		for (UiTableColumn& column : columns)
		{
			if (!std::isfinite(column.width) || !std::isfinite(column.minimumWidth) ||
				!std::isfinite(column.maximumWidth)) return false;
			keys.push_back(column.key);
			column.minimumWidth = std::max(0.0f, column.minimumWidth);
			column.maximumWidth = std::max(column.minimumWidth, column.maximumWidth);
			column.width = std::clamp(column.width, column.minimumWidth, column.maximumWidth);
		}
		if (!uniqueKeys(keys)) return false;
		config.columns = std::move(columns);
		rebuildHeader();
		materialize();
		return true;
	}

	Bool UiDataTableController::setSort(UiTableSort sort, const Bool notify)
	{
		if (sort.direction != UiTableSortDirection::None)
		{
			const auto found = std::find_if(config.columns.begin(), config.columns.end(),
				[sort](const UiTableColumn& column)
				{ return column.key == sort.column && column.sortable && column.less; });
			if (found == config.columns.end()) return false;
		}
		currentSort = sort;
		sortRows();
		(void)virtualRows.setKeys(orderedRows);
		rebuildHeader();
		materialize();
		if (notify && config.onSortChanged) config.onSortChanged(sort);
		return true;
	}

	Bool UiDataTableController::resizeColumn(
		const UiItemKey column, const Float width, const Bool notify)
	{
		auto found = std::find_if(config.columns.begin(), config.columns.end(),
			[column](const UiTableColumn& value) { return value.key == column; });
		if (found == config.columns.end() || !found->resizable || !std::isfinite(width)) return false;
		found->width = std::clamp(width, found->minimumWidth, found->maximumWidth);
		rebuildHeader();
		materialize();
		if (notify && config.onColumnResized) config.onColumnResized(column, found->width);
		return true;
	}

	Bool UiDataTableController::select(const UiItemKey row, const Bool notify)
	{
		if (std::find(config.rows.begin(), config.rows.end(), row) == config.rows.end()) return false;
		selectedRow = row;
		materialize();
		if (notify && config.onSelectionChanged) config.onSelectionChanged(row);
		return true;
	}

	Bool UiDataTableController::updateViewport(
		const Float scrollOffset, const Float viewportExtent)
	{
		scroll.setMetrics(Vec2(0, virtualRows.totalExtent()), Vec2(0, viewportExtent));
		scroll.setOffset(Vec2(0, scrollOffset));
		materialize();
		return true;
	}

	void UiDataTableController::rebuildHeader()
	{
		if (document.isTraversing()) return;
		for (const UiNodeHandle node : headerNodes)
			destroyOwnedNode(node);
		headerNodes.clear();
		for (const UiTableColumn& column : config.columns)
		{
			const UiNodeHandle node = makeNode(header, "ColumnHeader", "ui-table-column-header",
				column.label, UiSemanticRole::Button, column.label);
			headerNodes.push_back(node);
			if (UiNode* value = document.get(node))
				value->state.semantics.value = std::to_string(column.key) + ":" +
					std::to_string(column.width);
			setClass(node, "is-sort-ascending", currentSort.column == column.key &&
				currentSort.direction == UiTableSortDirection::Ascending);
			setClass(node, "is-sort-descending", currentSort.column == column.key &&
				currentSort.direction == UiTableSortDirection::Descending);
			makeInteractive(node, column.sortable);
			listen(node, UiRoutedEventType::Activate,
				[this, key = column.key](UiRoutedEvent& event)
				{
					UiTableSort next{ key, UiTableSortDirection::Ascending };
					if (currentSort.column == key &&
						currentSort.direction == UiTableSortDirection::Ascending)
						next.direction = UiTableSortDirection::Descending;
					else if (currentSort.column == key &&
						currentSort.direction == UiTableSortDirection::Descending)
						next.direction = UiTableSortDirection::None;
					(void)setSort(next);
					event.handled = true;
				});
			if (column.resizable)
			{
				const UiNodeHandle resize = makeNode(node, "ResizeHandle",
					"ui-table-column-resize-handle", {}, UiSemanticRole::Slider,
					"Resize " + column.label);
				makeInteractive(resize);
				listen(resize, UiRoutedEventType::PointerDown,
					[](UiRoutedEvent& event)
					{
						event.capturePointer = true;
						event.focusTarget = true;
						event.handled = true;
						event.stopPropagation();
					});
				listen(resize, UiRoutedEventType::Drag,
					[this, key = column.key](UiRoutedEvent& event)
					{
						const auto found = std::find_if(config.columns.begin(),
							config.columns.end(), [key](const UiTableColumn& value)
							{ return value.key == key; });
						if (found != config.columns.end())
							(void)resizeColumn(key,
								found->width + static_cast<Float>(event.delta.x));
						event.handled = true;
						event.stopPropagation();
					});
				listen(resize, UiRoutedEventType::PointerUp,
					[](UiRoutedEvent& event)
					{
						event.releasePointer = true;
						event.handled = true;
						event.stopPropagation();
					});
			}
		}
	}

	void UiDataTableController::sortRows()
	{
		orderedRows = config.rows;
		if (currentSort.direction == UiTableSortDirection::None) return;
		const auto column = std::find_if(config.columns.begin(), config.columns.end(),
			[this](const UiTableColumn& value) { return value.key == currentSort.column; });
		if (column == config.columns.end() || !column->less) return;
		const auto less = column->less;
		const Bool reverse = currentSort.direction == UiTableSortDirection::Descending;
		std::stable_sort(orderedRows.begin(), orderedRows.end(),
			[less, reverse](const UiItemKey left, const UiItemKey right)
			{
				if (less(left, right)) return !reverse;
				if (less(right, left)) return reverse;
				return left < right;
			});
	}

	void UiDataTableController::materialize()
	{
		if (document.isTraversing()) return;
		for (const UiNodeHandle node : materialized)
			destroyOwnedNode(node);
		materialized.clear();
		const UiVirtualRange range = virtualRows.visibleRange(
			scroll.offset().y, scroll.viewport().y, overscan);
		for (UInt index = range.first; index < range.last && index < orderedRows.size(); ++index)
		{
			const UiItemKey row = orderedRows[index];
			const UiNodeHandle rowNode = makeNode(body, "TableRow", "ui-table-row", {},
				UiSemanticRole::ListItem);
			materialized.push_back(rowNode);
			if (UiNode* value = document.get(rowNode))
			{
				value->state.semantics.value = std::to_string(row);
				value->state.semantics.selected = row == selectedRow;
			}
			setClass(rowNode, UiWidgetClasses::selected, row == selectedRow);
			makeInteractive(rowNode);
			listen(rowNode, UiRoutedEventType::Activate,
				[this, row](UiRoutedEvent& event)
				{ (void)select(row); event.handled = true; });
			for (const UiTableColumn& column : config.columns)
			{
				const String text = column.value ? column.value(row) : String{};
				const UiNodeHandle cell = makeNode(rowNode, "TableCell", "ui-table-cell",
					text, UiSemanticRole::Text);
				if (UiNode* value = document.get(cell))
					value->state.semantics.value = std::to_string(column.key) + ":" +
						std::to_string(column.width);
			}
		}
	}

	UiSplitPaneController::UiSplitPaneController(
		UiDocument& document, UiEventRouter& router, UiSplitPaneConfig config)
		: UiComposedController(document, router), config(std::move(config)),
		  currentRatio(std::clamp(this->config.ratio, 0.0f, 1.0f))
	{
		rootNode = makeNode(this->config.parent, "SplitPane",
			this->config.orientation == UiSplitOrientation::Horizontal ?
			"ui-split-pane-horizontal" : "ui-split-pane-vertical");
		first = makeNode(rootNode, "Pane", "ui-split-pane-first");
		handle = makeNode(rootNode, "ResizeHandle", "ui-resize-handle",
			{}, UiSemanticRole::Slider, "Resize panes");
		second = makeNode(rootNode, "Pane", "ui-split-pane-second");
		makeInteractive(handle);
		listen(handle, UiRoutedEventType::PointerDown,
			[this](UiRoutedEvent& event)
			{
				(void)setRatio(pointerRatio(event));
				event.capturePointer = true;
				event.focusTarget = true;
				event.handled = true;
			});
		listen(handle, UiRoutedEventType::Drag,
			[this](UiRoutedEvent& event)
			{ (void)setRatio(pointerRatio(event)); event.handled = true; });
		listen(handle, UiRoutedEventType::PointerUp,
			[](UiRoutedEvent& event) { event.releasePointer = true; });
		sync();
	}

	Bool UiSplitPaneController::setRatio(Float ratio, const Bool notify)
	{
		if (!std::isfinite(ratio)) return false;
		const UiNode* node = document.get(rootNode);
		const Float extent = node ? (config.orientation == UiSplitOrientation::Horizontal ?
			node->computed.size.x : node->computed.size.y) : 0;
		Float minimum = 0;
		Float maximum = 1;
		if (extent > 0)
		{
			minimum = std::clamp(config.minimumFirst / extent, 0.0f, 1.0f);
			maximum = std::clamp(1.0f - config.minimumSecond / extent, 0.0f, 1.0f);
			if (minimum > maximum) ratio = config.minimumFirst <= config.minimumSecond ? minimum : maximum;
			else ratio = std::clamp(ratio, minimum, maximum);
		}
		else ratio = std::clamp(ratio, 0.0f, 1.0f);
		currentRatio = ratio;
		sync();
		if (notify && config.onRatioChanged) config.onRatioChanged(currentRatio);
		return true;
	}

	Float UiSplitPaneController::pointerRatio(const UiRoutedEvent& event) const
	{
		const UiNode* node = document.get(rootNode);
		if (!node) return currentRatio;
		const Bool horizontal = config.orientation == UiSplitOrientation::Horizontal;
		const Float extent = horizontal ? node->computed.size.x : node->computed.size.y;
		const Double start = horizontal ? node->computed.position.x : node->computed.position.y;
		const Double pointer = horizontal ? event.position.x : event.position.y;
		return extent > 0 ? static_cast<Float>((pointer - start) / extent) : currentRatio;
	}

	void UiSplitPaneController::sync()
	{
		if (UiNode* node = document.get(rootNode))
			node->state.semantics.value = std::to_string(currentRatio);
		if (UiNode* node = document.get(first))
			node->state.semantics.value = std::to_string(currentRatio);
		if (UiNode* node = document.get(second))
			node->state.semantics.value = std::to_string(1.0f - currentRatio);
		document.markDirty(rootNode, DirtyFlag::Style);
		document.markDirty(rootNode, DirtyFlag::Arrange);
	}

	UiDockWorkspaceModel::UiDockWorkspaceModel()
	{
		workspace.root = allocateTabs(0);
	}

	UiDockNodeData* UiDockWorkspaceModel::node(const UiDockNodeId id)
	{
		const auto found = std::find_if(workspace.nodes.begin(), workspace.nodes.end(),
			[id](const UiDockNodeData& value) { return value.id == id; });
		return found == workspace.nodes.end() ? nullptr : &*found;
	}

	const UiDockNodeData* UiDockWorkspaceModel::node(const UiDockNodeId id) const
	{
		const auto found = std::find_if(workspace.nodes.begin(), workspace.nodes.end(),
			[id](const UiDockNodeData& value) { return value.id == id; });
		return found == workspace.nodes.end() ? nullptr : &*found;
	}

	UiDockNodeData* UiDockWorkspaceModel::owner(const UiDockPanelId panel)
	{
		for (UiDockNodeData& value : workspace.nodes)
			if (value.kind == UiDockNodeKind::Tabs &&
				std::find(value.tabs.begin(), value.tabs.end(), panel) != value.tabs.end())
				return &value;
		return nullptr;
	}

	Bool UiDockWorkspaceModel::containsPanel(const UiDockPanelId panel) const
	{
		if (panel == 0) return false;
		if (std::find(workspace.floatingPanels.begin(), workspace.floatingPanels.end(), panel) !=
			workspace.floatingPanels.end()) return true;
		for (const UiDockNodeData& value : workspace.nodes)
			if (std::find(value.tabs.begin(), value.tabs.end(), panel) != value.tabs.end()) return true;
		return false;
	}

	UiDockNodeId UiDockWorkspaceModel::allocateTabs(const UiDockPanelId panel)
	{
		if (workspace.nodes.size() >= maximumNodes) return 0;
		while (node(nextNodeId)) ++nextNodeId;
		UiDockNodeData value;
		value.id = nextNodeId++;
		if (panel != 0) { value.tabs.push_back(panel); value.selected = panel; }
		workspace.nodes.push_back(std::move(value));
		return workspace.nodes.back().id;
	}

	Bool UiDockWorkspaceModel::load(UiDockWorkspaceData data)
	{
		const UiDockWorkspaceData old = workspace;
		const UiDockNodeId oldNext = nextNodeId;
		workspace = std::move(data);
		nextNodeId = 1;
		for (const UiDockNodeData& value : workspace.nodes)
			nextNodeId = std::max(nextNodeId, value.id + 1);
		if (validate()) return true;
		workspace = old;
		nextNodeId = oldNext;
		return false;
	}

	Bool UiDockWorkspaceModel::dock(
		const UiDockPanelId panel, const UiDockNodeId target,
		const UiDockSide side, Float ratio)
	{
		UiDockNodeData* targetNode = node(target);
		if (panel == 0 || containsPanel(panel) || !targetNode ||
			targetNode->kind != UiDockNodeKind::Tabs ||
			workspace.nodes.size() + (side == UiDockSide::Tab ? 0 : 2) > maximumNodes)
			return false;
		if (side == UiDockSide::Tab)
		{
			if (targetNode->tabs.size() >= maximumPanels) return false;
			targetNode->tabs.push_back(panel);
			targetNode->selected = panel;
			return true;
		}
		const UiDockNodeId oldParent = targetNode->parent;
		const UiDockNodeId newTabs = allocateTabs(panel);
		if (newTabs == 0) return false;
		while (node(nextNodeId)) ++nextNodeId;
		UiDockNodeData split;
		split.id = nextNodeId++;
		split.parent = oldParent;
		split.kind = UiDockNodeKind::Split;
		split.orientation = side == UiDockSide::Left || side == UiDockSide::Right ?
			UiSplitOrientation::Horizontal : UiSplitOrientation::Vertical;
		split.ratio = std::clamp(ratio, minimumRatio, maximumRatio);
		const Bool before = side == UiDockSide::Left || side == UiDockSide::Top;
		split.first = before ? newTabs : target;
		split.second = before ? target : newTabs;
		workspace.nodes.push_back(split);
		node(target)->parent = split.id;
		node(newTabs)->parent = split.id;
		if (oldParent == 0) workspace.root = split.id;
		else if (UiDockNodeData* parent = node(oldParent))
		{
			if (parent->first == target) parent->first = split.id;
			else if (parent->second == target) parent->second = split.id;
			else return false;
		}
		return true;
	}

	Bool UiDockWorkspaceModel::undock(const UiDockPanelId panel)
	{
		UiDockNodeData* source = owner(panel);
		if (!source || workspace.floatingPanels.size() >= maximumPanels) return false;
		const UiDockNodeId sourceId = source->id;
		std::erase(source->tabs, panel);
		if (source->selected == panel)
			source->selected = source->tabs.empty() ? 0 : source->tabs.front();
		workspace.floatingPanels.push_back(panel);
		if (source->tabs.empty()) removeEmpty(sourceId);
		return true;
	}

	Bool UiDockWorkspaceModel::move(const UiDockPanelId panel, const UiDockNodeId target,
		const UiDockSide side, const Float ratio)
	{
		if (!containsPanel(panel) || !node(target)) return false;
		UiDockWorkspaceData backup = workspace;
		UiDockNodeData* source = owner(panel);
		if (source)
		{
			const UiDockNodeId sourceId = source->id;
			std::erase(source->tabs, panel);
			if (source->selected == panel)
				source->selected = source->tabs.empty() ? 0 : source->tabs.front();
			if (source->tabs.empty()) removeEmpty(sourceId);
		}
		else std::erase(workspace.floatingPanels, panel);
		if (dock(panel, target, side, ratio)) return true;
		workspace = std::move(backup);
		return false;
	}

	Bool UiDockWorkspaceModel::select(
		const UiDockNodeId tabs, const UiDockPanelId panel)
	{
		UiDockNodeData* value = node(tabs);
		if (!value || value->kind != UiDockNodeKind::Tabs ||
			std::find(value->tabs.begin(), value->tabs.end(), panel) == value->tabs.end())
			return false;
		value->selected = panel;
		return true;
	}

	Bool UiDockWorkspaceModel::setSplitRatio(
		const UiDockNodeId split, Float ratio, const Float availableExtent)
	{
		UiDockNodeData* value = node(split);
		if (!value || value->kind != UiDockNodeKind::Split || !std::isfinite(ratio)) return false;
		Float low = minimumRatio;
		Float high = maximumRatio;
		if (availableExtent > 0)
		{
			low = std::max(low, value->minimumFirst / availableExtent);
			high = std::min(high, 1.0f - value->minimumSecond / availableExtent);
		}
		value->ratio = low <= high ? std::clamp(ratio, low, high) :
			(value->minimumFirst <= value->minimumSecond ? low : high);
		return true;
	}

	void UiDockWorkspaceModel::removeEmpty(const UiDockNodeId id)
	{
		UiDockNodeData* empty = node(id);
		if (!empty || empty->kind != UiDockNodeKind::Tabs || !empty->tabs.empty() ||
			id == workspace.root) return;
		const UiDockNodeId parentId = empty->parent;
		UiDockNodeData* parent = node(parentId);
		if (!parent || parent->kind != UiDockNodeKind::Split) return;
		const UiDockNodeId sibling = parent->first == id ? parent->second : parent->first;
		const UiDockNodeId grandparent = parent->parent;
		if (UiDockNodeData* siblingNode = node(sibling)) siblingNode->parent = grandparent;
		if (grandparent == 0) workspace.root = sibling;
		else if (UiDockNodeData* grand = node(grandparent))
		{
			if (grand->first == parentId) grand->first = sibling;
			else if (grand->second == parentId) grand->second = sibling;
		}
		workspace.nodes.erase(std::remove_if(workspace.nodes.begin(), workspace.nodes.end(),
			[id, parentId](const UiDockNodeData& value)
			{ return value.id == id || value.id == parentId; }), workspace.nodes.end());
	}

	Bool UiDockWorkspaceModel::validate() const
	{
		if (workspace.version != 1 || workspace.nodes.empty() ||
			workspace.nodes.size() > maximumNodes || !node(workspace.root)) return false;
		std::set<UiDockNodeId> ids;
		std::set<UiDockPanelId> panels;
		for (const UiDockPanelId panel : workspace.floatingPanels)
			if (panel == 0 || !panels.insert(panel).second) return false;
		for (const UiDockNodeData& value : workspace.nodes)
		{
			if (value.id == 0 || !ids.insert(value.id).second) return false;
			if (value.kind == UiDockNodeKind::Tabs)
			{
				if (value.first != 0 || value.second != 0) return false;
				for (const UiDockPanelId panel : value.tabs)
					if (panel == 0 || !panels.insert(panel).second) return false;
				if (value.selected != 0 &&
					std::find(value.tabs.begin(), value.tabs.end(), value.selected) ==
					value.tabs.end()) return false;
			}
			else if (!node(value.first) || !node(value.second) || value.first == value.second ||
				!std::isfinite(value.ratio) || !std::isfinite(value.minimumFirst) ||
				!std::isfinite(value.minimumSecond) || value.minimumFirst < 0 ||
				value.minimumSecond < 0 || value.ratio < minimumRatio ||
				value.ratio > maximumRatio) return false;
		}
		if (panels.size() > maximumPanels) return false;
		std::set<UiDockNodeId> visited;
		Function<Bool(UiDockNodeId, UiDockNodeId, UInt)> visit =
			[this, &visited, &visit](const UiDockNodeId id,
				const UiDockNodeId expectedParent, const UInt depth)
			{
				if (depth > maximumDepth || !visited.insert(id).second) return false;
				const UiDockNodeData* value = node(id);
				if (!value || value->parent != expectedParent) return false;
				return value->kind == UiDockNodeKind::Tabs ||
					(visit(value->first, id, depth + 1) &&
					 visit(value->second, id, depth + 1));
			};
		return visit(workspace.root, 0, 0) && visited.size() == workspace.nodes.size();
	}
}
