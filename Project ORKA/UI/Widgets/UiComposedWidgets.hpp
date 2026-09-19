#pragma once

#include "UiWidgets.hpp"

namespace Ui
{
	using UiItemKey = U64;
	using UiItemCallback = Function<void(UiItemKey)>;

	// Shared lifetime and authored-node plumbing for composed controllers.
	class UiComposedController
	{
	public:
		static constexpr SizeT maximumOwnedNodes = 65536;

		UiComposedController(UiDocument& document, UiEventRouter& router);
		virtual ~UiComposedController();
		UiComposedController(const UiComposedController&) = delete;
		UiComposedController& operator=(const UiComposedController&) = delete;

		[[nodiscard]] UiNodeHandle root() const { return rootNode; }

	protected:
		UiDocument& document;
		UiEventRouter& router;
		UiNodeHandle rootNode;
		Vector<UiNodeHandle> ownedNodes;
		Vector<UiListenerToken> listeners;
		Vector<UiNodeHandle> listenerNodes;

		[[nodiscard]] UiNodeHandle makeNode(UiNodeHandle parent, String type, String styleClass,
			String text = {}, UiSemanticRole role = UiSemanticRole::None, String label = {});
		void makeInteractive(UiNodeHandle node, Bool focusable = true, Int tabIndex = 0,
			Bool focusScope = false, Bool modal = false);
		void listen(UiNodeHandle node, UiRoutedEventType type, UiEventCallback callback);
		void setClass(UiNodeHandle node, const String& styleClass, Bool present);
		void destroyOwnedNode(UiNodeHandle node);
		void clearOwnedChildren();
	};

	struct UiMenuItem
	{
		UiItemKey key = 0;
		UiItemKey parentKey = 0;
		String label;
		String command;
		Bool enabled = true;
		Bool separator = false;
		Bool checked = false;
	};

	enum class UiMenuPresentation : Byte { Menu, MenuBar, ContextMenu };

	struct UiMenuConfig
	{
		UiNodeHandle parent;
		Vector<UiMenuItem> items;
		UiMenuPresentation presentation = UiMenuPresentation::Menu;
		UiItemCallback onActivate;
		Function<void(Bool)> onOpenChanged;
	};

	class UiMenuController final : public UiComposedController
	{
	public:
		static constexpr SizeT maximumItems = 4096;
		UiMenuController(UiDocument& document, UiEventRouter& router, UiMenuConfig config);
		[[nodiscard]] Bool setItems(Vector<UiMenuItem> items);
		[[nodiscard]] Bool open(UiItemKey parentKey = 0);
		void close();
		[[nodiscard]] Bool isOpen() const { return opened; }
		[[nodiscard]] UiItemKey activeParent() const { return openParent; }

	private:
		UiMenuConfig config;
		Vector<UiNodeHandle> itemNodes;
		UiItemKey openParent = 0;
		Bool opened = false;
		void rebuild();
		void activate(SizeT index, UiRoutedEvent& event);
	};

	enum class UiPopupKind : Byte { Tooltip, Popover };
	struct UiPopupConfig
	{
		UiNodeHandle parent;
		UiNodeHandle anchor;
		String text;
		UiPopupKind kind = UiPopupKind::Tooltip;
		Bool interactive = false;
		Function<void(Bool)> onOpenChanged;
	};

	class UiPopupController final : public UiComposedController
	{
	public:
		UiPopupController(UiDocument& document, UiEventRouter& router, UiPopupConfig config);
		void show();
		void hide();
		[[nodiscard]] Bool isOpen() const { return opened; }
		void setText(String text);

	private:
		UiPopupConfig config;
		UiNodeHandle content;
		Bool opened = false;
	};

	struct UiDialogAction
	{
		UiItemKey key = 0;
		String label;
		Bool enabled = true;
		Bool defaultAction = false;
		Bool cancelAction = false;
	};

	enum class UiDialogCloseReason : Byte { Action, Cancelled, Programmatic };
	using UiDialogCloseCallback = Function<void(UiDialogCloseReason, UiItemKey)>;

	struct UiDialogConfig
	{
		UiNodeHandle parent;
		String title;
		Vector<UiDialogAction> actions;
		UiDialogCloseCallback onClose;
	};

	class UiDialogController final : public UiComposedController
	{
	public:
		static constexpr SizeT maximumActions = 64;
		UiDialogController(UiDocument& document, UiEventRouter& router, UiDialogConfig config);
		~UiDialogController() override;
		[[nodiscard]] UiNodeHandle contentNode() const { return content; }
		void show();
		void close(UiDialogCloseReason reason = UiDialogCloseReason::Programmatic,
			UiItemKey action = 0);
		[[nodiscard]] Bool isOpen() const { return opened; }

	private:
		UiDialogConfig config;
		UiNodeHandle content;
		Vector<UiNodeHandle> actionNodes;
		UiItemKey defaultAction = 0;
		UiItemKey cancelAction = 0;
		Bool opened = false;
		void invoke(UiItemKey key);
	};

