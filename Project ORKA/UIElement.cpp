#include "UIElement.hpp"
#include "Renderer.hpp"
#include "RenderingOverrides.hpp"
#include "UserInterface.hpp"

Index nextInteractiveElementId = 0;
UIElement::~UIElement() = default;

UIElement& UIElement::column()
{
	isVertical = true;
	return *this;
}

UIElement& UIElement::row()
{
	isVertical = false;
	return *this;
}

UInt UIElement::getId() const
{
	return id;
}

const String& UIElement::getDebugName() const
{
	return debugName;
}

Int UIElement::getWidth() const
{
	return width;
}

Bool UIElement::isEmpty() const
{
	return children.empty();
}

Int UIElement::getHeight() const
{
	return height;
}

IVec2 UIElement::getSize() const
{
	return { width, height };
}

IVec2 UIElement::getPosition() const
{
	return { positionX, positionY };
}

Color UIElement::getHoverColor() const
{
	return hoveredBackgroundColor;
}

Bool UIElement::isPointInside(const Window& window, const DVec2& point) const
{
	const IVec2 renderPos = IVec2(positionX, positionY + window.getHeight());
	const Int   left = renderPos.x;
	const Int   right = renderPos.x + width;
	const Int   top = renderPos.y;
	const Int   bottom = renderPos.y - height;

	return point.x >= left && point.x <= right && point.y <= top && point.y >= bottom;
}

void UIElement::setHovered(Bool newState) {
	hovered = newState;
}

DVec2 UIElement::getMousePosTopLeft(const Window& window) const
{
	const DVec2 topLeft(positionX, positionY + window.getHeight());
	const DVec2 mouse = window.mousePositionBotLeft();

	return { mouse.x - topLeft.x, topLeft.y - mouse.y };
}

DVec2 UIElement::getMousePosBottomLeft(const Window& window) const
{
	const DVec2 bottomLeft(positionX, positionY + window.getHeight() - height);
	const DVec2 mouse = window.mousePositionBotLeft();

	return { mouse.x - bottomLeft.x, mouse.y - bottomLeft.y };
}

UIElement* UIElement::findDeepestAt(Window& window, const DVec2& point)
{
	if (!isPointInside(window, point)) return nullptr;

	for (auto it = children.rbegin(); it != children.rend(); ++it)
	{
		if (UIElement* found = (*it)->findDeepestAt(window, point)) return found;
	}
	return this;
}

void UIElement::update(Window& window)
{
	for (UIElement* c : children) c->update(window);
}

void UIElement::create(Window& window)
{
	for (UIElement* c : children) c->create(window);
}

void UIElement::render(Window& window)
{
	Renderer& r = window.renderer;
	WireframeOverride wireframeGuard(r, false); // ui should render normally, even in wireframe mode
	DepthTestOverride depthGuard(r, false); //ui renders depth independent
	r.uiSpace(window); // set coordinates system to pixel space
	r.setRenderRegion(IVec2(0), window.getSize()); // only render in that region
	const IVec2 backgroundPos = getPosition() + IVec2(0, window.getHeight()); // y is negative to avoid flipping, so height is added
	const IVec2 backgroundSize = getSize();

	//render background
	Color bg = backgroundColor;
	if (hovered) bg = hoveredBackgroundColor;
	if (active) bg = activeBackgroundColor;
	if (bg.a > 0) // if background color is fully transparent, don't render it
	{
		r.useShader("color");
		r.fill(bg);
		r.UIRectangle(backgroundPos, backgroundSize);
	}

	//render border
	Int bt = borderThickness;
	if (focused) bt = borderThicknessFocused;
	if (bt > 0)
	{
		if (backgroundSize.x <= 0 || backgroundSize.y <= 0) return;
		const Int thickness = min(borderThickness, min(backgroundSize.x, backgroundSize.y));
		if (thickness <= 0) return;

		const Int sideHeight = max(backgroundSize.y - thickness * 2, 0);
		const Int bottomOffset = max(backgroundSize.y - thickness, 0);
		const Int rightOffset = max(backgroundSize.x - thickness, 0);

		r.fill(borderCol);
		r.uniforms().setCustomColor(borderCol);
		r.UIRectangle(backgroundPos, IVec2(backgroundSize.x, thickness)); // top
		r.UIRectangle(backgroundPos + IVec2(0, -bottomOffset), IVec2(backgroundSize.x, thickness)); // bottom

		if (sideHeight > 0)
		{
			r.UIRectangle(backgroundPos + IVec2(0, -thickness), IVec2(thickness, sideHeight)); // left
			r.UIRectangle(backgroundPos + IVec2(rightOffset, -thickness), IVec2(thickness, sideHeight)); // right
		}
	}

	//render text
	if (!text.empty())
	{
		r.useShader("text");
		r.uniforms().setMMatrix(1);
		r.fill(textColor);
		r.textRenderSystem.alignText(Alignment::left, Alignment::bottom);
		const Vec2 textPosition = Vec2(backgroundPos + IVec2(paddingX, -paddingY));
		r.textRenderSystem.render(r, text, textPosition);
	}

	//render children
	for (const auto i : children) i->render(window);
}

