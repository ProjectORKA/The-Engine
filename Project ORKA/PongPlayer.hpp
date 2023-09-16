
#include "Window.hpp"
#include "PongBall.hpp"

#define PONG_DIFFICULTY_MODIFIER 1.05

struct PongPlayer
{
	UInt score = 0;
	Bool shoot = false;
	Vec2 position = Vec2(0, 0);
	Vec2 direction = Vec2(1, 0);
	// ai stuff
	Float target = 0.0f;
	Float velocity = 0.0f;
	Float difficulty = 7.0f;

	void update(Window& window);
	void mouseInput(Vec3 cursorWorldPosition);
	void ballLocationInput(Vector<PongBall>& balls);
	void aiInput(Vector<PongBall>& balls, Float deltaTime);
	void keyboardInput(Window& window, Int up, Int down, Int shoot);
};

PongBall* getClosestBall(const PongPlayer& player, Vector<PongBall>& balls);