	struct UiTabItem
	{
		UiItemKey key = 0;
		String label;
		Bool enabled = true;
		Bool closable = false;
	};
	using UiTabChangeCallback = Function<void(UiItemKey)>;

	struct UiTabsConfig
	{
		UiNodeHandle parent;
		Vector<UiTabItem> tabs;
		UiItemKey selected = 0;
		UiTabChangeCallback onChange;
		UiItemCallback onClose;
	};

	class UiTabsController final : public UiComposedController
	{
	public:
		static constexpr SizeT maximumTabs = 1024;
		UiTabsController(UiDocument& document, UiEventRouter& router, UiTabsConfig config);
		[[nodiscard]] Bool setTabs(Vector<UiTabItem> tabs);
		[[nodiscard]] Bool select(UiItemKey key, Bool notify = true);
		[[nodiscard]] UiItemKey selected() const { return selectedKey; }
		[[nodiscard]] UiNodeHandle panelNode() const { return panel; }

	private:
		UiTabsConfig config;
		Vector<UiNodeHandle> tabNodes;
		UiNodeHandle tabList;
		UiNodeHandle panel;
		UiItemKey selectedKey = 0;
		void rebuild();
		void sync();
	};

	struct UiTreeItem
	{
		UiItemKey key = 0;
		UiItemKey parentKey = 0;
		String label;
		Bool enabled = true;
	};
	struct UiTreeRow
	{
		UiItemKey key = 0;
		UInt depth = 0;
	};
	using UiTreeExpansionCallback = Function<void(UiItemKey, Bool)>;

	struct UiTreeViewConfig
	{
		UiNodeHandle parent;
		Vector<UiTreeItem> items;
		Float estimatedRowHeight = 24;
		Float overscanExtent = 48;
		UiItemCallback onSelectionChanged;
		UiTreeExpansionCallback onExpansionChanged;
	};

	class UiTreeViewController final : public UiComposedController
	{
	public:
		static constexpr UInt maximumItems = UiVirtualList::maximumItemCount;
		static constexpr UInt maximumDepth = 256;
		UiTreeViewController(UiDocument& document, UiEventRouter& router, UiTreeViewConfig config);
		[[nodiscard]] Bool setItems(Vector<UiTreeItem> items);
		[[nodiscard]] Bool setExpanded(UiItemKey key, Bool expanded, Bool notify = true);
		[[nodiscard]] Bool select(UiItemKey key, Bool notify = true);
		[[nodiscard]] Bool updateViewport(Float scrollOffset, Float viewportExtent);
		[[nodiscard]] Bool setMeasuredHeight(UiItemKey key, Float height);
		[[nodiscard]] const Vector<UiTreeRow>& visibleRows() const { return rows; }
		[[nodiscard]] UiItemKey selected() const { return selectedKey; }

	private:
		UiTreeViewConfig config;
		Map<UiItemKey, UiTreeItem> byKey;
		Map<UiItemKey, Vector<UiItemKey>> children;
		std::set<UiItemKey> expandedKeys;
		Vector<UiTreeRow> rows;
		Vector<UiNodeHandle> materialized;
		UiVirtualList virtualRows;
		UiScrollState scroll;
		UiItemKey selectedKey = 0;
		Float overscan = 48;
		void flatten();
		void append(UiItemKey parent, UInt depth);
		void materialize();
	};

	enum class UiTableSortDirection : Byte { None, Ascending, Descending };
	struct UiTableColumn
	{
		UiItemKey key = 0;
		String label;
		Float width = 120;
		Float minimumWidth = 24;
		Float maximumWidth = 4096;
		Bool sortable = true;
		Bool resizable = true;
		Function<String(UiItemKey)> value;
		Function<Bool(UiItemKey, UiItemKey)> less;
	};
	struct UiTableSort
	{
		UiItemKey column = 0;
		UiTableSortDirection direction = UiTableSortDirection::None;
	};
	using UiTableSortCallback = Function<void(UiTableSort)>;
	using UiColumnResizeCallback = Function<void(UiItemKey, Float)>;

	struct UiDataTableConfig
	{
		UiNodeHandle parent;
		Vector<UiItemKey> rows;
		Vector<UiTableColumn> columns;
		Float estimatedRowHeight = 24;
		Float overscanExtent = 48;
		UiItemCallback onSelectionChanged;
		UiTableSortCallback onSortChanged;
		UiColumnResizeCallback onColumnResized;
	};

	class UiDataTableController final : public UiComposedController
	{
	public:
		static constexpr UInt maximumRows = UiVirtualList::maximumItemCount;
		static constexpr SizeT maximumColumns = 256;
		UiDataTableController(UiDocument& document, UiEventRouter& router, UiDataTableConfig config);
		[[nodiscard]] Bool setRows(Vector<UiItemKey> rows);
		[[nodiscard]] Bool setColumns(Vector<UiTableColumn> columns);
		[[nodiscard]] Bool setSort(UiTableSort sort, Bool notify = true);
		[[nodiscard]] Bool resizeColumn(UiItemKey column, Float width, Bool notify = true);
		[[nodiscard]] Bool select(UiItemKey row, Bool notify = true);
		[[nodiscard]] Bool updateViewport(Float scrollOffset, Float viewportExtent);
		[[nodiscard]] UiTableSort sort() const { return currentSort; }

