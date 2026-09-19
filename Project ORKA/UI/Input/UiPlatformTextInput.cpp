#include "UiPlatformTextInput.hpp"
#include "../../WindowAPI.hpp"

namespace Ui
{
	String UiGlfwClipboard::readText() const
	{
		if (!apiWindow) return {};
		const Char* text = glfwGetClipboardString(apiWindow);
		return text ? String(text) : String{};
	}

	void UiGlfwClipboard::writeText(const String& text)
	{
		if (!apiWindow) return;
		glfwSetClipboardString(apiWindow, text.c_str());
	}

	void UiPlatformTextInputBridge::setEventSink(EventSink sink)
	{
		eventSink = std::move(sink);
	}

	void UiPlatformTextInputBridge::clearEventSink()
	{
		eventSink = {};
		compositionActive = false;
	}

	Bool UiPlatformTextInputBridge::unicodeCodepoint(const U32 codepoint) const
	{
		if (codepoint > 0x10ffff || (codepoint >= 0xd800 && codepoint <= 0xdfff)) return false;
		UiEvent event;
		event.type = UiEventType::Text;
		event.codepoint = codepoint;
		return send(std::move(event));
	}

	Bool UiPlatformTextInputBridge::windowFocusChanged(const Bool focused)
	{
		if (!focused && compositionActive) (void)compositionCancel();
		UiEvent event;
		event.type = UiEventType::Focus;
		event.focused = focused;
		return send(std::move(event));
	}

	Bool UiPlatformTextInputBridge::compositionStart()
	{
		if (compositionActive) return false;
		compositionActive = true;
		UiEvent event;
		event.type = UiEventType::CompositionStart;
		return send(std::move(event));
	}

	Bool UiPlatformTextInputBridge::compositionUpdate(String utf8, const SizeT selectionStart,
		const SizeT selectionEnd)
	{
		if (!compositionActive) (void)compositionStart();
		UiEvent event;
		event.type = UiEventType::CompositionUpdate;
		event.text = std::move(utf8);
		event.compositionStart = selectionStart;
		event.compositionEnd = selectionEnd;
		return send(std::move(event));
	}

	Bool UiPlatformTextInputBridge::compositionCommit(String utf8)
	{
		if (!compositionActive) (void)compositionStart();
		UiEvent event;
		event.type = UiEventType::CompositionCommit;
		event.text = std::move(utf8);
		compositionActive = false;
		return send(std::move(event));
	}

	Bool UiPlatformTextInputBridge::compositionCancel()
	{
		if (!compositionActive) return false;
		compositionActive = false;
		UiEvent event;
		event.type = UiEventType::CompositionCancel;
		return send(std::move(event));
	}

	Bool UiPlatformTextInputBridge::send(UiEvent event) const
	{
		return eventSink && eventSink(std::move(event));
	}

	Bool UiTextEditCommandRouter::route(UiRoutedEvent& event, UiTextEditModel& model,
		UiClipboard* clipboard) const
	{
		constexpr Int keyBackspace = 259;
		constexpr Int keyEnter = 257;
		constexpr Int keyDelete = 261;
		constexpr Int keyRight = 262;
		constexpr Int keyLeft = 263;
		constexpr Int keyDown = 264;
		constexpr Int keyUp = 265;
		constexpr Int keyHome = 268;
		constexpr Int keyEnd = 269;
		constexpr Int modifierShift = 0x0001;
		constexpr Int modifierControl = 0x0002;
		const Bool shift = (event.modifiers & modifierShift) != 0;
		const Bool control = (event.modifiers & modifierControl) != 0;
		Bool handled = false;

		if (event.type == UiRoutedEventType::TextInput)
			handled = model.execute(UiTextEditCommand::InsertText, event.text, clipboard);
		else if (event.type == UiRoutedEventType::CompositionStart)
		{
			model.compositionStart();
			handled = true;
		}
		else if (event.type == UiRoutedEventType::CompositionUpdate)
			handled = model.compositionUpdate(event.text);
		else if (event.type == UiRoutedEventType::CompositionCommit)
			handled = model.compositionCommit(event.text);
		else if (event.type == UiRoutedEventType::CompositionCancel)
		{
			model.compositionCancel();
			handled = true;
		}
		else if (event.type == UiRoutedEventType::KeyDown)
		{
			UiTextEditCommand command;
			if (control && event.key == 'A') command = UiTextEditCommand::SelectAll;
			else if (control && event.key == 'C') command = UiTextEditCommand::Copy;
			else if (control && event.key == 'X') command = UiTextEditCommand::Cut;
			else if (control && event.key == 'V') command = UiTextEditCommand::Paste;
			else if (control && event.key == 'Z') command = shift ? UiTextEditCommand::Redo : UiTextEditCommand::Undo;
			else if (control && event.key == 'Y') command = UiTextEditCommand::Redo;
			else if (event.key == keyBackspace) command = UiTextEditCommand::DeleteBackward;
			else if (event.key == keyDelete) command = UiTextEditCommand::DeleteForward;
			else if (event.key == keyEnter && model.metadata().multiline)
			{
				handled = model.execute(UiTextEditCommand::InsertText, "\n", clipboard);
				if (handled)
				{
					event.handled = true;
					event.preventDefault();
				}
				return handled;
			}
			else if (event.key == keyLeft) command = control ?
				UiTextEditCommand::MoveWordBackward : UiTextEditCommand::MoveBackward;
			else if (event.key == keyRight) command = control ?
				UiTextEditCommand::MoveWordForward : UiTextEditCommand::MoveForward;
			else if (event.key == keyUp) command = UiTextEditCommand::MoveLineUp;
			else if (event.key == keyDown) command = UiTextEditCommand::MoveLineDown;
			else if (event.key == keyHome) command = UiTextEditCommand::MoveLineStart;
			else if (event.key == keyEnd) command = UiTextEditCommand::MoveLineEnd;
			else return false;
			handled = model.execute(command, {}, clipboard, shift);
		}

		if (handled)
		{
			event.handled = true;
			event.preventDefault();
		}
		return handled;
	}
}
