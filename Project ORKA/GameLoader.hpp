#pragma once

#include "Basics.hpp"
#include "UIButton.hpp"

struct Window;

struct GameEntry
{
	String           displayName;
	String           description;
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
	void destroy(Window& window) override;

	[[nodiscard]] Function<void()> consumeSelectedGame();

private:
	struct LaunchButton final : UIButton
	{
		GameLoader* owner      = nullptr;
		Window*     owningWindow = nullptr;
		Int         entryIndex = -1;

		LaunchButton(GameLoader& loaderRef, Int index);
		void doThis() override;
	};

	void buildUi();
	void onGameSelected(Window& window, Int index);
	static Vector<GameEntry> buildDefaultEntries();

	Vector<GameEntry>     entries;
	Vector<LaunchButton>  buttons;
	Vector<UIElement>     entryContents;
	Vector<UIElement>     nameLabels;
	Vector<UIElement>     descriptionLabels;
	UIElement             titleLabel;

	Atomic<Int>      selectionIndex = -1;
	Function<void()> selectedGame;
	Bool             built = false;
};