void UIElement::destroy(Window& window)
{
	for (UIElement* c : children) c->destroy(window);
}

UIElement& UIElement::gap(const Int width)
{
	gapX = width;
	gapY = width;
	return *this;
}

UIElement& UIElement::color(const Color col)
{
	textColor = col;
	return *this;
}

UIElement& UIElement::padding(const Int width)
{
	paddingX = width;
	paddingY = width;
	return *this;
}

UIElement& UIElement::growX(const Bool doesGrow)
{
	this->growsInX = doesGrow;
	return *this;
}

UIElement& UIElement::growY(const Bool doesGrow)
{
	this->growsInY = doesGrow;
	return *this;
}

UIElement& UIElement::setPositionX(const Int x)
{
	positionX = x;
	return *this;
}

UIElement& UIElement::setPositionY(const Int y)
{
	positionY = y;
	return *this;
}

void UIElement::dropElement(UIElement* element)
{
	element->parent = nullptr;
	for (Int i = 0; i < children.size(); i++) if (children[i] == element) children.erase(children.begin() + i);
}

UIElement& UIElement::setWidth(const Int width)
{
	this->width = width;
	return *this;
}

void UIElement::renderInteractive(Window& window)
{
	for (UInt i = 0; i < children.size(); i++)
	{
		children[i]->renderInteractive(window);
	}
}

UIElement& UIElement::setHeight(const Int height)
{
	this->height = height;
	return *this;
}

UIElement& UIElement::setText(const String& text)
{
	this->text = text;
	return *this;
}

UIElement& UIElement::background(const Color color)
{
	backgroundColor = color;
	return *this;
}

UIElement& UIElement::hoveredBackground(const Color color)
{
	hoveredBackgroundColor = color;
	return *this;
}

UIElement& UIElement::activeBackground(const Color color)
{
	activeBackgroundColor = color;
	return *this;
}

UIElement& UIElement::borderWidth(const Int width)
{
	borderThickness = max(0, width);
	return *this;
}

UIElement& UIElement::borderColor(const Color color)
{
	borderCol = color;
	return *this;
}

UIElement& UIElement::border(const Int width, const Color color)
{
	return borderWidth(width).borderColor(color);
}

UIElement& UIElement::setName(const String& debugName)
{
	this->debugName = debugName;
	return *this;
}

UIElement& UIElement::addElement(UIElement& element)
{
	if (element.parent != nullptr) logError("UI Element (" + debugName + ") already has parent!");
	children.push_back(&element);
	element.parent = this;
	return *this;
}

void UIElement::inputEvent(Window& window, InputEvent& input){}

void UIElement::onMouseEnter(Window& window){}

void UIElement::onMouseLeave(Window& window){}

