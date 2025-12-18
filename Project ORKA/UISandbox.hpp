#pragma once

#include "Game.hpp"
#include "Basics.hpp"
#include "Pong.hpp"
#include "UIButton.hpp"
#include "Sandbox.hpp"

struct Renderer;
struct Window;

struct Toggle final : UIButton
{
	Bool toggle = false;

	void doThis() override;
	void update(Window& window) override;
	void destroy(Window& window) override;
	void create(Window& window) override;
};

struct UISandbox
{
	void run() const
	{
		UIElement a;
		a.padding(20);
		a.name("GlobalView");
		a.background(Color(1, 1, 1, 0.1));
		a.row();
		a.growX(true);
		a.growY(true);
		a.color(Color(1, 1, 1, 1));
		a.setText("A");
		a.gap(20);

		UIElement b;
		b.padding(0);
		b.name("GlobalView");
		b.background(Color(1, 1, 1, 0.1));
		b.row();
		b.growX(true);
		b.growY(true);
		b.color(Color(1, 1, 1, 1));
		b.setText("B");

		UIElement c;
		c.padding(0);
		c.name("GlobalView");
		c.background(Color(1, 1, 1, 0.1));
		c.row();
		c.growX(true);
		c.growY(true);
		c.color(Color(1, 1, 1, 1));
		c.setText("C");

		// Embedded sandbox (3D scene) rendered as a UI element
		SandboxRenderer sandboxRenderer;
		sandboxRenderer.padding(20);
		sandboxRenderer.name("Sandbox");
		sandboxRenderer.growX(true);
		sandboxRenderer.growY(true);

		//UIElement d;
		//d.padding(20);
		//d.name("GlobalView");
		//d.background(Color(1, 1, 1, 0.1));
		//d.row();
		//d.growX(true);
		//d.growY(true);
		//d.setWidth(200);
		//d.setHeight(100);
		//d.setPositionY(-200);
		//d.setPositionX(300);
		//d.color(Color(1, 1, 1, 1));
		//d.setText("D");

		//UIElement e;
		//e.padding(20);
		//e.name("GlobalView");
		//e.background(Color(1, 1, 1, 0.1));
		//e.row();
		//e.growX(true);
		//e.growY(true);
		//e.setWidth(200);
		//e.setHeight(100);
		//e.setPositionY(-200);
		//e.setPositionX(300);
		//e.color(Color(1, 1, 1, 1));
		//e.setText("E");

		//SandboxRenderer f;
		//f.padding(20);
		//f.name("GlobalView");
		//f.background(Color(1, 1, 1, 0.1));
		//f.row();
		//f.growX(true);
		//f.growY(true);
		//f.setWidth(200);
		//f.setHeight(200);
		//f.setPositionY(-200);
		//f.setPositionX(300);
		//f.color(Color(1, 1, 1, 1));
		//f.setText("F");

		//PongRenderer g;
		//g.padding(20);
		//g.name("GlobalView");
		//g.background(Color(1, 1, 1, 0.1));
		//g.row();
		//g.growX(true);
		//g.growY(true);
		//g.setWidth(200);
		//g.setHeight(200);
		//g.setPositionY(-200);
		//g.setPositionX(300);
		//g.color(Color(1, 1, 1, 1));
		//g.setText("G");

		//UIElement h;
		//h.padding(20);
		//h.name("GlobalView");
		//h.background(Color(1, 1, 1, 0.1));
		//h.row();
		//h.growX(true);
		//h.growY(true);
		//h.setWidth(200);
		//h.setHeight(200);
		//h.setPositionY(-200);
		//h.setPositionX(300);
		//h.color(Color(1, 1, 1, 1));
		//h.setText("H");

		Window& window = ui.window("ORKA UI Sandbox", settings.defaultWindowWidth, settings.defaultWindowHeight, true, true, WindowState::Windowed);
		window.name("Window");
		window.background(Color(0.05, 0.05, 0.05, 1));
		window.addElement(a);
		window.addElement(b);
		//window.addElement(c);
		//window.addElement(sandboxRenderer);
		window.padding(100);
		window.gap(0);
		window.column();
		ui.run();
	}
};
