#pragma once

#include "Basics.hpp"

struct Window;
struct Renderer;

// Screen-space crosshair: dark outline circle with a white fill.
void renderCrosshairDot(Window& window, Float outerSize = 3.0f, Float innerSize = 2.0f);
void renderCrosshairDot(Renderer& renderer, Vec2 center, Float outerSize = 3.0f, Float innerSize = 2.0f);