void UIElement::growSizes()
{
	if (children.empty()) return;

	Vector<UIElement*> growChildrenX;
	Vector<UIElement*> growChildrenY;
	for (const auto i : children)
	{
		if (i->growsInX) growChildrenX.push_back(i);
		if (i->growsInY) growChildrenY.push_back(i);
	}

	const auto distribute = [](Vector<UIElement*>& growChildren, Int freeSpace, const Bool useWidth)
		{
			while (freeSpace != 0 && !growChildren.empty())
			{
				if (freeSpace > 0)
				{
					UIElement* smallestChild = growChildren[0];
					for (const auto i : growChildren)
					{
						if ((useWidth ? i->getWidth() : i->getHeight()) < (useWidth ? smallestChild->getWidth() : smallestChild->getHeight())) smallestChild = i;
					}
					if (useWidth) smallestChild->width += 1;
					else smallestChild->height += 1;
					freeSpace -= 1;
				}
				else
				{
					UIElement* largestChild = growChildren[0];
					for (const auto i : growChildren)
					{
						if ((useWidth ? i->getWidth() : i->getHeight()) > (useWidth ? largestChild->getWidth() : largestChild->getHeight())) largestChild = i;
					}
					Int& dimension = useWidth ? largestChild->width : largestChild->height;
					if (dimension > 0)
					{
						dimension -= 1;
						freeSpace += 1;
					}
					else break;
				}
			}
		};

	if (!isVertical)
	{
		// main axis: X (row layout) -> distribute width across children that grow
		if (!growChildrenX.empty()) distribute(growChildrenX, width - contentWidth, true);

		// cross axis: Y should not be split, let growing children fill the available height
		const Int targetHeight = max(0, height - paddingY * 2);
		for (auto* child : growChildrenY) child->height = targetHeight;
		contentHeight = max(contentHeight, targetHeight + paddingY * 2);
	}
	else
	{
		// main axis: Y (column layout) -> distribute height across children that grow
		if (!growChildrenY.empty()) distribute(growChildrenY, height - contentHeight, false);

		// cross axis: X should not be split, let growing children fill the available width
		const Int targetWidth = max(0, width - paddingX * 2);
		for (auto* child : growChildrenX) child->width = targetWidth;
		contentWidth = max(contentWidth, targetWidth + paddingX * 2);
	}

	for (const auto i : children) i->growSizes();
}

void UIElement::updateSizes()
{
	if (children.empty())
	{
		contentWidth = width;
		contentHeight = height;
	}
	else
	{
		if (parent)
		{
			width = 0;
			height = 0;
		}
		contentWidth = paddingX * 2;
		contentHeight = paddingY * 2;
	}

	for (const auto i : children) i->updateSizes();

	if (isVertical) contentHeight += gapY * (children.size() - 1);
	else contentWidth += gapX * (children.size() - 1);

	if (parent)
	{
		if (!growsInX) setWidth(contentWidth);
		if (!growsInY) setHeight(contentHeight);

		if (parent->isVertical)
		{
			parent->contentWidth = max(parent->contentWidth, width);
			parent->contentHeight += height;
		}
		else
		{
			parent->contentHeight = max(parent->contentHeight, height);
			parent->contentWidth += width;
		}
	}
}

void UIElement::updatePositions()
{
	// padding at least centers the content, when the container is too small
	const Int actualPaddingX = min(paddingX, width / 2);
	const Int actualPaddingY = min(paddingY, height / 2);

	Int xOffset = positionX + actualPaddingX;
	Int yOffset = positionY - actualPaddingY;

	if (isVertical)
	{
		for (const auto i : children)
		{
			i->setPositionX(xOffset);
			i->setPositionY(yOffset);
			// Child padding is already accounted for in its own size; do not
			// compound it here or gaps will depend on sibling padding.
			yOffset -= i->getHeight() + gapY;
		}
	}
	else
	{
		for (const auto i : children)
		{
			i->setPositionX(xOffset);
			i->setPositionY(yOffset);
			// Avoid double-counting padding when spacing siblings in a row.
			xOffset += i->getWidth() + gapX;
		}
	}

	for (const auto i : children) i->updatePositions();
}
