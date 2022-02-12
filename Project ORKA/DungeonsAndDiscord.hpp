
#pragma once

#include "Transform.hpp"
#include "Game.hpp"
#include "KeyMap.hpp"
#include "Player.hpp"
#include "ECS.hpp"

// todo list
// import objects
// move objects around
// also scale and rotate them in real time
// be able to change the sky

struct Renderer;
struct Window;


struct DNDWorld : public GameSimulation {
	//savedata for camera
	Float fieldOfView = 80.0;
	Float nearClipValue = 0.1;
	Float farClipValue = 100000;
	Vec3 location = Vec3(0);
	Float rotationX = 0;
	Float rotationZ = 0;
	//player data
	Int speedExponent = 0;

	Vector<Entity> entities;
	
	void save();
	void load();
	void create() override;
	void destroy() override;
	void addObject(Entity e);
};

struct DNDRenderer : public GameRenderer {
	//Input
	Action	forward;
	Action	backward;
	Action	left;
	Action	right;
	Action	upward;
	Action	downward;
	Action	jump;
	Action	wireframe;

	Float mouseSensitivity = 0.0015f;
	Player player;

	DNDWorld* world = nullptr;

	Index lastSelectedObject = -1;
	Vector<Index> selectedObjects;

	DNDRenderer() {};
	DNDRenderer(DNDWorld* world);
	virtual void update(Renderer& renderer)override;
	virtual void mouseIsMoving(Window& window, IVec2 position)  override;
	virtual void render(TiledRectangle area, Renderer& renderer) override;
	virtual void filesDropped(Window& window, Vector<Path> paths) override;
	virtual void renderInteractive(TiledRectangle area, Renderer& renderer) override;
	virtual void mouseIsScrolled(Window& window, Double xAxis, Double yAxis) override;
	virtual void buttonIsPressed(Window& window, Key key, Int action, Int modifiers) override;
	virtual void mouseIsPressed(Window& window, MouseButton button, Int action, Int modifiers) override;
};

struct DND {
	DNDWorld world;
	DNDRenderer renderer;
	DND();
};

Int diceRoll(Int diceCount);