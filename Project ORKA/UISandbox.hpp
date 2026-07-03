#pragma once

#include "Game.hpp"
#include "Basics.hpp"
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
		a.setName("A");
		a.row();
		a.growX(true);
		a.growY(true);
		a.color(Color(1, 1, 1, 1));
		a.setText("A");
		a.gap(20);

		UIButton b;
		b.padding(0);
		b.setName("B");
		b.row();
		b.growX(true);
		b.growY(true);
		b.color(Color(1, 1, 1, 1));
		b.setText("B");

		UIElement c;
		c.padding(0);
		c.setName("C");
		c.row();
		c.growX(true);
		c.growY(true);
		c.color(Color(1, 1, 1, 1));
		c.setText("C");

		// Embedded sandbox (3D scene) rendered as a UI element
		SandboxRenderer sandbox;

		UIElement d;
		d.padding(20);
		d.setName("GlobalView");
		d.row();
		d.color(Color(1, 1, 1, 1));
		d.setText("D");

		UIElement e;
		e.padding(20);
		e.setName("GlobalView");
		e.color(Color(1, 1, 1, 1));
		e.setText("E");

		UIElement f;
		f.padding(20);
		f.setName("GlobalView");
		f.color(Color(1, 1, 1, 1));
		f.setText("F");

		a.addElement(sandbox);
		a.addElement(d);
		a.addElement(e);
		a.addElement(f);

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
		window.setName("Window");
		window.addElement(a);
		window.addElement(b);
		window.addElement(c);
		window.padding(5);
		window.gap(5);
		window.column();
		ui.run();
	}
};
