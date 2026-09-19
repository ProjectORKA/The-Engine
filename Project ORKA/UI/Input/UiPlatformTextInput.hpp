#pragma once

#include "UiEventRouter.hpp"
#include "UiTextEditModel.hpp"

namespace External { struct GLFWwindow; }

namespace Ui
{
	class UiGlfwClipboard final : public UiClipboard
	{
	public:
		explicit UiGlfwClipboard(External::GLFWwindow* window = nullptr) : apiWindow(window) {}
		void setWindow(External::GLFWwindow* window) { apiWindow = window; }
		[[nodiscard]] String readText() const override;
		void writeText(const String& text) override;

	private:
		External::GLFWwindow* apiWindow = nullptr;
	};

	// Platform adapters feed this bridge. It owns no widget or native-window state.
	class UiPlatformTextInputBridge
	{
	public:
		using EventSink = Function<Bool(UiEvent)>;

		void setEventSink(EventSink sink);
		void clearEventSink();
		[[nodiscard]] Bool unicodeCodepoint(U32 codepoint) const;
		[[nodiscard]] Bool windowFocusChanged(Bool focused);
		[[nodiscard]] Bool compositionStart();
		[[nodiscard]] Bool compositionUpdate(String utf8, SizeT selectionStart, SizeT selectionEnd);
		[[nodiscard]] Bool compositionCommit(String utf8);
		[[nodiscard]] Bool compositionCancel();

	private:
		EventSink eventSink;
		Bool compositionActive = false;

		[[nodiscard]] Bool send(UiEvent event) const;
	};

	// Default command policy for a focused editable node. Applications may replace
	// this mapping while continuing to use UiTextEditModel::execute directly.
	class UiTextEditCommandRouter
	{
	public:
		[[nodiscard]] Bool route(UiRoutedEvent& event, UiTextEditModel& model,
			UiClipboard* clipboard = nullptr) const;
	};
}
