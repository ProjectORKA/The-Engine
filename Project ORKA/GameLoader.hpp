#pragma once

#include "Basics.hpp"
#include "UI/Authoring/UiAuthoredHost.hpp"
#include "UI/Theme/UiAppTheme.hpp"
#include "UI/UiHost.hpp"

struct Window;

struct GameEntry
{
	String           displayName;
	String           description;
	String           iconName;
	Function<void()> launch;
};

// UI element shown after the intro finishes. It reuses the existing intro window
// to present the list of games and records the user's selection without
// spawning additional windows.
struct GameLoader final : UIElement
{
	Color backgroundColor = Color(0.0, 0.0, 0.0, 1.0);
	Color hoveredBackgroundColor = Color(0.0, 0.0, 0.0, 1.0);
	Color activeBackgroundColor = Color(0.0, 0.0, 0.0, 1.0);

	GameLoader();

	void create(Window& window) override;
	void update(Window& window) override;
	void destroy(Window& window) override;

	[[nodiscard]] Function<void()> consumeSelectedGame();

private:
	void applyTheme();
	void toggleTheme();
	void applyLauncherLayout();
	void wireLauncherInteractions();
	void onGameSelected(Window& window, Int index);
	static Vector<GameEntry> buildDefaultEntries();

	Vector<GameEntry> entries;
	Ui::UiAppTheme theme = Ui::UiAppTheme::light();
	Ui::UiHost host;
	UniquePointer<Ui::UiAuthoredHost> authored;
	Window* owningWindow = nullptr;

	Atomic<Int>      selectionIndex = -1;
	Function<void()> selectedGame;
	Bool             authoredReady = false;
};
