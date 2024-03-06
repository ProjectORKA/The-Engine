#pragma once

#include "UserInterface.hpp"
#include "Game.hpp"
#include "Player.hpp"

//prototypes
#include "AxisNeighborPrototype.hpp"
#include "AxisNeighborPrototype2.hpp"
#include "SquareNeighborPrototype.hpp"
#include "KDTreePrototype.hpp"
#include "WeirdSphereAlgorithm.hpp"
#include "KDTreeImplementation.hpp"
#include "ParallelizedConvexHullPrototype.hpp"

struct PlaneIntersectionPrototype : Prototype
{
	void create() override;
	void action() override;
	void render(Renderer& r, const Player& player) override;
};

struct PersistentInfoPrototype : Prototype
{
	struct Point
	{
		Vec2          position     = randomVec2Fast(-10, 10);
		Point*        closestPoint = nullptr;
		Vector<Index> neighbours;

		void render(Renderer& r, const Index id) const
		{
			//r.useShader("color");
			r.circle(position, 0.05f);
			//r.textRenderSystem.render(r,toString(id));
			//r.useShader("color");
		}
	};

	struct Connection
	{
		Index a = -1;
		Index b = -1;

		void render(Renderer& r, const Vector<Point>& points) const
		{
			r.line(points[a].position, points[b].position);
		}

		Connection(const Index a, const Index b)
		{
			this->a = a;
			this->b = b;
		}
	};

	Int                step        = 0;
	Int                currentStep = 0;
	Vector<Point>      points;
	Vector<Connection> connections;

	void action() override
	{
		beep();
		step++;
	}

	void actuallyAddConnection(Index a, Index b)
	{
		logDebug("Added (" + toString(a) + "|" + toString(b) + ")");
		connections.emplace_back(a, b);
		points[a].neighbours.push_back(b);
		points[b].neighbours.push_back(a);
	}

	void actuallyRemoveConnection(const Index c)
	{
		logDebug("Removed (" + toString(connections[c].a) + "|" + toString(connections[c].b) + ")");
		auto& aNeighbours = points[connections[c].a].neighbours;
		std::erase(aNeighbours, connections[c].b);
		auto& bNeighbours = points[connections[c].b].neighbours;
		std::erase(bNeighbours, connections[c].a);
		connections.erase(connections.begin() + c);
	}

	Bool deleteOverlappingConnection()
	{
		for(Int ca = connections.size() - 1; ca >= 0; ca--)
		{
			for(Int cb = 0; cb < ca; cb++)
			{
				const Vec2 a = points[connections[ca].a].position;
				const Vec2 b = points[connections[ca].b].position;
				const Vec2 c = points[connections[cb].a].position;
				const Vec2 d = points[connections[cb].b].position;

				if(doLinesIntersect(a, b, c, d))
				{
					actuallyRemoveConnection(ca);
					return true;
				}
			}
		}
		return false;
	}

	Bool cleanupConnections()
	{
		//for every connection
		for(Int i = connections.size() - 1; i >= 0; i--)
		{
			//get both points
			const Index a = connections[i].a;
			const Index b = connections[i].b;

			//get all neighbours for each point
			Vector<Index> an = points[a].neighbours;
			Vector<Index> bn = points[b].neighbours;

			//get the two points that are connected to both a and b
			Index c = -1;
			Index d = -1;

			for(const int ani : an) for(const int bni : bn) if(ani == bni) c = ani;
			for(const int ani : an) for(const int bni : bn) if(ani == bni && ani != c) d = ani;

			//check if the points exist
			if(c != -1 && d != -1 && c != d)
			{
				//check if their connection would be shorter
				const Vec2 ap = points[a].position;
				const Vec2 bp = points[b].position;
				const Vec2 cp = points[c].position;
				const Vec2 dp = points[d].position;

				const Float ab = distance(ap, bp);
				const Float cd = distance(cp, dp);

				if(cd < ab && doLinesIntersect(ap,bp,cp,dp))
				{
					actuallyRemoveConnection(i);
					if(!tryAddConnection(c,d)){
						actuallyAddConnection(a,b);
						return false;
					}
					return true;
				}
			}
		}
		return false;
	}

