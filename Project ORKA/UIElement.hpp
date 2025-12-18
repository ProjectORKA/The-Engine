#pragma once
#include "Input.hpp"

extern Index nextInteractiveElementId;

struct Renderer;
struct Window;

struct UIElement
{
	UIElement*         parent = nullptr;
	Vector<UIElement*> children;

	virtual      ~UIElement();
	virtual void update(Window& window);
	virtual void create(Window& window);
	virtual void render(Window& window);
	virtual void destroy(Window& window);
	virtual void renderInteractive(Window& window);
	virtual void inputEvent(Window& window, InputEvent input);

	void growSizes();
	void updateSizes();
	void updatePositions();
	void setDebugName(const String& name);
	void dropElement(UIElement* element);

	UIElement& column();
	UIElement& row();
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
	UIElement& setText(const String& text);
	UIElement& name(const String& debugName);
	UIElement& addElement(UIElement& element);

	[[nodiscard]] UInt  getId() const;
	[[nodiscard]] IVec2 getSize() const;
	[[nodiscard]] Bool  isEmpty() const;
	[[nodiscard]] Int   getWidth() const;
	[[nodiscard]] Int   getHeight() const;
	[[nodiscard]] IVec2 getPosition() const;
	[[nodiscard]] Bool  isPointInside(const Window& window, const DVec2& point) const;

private:
	String text;
	String debugName       = "Unknown";


	Color  backgroundColor = Color(0, 0, 0, 0);
	Color  textColor       = Color(0, 0, 0, 1);

	Int    gapY            = 0;
	Int    gapX            = 0;
	Int    paddingX        = 0;
	Int    paddingY        = 0;
	Int    width           = 0;
	Int    height          = 0;
	Int    positionX       = 0;
	Int    positionY       = 0;
	Int    contentWidth    = 0;
	Int    contentHeight   = 0;

	Index  id              = nextInteractiveElementId++;

	Bool   isVertical      = true;
	Bool   growsInX        = true;
	Bool   growsInY        = true;
	Bool   hasBackground   = false;
};
