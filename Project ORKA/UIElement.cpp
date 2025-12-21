#include "UIElement.hpp"
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
	return {width, height};
}

IVec2 UIElement::getPosition() const
{
	return {positionX, positionY};
}

Bool UIElement::isPointInside(const Window& window, const DVec2& point) const
{
	const IVec2 renderPos(positionX, positionY + window.getHeight());
	const Int   left   = renderPos.x;
	const Int   right  = renderPos.x + width;
	const Int   top    = renderPos.y;
	const Int   bottom = renderPos.y - height;

	return point.x >= left && point.x <= right && point.y <= top && point.y >= bottom;
}

DVec2 UIElement::getMousePosTopLeft(const Window& window) const
{
	const DVec2 topLeft(positionX, positionY + window.getHeight());
	const DVec2 mouse = window.mousePosBotLeft;

	return {mouse.x - topLeft.x, topLeft.y - mouse.y};
}

DVec2 UIElement::getMousePosBottomLeft(const Window& window) const
{
	const DVec2 bottomLeft(positionX, positionY + window.getHeight() - height);
	const DVec2 mouse = window.mousePosBotLeft;

	return {mouse.x - bottomLeft.x, mouse.y - bottomLeft.y};
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
	Renderer & r = window.renderer;

	WireframeOverride wireframeGuard(r, false);
	r.uiSpace(window);
	r.setRenderRegion(IVec2(0), window.getSize());
	
	if (hasBackground)
	{
		const IVec2 renderPos = getPosition() + IVec2(0, window.getHeight());
		DepthTestOverride override(r, false);
		r.useShader("color");

		const Bool hovered = isPointInside(window, window.mousePosBotLeft);
		const Color bg = lerp(backgroundColor, Color(1, 1, 1, 1), static_cast<Float>(hovered) * 0.1f);
		r.fill(bg);
		r.UIRectangle(renderPos, getSize());

		if (!text.empty())
		{
			r.useShader("text");
			r.uniforms().setMMatrix(1);
			r.fill(textColor);
			r.textRenderSystem.alignText(Alignment::left, Alignment::bottom);
			const Vec2 textPosition = Vec2(renderPos + IVec2(paddingX, -paddingY));
			r.textRenderSystem.render(r, text, textPosition);
		}
	}
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
	hasBackground   = true;
	backgroundColor = color;
	return *this;
}

UIElement& UIElement::name(const String& debugName)
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

void UIElement::inputEvent(Window& window, const InputEvent input)
{
	for (UIElement* c : children) c->inputEvent(window, input);
}

void UIElement::setDebugName(const String& name) {
	debugName = name;
}

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
		contentWidth  = width;
		contentHeight = height;
	}
	else
	{
		if (parent)
		{
			width  = 0;
			height = 0;
		}
		contentWidth  = paddingX * 2;
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
			yOffset -= i->getHeight() + i->paddingY * 2 + gapY;
		}
	}
	else
	{
		for (const auto i : children)
		{
			i->setPositionX(xOffset);
			i->setPositionY(yOffset);
			xOffset += i->getWidth() + i->paddingX * 2 + gapX;
		}
	}

	for (const auto i : children) i->updatePositions();
}