	private:
		UiDataTableConfig config;
		Vector<UiItemKey> orderedRows;
		Vector<UiNodeHandle> headerNodes;
		Vector<UiNodeHandle> materialized;
		UiNodeHandle header;
		UiNodeHandle body;
		UiVirtualList virtualRows;
		UiScrollState scroll;
		UiTableSort currentSort;
		UiItemKey selectedRow = 0;
		Float overscan = 48;
		void rebuildHeader();
		void sortRows();
		void materialize();
	};

	enum class UiSplitOrientation : Byte { Horizontal, Vertical };
	using UiSplitChangeCallback = Function<void(Float)>;
	struct UiSplitPaneConfig
	{
		UiNodeHandle parent;
		UiSplitOrientation orientation = UiSplitOrientation::Horizontal;
		Float ratio = 0.5f;
		Float minimumFirst = 0;
		Float minimumSecond = 0;
		UiSplitChangeCallback onRatioChanged;
	};

	class UiSplitPaneController final : public UiComposedController
	{
	public:
		UiSplitPaneController(UiDocument& document, UiEventRouter& router, UiSplitPaneConfig config);
		[[nodiscard]] UiNodeHandle firstPane() const { return first; }
		[[nodiscard]] UiNodeHandle secondPane() const { return second; }
		[[nodiscard]] UiNodeHandle handleNode() const { return handle; }
		[[nodiscard]] Bool setRatio(Float ratio, Bool notify = true);
		[[nodiscard]] Float ratio() const { return currentRatio; }

	private:
		UiSplitPaneConfig config;
		UiNodeHandle first;
		UiNodeHandle handle;
		UiNodeHandle second;
		Float currentRatio = 0.5f;
		Float pointerRatio(const UiRoutedEvent& event) const;
		void sync();
	};

	using UiDockNodeId = U64;
	using UiDockPanelId = U64;
	enum class UiDockNodeKind : Byte { Tabs, Split };

	struct UiDockNodeData
	{
		UiDockNodeId id = 0;
		UiDockNodeId parent = 0;
		UiDockNodeKind kind = UiDockNodeKind::Tabs;
		UiSplitOrientation orientation = UiSplitOrientation::Horizontal;
		Float ratio = 0.5f;
		Float minimumFirst = 0;
		Float minimumSecond = 0;
		UiDockNodeId first = 0;
		UiDockNodeId second = 0;
		Vector<UiDockPanelId> tabs;
		UiDockPanelId selected = 0;
	};

	struct UiDockWorkspaceData
	{
		UInt version = 1;
		UiDockNodeId root = 0;
		Vector<UiDockNodeData> nodes;
		Vector<UiDockPanelId> floatingPanels;
	};

	enum class UiDockSide : Byte { Tab, Left, Right, Top, Bottom };

	class UiDockWorkspaceModel
	{
	public:
		static constexpr SizeT maximumNodes = 4096;
		static constexpr SizeT maximumPanels = 16384;
		static constexpr UInt maximumDepth = 128;
		static constexpr Float minimumRatio = 0.01f;
		static constexpr Float maximumRatio = 0.99f;

		UiDockWorkspaceModel();
		[[nodiscard]] const UiDockWorkspaceData& data() const { return workspace; }
		[[nodiscard]] Bool load(UiDockWorkspaceData data);
		[[nodiscard]] Bool dock(UiDockPanelId panel, UiDockNodeId target,
			UiDockSide side = UiDockSide::Tab, Float ratio = 0.5f);
		[[nodiscard]] Bool undock(UiDockPanelId panel);
		[[nodiscard]] Bool move(UiDockPanelId panel, UiDockNodeId target,
			UiDockSide side = UiDockSide::Tab, Float ratio = 0.5f);
		[[nodiscard]] Bool select(UiDockNodeId tabs, UiDockPanelId panel);
		[[nodiscard]] Bool setSplitRatio(UiDockNodeId split, Float ratio, Float availableExtent);
		[[nodiscard]] Bool validate() const;

	private:
		UiDockWorkspaceData workspace;
		UiDockNodeId nextNodeId = 1;
		[[nodiscard]] UiDockNodeData* node(UiDockNodeId id);
		[[nodiscard]] const UiDockNodeData* node(UiDockNodeId id) const;
		[[nodiscard]] UiDockNodeData* owner(UiDockPanelId panel);
		[[nodiscard]] Bool containsPanel(UiDockPanelId panel) const;
		[[nodiscard]] UiDockNodeId allocateTabs(UiDockPanelId panel);
		void removeEmpty(UiDockNodeId id);
	};
}