	Bool tryAddConnection(const Index a, const Index b)
	{
		//make sure no bullshit is happening
		if(a == b) return false;
		if(points[a].position == points[b].position) return false;

		//get all the other connections it would intersect with
		Vector<Int> connectionIds;

		for(Int c = 0; c < connections.size(); c++)
		{
			const Vec2 ap = points[a].position;
			const Vec2 bp = points[b].position;
			const Vec2 cp = points[connections[c].a].position;
			const Vec2 dp = points[connections[c].b].position;

			if(doLinesIntersect(ap,bp,cp,dp)) connectionIds.push_back(c);
		}

		//check if any of their distances is smaller than it
		const Float dist1 = distance(points[a].position, points[b].position);

		for(const Int connectionID : connectionIds)
		{
			const Float dist2 = distance(points[connections[connectionID].a].position, points[connections[connectionID].b].position);
			if(dist2 < dist1) return false;
		}

		std::ranges::sort(connectionIds, std::greater<UInt>());

		// if not then add the connection and remove all intersecting
		for(const Int connectionID : connectionIds) actuallyRemoveConnection(connectionID);
		actuallyAddConnection(a, b);
		return true;
	}

	Index closestPoint(const Vec2 pos, const Vector<Index>& samplePoints)
	{
		Float dist = distance(pos, points[samplePoints[0]].position);
		Int nearID = samplePoints[0];
		for(Int i = 1; i < samplePoints.size(); i++)
		{
			Float dist2 = distance(pos, points[samplePoints[0]].position);
			if(dist2 < dist)
			{
				dist = dist2;
				nearID = samplePoints[i];
			}
		}
	}

	void addPoint()
	{
		//create a new point at a random location
		Vec2 newPos = randomVec2Fast(-10,10);

		//get samples for the existing data
		const Int sampleCount = ceil(log(points.size()));
		Vector<Index> samplePoints;
		for(Int i = 0; i < sampleCount; i++) samplePoints.push_back(randomIntFast(points.size()));



		//get closest sample
		Index closestID = closestPoint(newPos,samplePoints);

		//...

		for(Int i = 0; i < points.size() - 1; i++) tryAddConnection(points.size() - 1, i);
	}

	void create() override
	{
		for(int i = 0; i < 23; i++) action();
	}

	void render(Renderer& r, const Player& player) override
	{
		if(currentStep < step)
		{
			addPoint();
			currentStep++;
		}

		r.useShader("color");
		r.fill(255, 255, 255);
		//for(Int i = 0; i < points.size(); i++) points[i].render(r,i);

		Vector<Vec2> lines;
		for(const Connection& connection : connections)
		{
			lines.push_back(points[connection.a].position);
			lines.push_back(points[connection.b].position);
		}
		r.lines(lines);
	}
};

struct PrototypingRenderer final : GameRenderer
{
	Mutex       mutex;
	DebugPlayer player;
	Framebuffer framebuffer;
	Float       mouseSensitivity = 0.0015f;
	InputId     repeatAction     = InputId(InputType::KeyBoard, SPACE);
	InputEvent  enter            = InputEvent(InputType::Mouse, LMB, true);
	InputEvent  exit             = InputEvent(InputType::Mouse, RMB, false);
	InputEvent  wireframeToggle  = InputEvent(InputType::KeyBoard, F, true);
	InputEvent  action           = InputEvent(InputType::KeyBoard, G, true);

	// prototypes
	//PlaneIntersectionPrototype prototype;
	//AxisNeighborPrototype prototype;
	//AxisNeighborPrototype2 prototype;
	//SquareNeighborPrototype prototype;
	//WeirdSphereAlgorithmPrototype prototype;
	//KDTreePrototype prototype;
	//KDTreePrototype2 prototype;
	//ParallelizedConvexHullPrototype prototype;
	PersistentInfoPrototype prototype;

	void update(Window& window) override;
	void destroy(Window& window) override;
	void connect(GameSimulation& simulation) override;
	void inputEvent(Window& window, InputEvent input) override;
	void create(Window& window) override;
	void render(Window& window, TiledRectangle area) override;
	void renderInteractive(Window& window, TiledRectangle area) override;
};

struct PrototypingSandbox
{
	UserInterface       ui;
	Window              window;
	PrototypingRenderer renderer;

	void run()
	{
		ui.create();
		ui.window("Prototyping Sandbox", Area(settings.defaultWindowWidth, settings.defaultWindowHeight), true, true, WindowState::Windowed, renderer);
		ui.run();
	}
};
