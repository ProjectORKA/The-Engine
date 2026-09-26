#pragma once

#include "SimpleRtsWorldPresets.hpp"
#include "SimpleRtsWorldSettings.hpp"
#include "UI/Authoring/UiAuthoredHost.hpp"
#include "UI/Theme/UiAppTheme.hpp"
#include "UI/UiHost.hpp"
#include "UIElement.hpp"

struct SimpleRtsRenderer;

struct SimpleRtsSettingsPanel final : UIElement
{
	explicit SimpleRtsSettingsPanel(SimpleRtsRenderer& owner);

	void bindRenderer(SimpleRtsRenderer& owner);
	void setVisiblePanel(const Bool visible);
	[[nodiscard]] Bool isVisiblePanel() const { return panelVisible; }

	void togglePanel();
	void applyWorldFromDraft();
	void syncOverlayLayout(Window& window);

	void create(Window& window) override;
	void update(Window& window) override;
	void render(Window& window) override;
	void inputEvent(Window& window, InputEvent& input) override;
	void destroy(Window& window) override;
	[[nodiscard]] Bool isPointInside(const Window& window, const DVec2& point) const override;

	SimpleRtsWorldSettings draftSettings;

private:
	SimpleRtsRenderer* renderer = nullptr;
	Ui::UiAppTheme theme = Ui::UiAppTheme::light();
	Ui::UiHost host;
	UniquePointer<Ui::UiAuthoredHost> authored;
	Bool authoredReady = false;
	Bool panelVisible = true;
	Bool worldRunning = false;
	Int nextTabIndex = 0;
	String selectedPreset;
	String presetName;
	String statusLine;

	Window* owningWindow = nullptr;

	void refreshBoundState();
	void applyTheme();
	void syncDraftFromProperties(const String& path, const Ui::UiValue& value);
	void loadSelectedPresetIntoDraft();
	void publishPresetsList();
};
