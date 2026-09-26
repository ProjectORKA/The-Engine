#pragma once
#include "Input.hpp"

extern Index nextInteractiveElementId;

struct Renderer;
struct Window;

struct UIElement
{
	UIElement* parent = nullptr;
	Vector<UIElement*> children;

	virtual      ~UIElement();
	virtual void update(Window& window);
	virtual void create(Window& window);
	virtual void render(Window& window);
	virtual void destroy(Window& window);
	virtual void renderInteractive(Window& window);
	virtual void inputEvent(Window& window, InputEvent& input);
	virtual void onMouseEnter(Window& window);
	virtual void onMouseLeave(Window& window);

	void growSizes();
	void updateSizes();
	void updatePositions();
	void setHovered(Bool newState);
	void dropElement(UIElement* element);

	UIElement& row();
	UIElement& column();
	UIElement& gap(Int width);
	UIElement& color(Color col);
	UIElement& padding(Int width);
	UIElement& setPositionX(Int x);
	UIElement& setPositionY(Int y);
	UIElement& setWidth(Int width);
	UIElement& growX(Bool doesGrow);
	UIElement& growY(Bool doesGrow);
	UIElement& setHeight(Int height);
	UIElement& background(Color color);
	UIElement& hoveredBackground(Color color);
	UIElement& activeBackground(Color color);
	UIElement& borderWidth(Int width);
	UIElement& borderColor(Color color);
	UIElement& border(Int width, Color color);
	UIElement& setText(const String& text);
	UIElement& setName(const String& debugName);
	UIElement& addElement(UIElement& element);

	[[nodiscard]] UInt          getId() const;
	[[nodiscard]] IVec2         getSize() const;
	[[nodiscard]] Bool          isEmpty() const;
	[[nodiscard]] Int           getWidth() const;
	[[nodiscard]] Int           getHeight() const;
	[[nodiscard]] IVec2         getPosition() const;
	[[nodiscard]] Color         getHoverColor() const;
	[[nodiscard]] const String& getDebugName() const;
	[[nodiscard]] DVec2         getMousePosTopLeft(const Window& window) const;
	[[nodiscard]] DVec2         getMousePosBottomLeft(const Window& window) const;
	[[nodiscard]] UIElement*    findDeepestAt(Window& window, const DVec2& point);
	[[nodiscard]] virtual Bool    isPointInside(const Window& window, const DVec2& point) const;

protected:
	String text;
	String debugName = "Unknown";
	Bool   hovered = false;
	Bool   active = false;
	Bool   focused = false;

	//background
	Color  backgroundColor = Color(1.0, 1.0, 1.0, 0.1);
	Color  hoveredBackgroundColor = Color(1.0, 1.0, 1.0, 0.1);
	Color  activeBackgroundColor = Color(1.0, 1.0, 1.0, 0.5);

	//border
	Color  borderCol = Color(1.0, 1.0, 1.0, 0.1);
	Int    borderThickness = 1;
	Int    borderThicknessFocused = 2;

	//text
	Color  textColor = Color(1.0, 1.0, 1.0, 1.0);

	Int    gapY = 0;
	Int    gapX = 0;
	Int    paddingX = 0;
	Int    paddingY = 0;
	Int    width = 0;
	Int    height = 0;
	Int    positionX = 0;
	Int    positionY = 0;
	Int    contentWidth = 0;
	Int    contentHeight = 0;

	Index  id = nextInteractiveElementId++;

	Bool   isVertical = true;
	Bool   growsInX = true;
	Bool   growsInY = true;
};
