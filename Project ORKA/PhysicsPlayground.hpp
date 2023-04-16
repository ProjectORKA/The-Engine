#pragma once

#include "Game.hpp"
#include "Random.hpp"
#include "Physics.hpp"
#include "Collision.hpp"

const UInt nodeCount = 500;
const Float particleSize = 0.00;
const Float lineSize = 0.002;
const Float startingVelocity = 0.0;
const Vec2 gravity = Vec2(0, 0);
const Float connectionDistance = 0.1;

struct PhysicsParticle {
	Vec2 position = randomVec2(-1, 1);
	Vec2 velocity = randomVec2(-startingVelocity, startingVelocity);
	Bool isColliding = false;
	Vector<Index> connected = {};

	void render(ResourceManager& resourceManager, Window& window, std::vector<PhysicsParticle>& particles) {
		Renderer& renderer = window.renderer;
		renderer.rectangle(resourceManager, position, Vec2(particleSize));
	}
};

struct Connection {
	Index a = -1;
	Index b = -1;

	Connection(Index a, Index b) {
		this->a = a;
		this->b = b;
	};
};

struct PhysicsPlaygroundSimulation : public GameSimulation {
	Bool paused = true;
	std::vector<PhysicsParticle> nodes;
	std::vector<Connection> connections;

	Vec2 getPointOnSmoothCurve(Float value) {
		Vector<Vec2> curve;

		curve.resize(nodes.size());

		for (Index i = 0; i < nodes.size(); i++) {
			curve[i] = nodes[i].position;
		}

		while (curve.size() > 1) {
			for (Index i = 0; i < curve.size() - 1; i++) {
				curve[i] = lerp(curve[i], curve[i + 1], value);
			}
			curve.popBack();
		}

		return curve.first();
	}

	PhysicsPlaygroundSimulation() {
		while (nodes.size() < nodeCount) {
			nodes.emplace_back();
		}
	}

	void connectByDistance() {
		connections.clear();

		for (Index a = 0; a < nodes.size(); a++) {
			for (int b = a; b < nodes.size(); b++) {
				if (a != b) {
					if (distance(nodes[a].position, nodes[b].position) < connectionDistance) {
						connections.emplace_back(a, b);
					}
				}
			}
		}
	}
	void connectAllNodesInALine() {
		for (int i = 0; i < nodeCount - 1; i++) {
			makeConnection(i, i + 1);
		}
	}
	void neighborBasedConnections() {
		static bool once = true;
		static int currentNodeIndex = 1;
		//for (int i = 0; i < currentNodeIndex; i++) {

		//}

		if (once) {

			for (int i = 0; i < nodes.size(); i++) {
				makeConnection(i, 0);
				makeConnection(i, 1);
			}
			once = false;

			//connectNeighbors(0);

			//testAllConnectionsAndRemoveIntersecting();
		}
	}

	void removeIntersecting() {
		for (int a = 0; a < connections.size(); a++) {
			for (int b = a; b < connections.size(); b++) {
				Vec2 p1 = nodes[connections[a].a].position;
				Vec2 p2 = nodes[connections[a].b].position;
				Vec2 p3 = nodes[connections[b].a].position;
				Vec2 p4 = nodes[connections[b].b].position;

				if (doIntersect(p1, p2, p3, p4)) {
					Float d1 = distance(p1, p2);
					Float d2 = distance(p3, p4);

					if (d1 < d2) connections.erase(connections.begin() + b);
					else connections.erase(connections.begin() + a);
				}
			}
		}
	}
	void connectNeighbors(Index node) {
		for (int a = 0; a < nodes[node].connected.size(); a++) {
			for (int b = a; b < nodes[node].connected.size(); b++) {
				makeConnection(a, b);
			}
		}
	}
	void shorterFullConnection() {
		for (int i = 0; i < connections.size(); i++) {

			Index b = connections[i].a;
			Index c = connections[i].b;
			Index a = -1;
			Index d = -1;

			for (Index n : nodes[b].connected) {
				if (n != c) a = n;
			}
			for (Index n : nodes[c].connected) {
				if (n != b) d = n;
			}
			if (a == -1 || d == -1) continue;

			if (a == b || a == c || a == d || b == c || b == d || c == d) logError("failure");

			Vec2 ap = nodes[a].position;
			Vec2 bp = nodes[b].position;
			Vec2 cp = nodes[c].position;
			Vec2 dp = nodes[d].position;

			Float d1 = distance(ap, bp) + distance(bp, cp) + distance(cp, dp);
			Float d2 = distance(ap, cp) + distance(cp, bp) + distance(bp, dp);

			if (d2 < d1) {
				removeConnection(a, b);
				removeConnection(b, c);
				removeConnection(c, d);
				makeConnection(a, c);
				makeConnection(c, b);
				makeConnection(b, d);
			}
		}

		//i++;
	//	if (i == nodeCount - 3) i = 0;
	}
	void intersectionBasedConnections() {
		static Index a = 0;
		static Index b = a + 1;

		Int failsafe = 0;

		Bool intersect = false;

		for (Index c = 0; c < connections.size(); c++) {
			Vec2 aPos = nodes[a].position;
			Vec2 bPos = nodes[b].position;
			Vec2 cAPos = nodes[connections[c].a].position;
			Vec2 cBPos = nodes[connections[c].b].position;

			if (doIntersect(aPos, bPos, cAPos, cBPos)) {
				Float d1 = distance(aPos, bPos);
				Float d2 = distance(cAPos, cBPos);

				if (d1 < d2) {
					logDebug(String("A: ").append(toString(d1)));
					logDebug(String("B: ").append(toString(d2)));

					connections.erase(connections.begin() + c);
					return;
				}
				intersect = true;
			}
		}

		if (!intersect) makeConnection(a, b);

		b++;
		if (b == nodeCount) {
			a++;
			b = a + 1;
		}
		if (a == nodeCount - 1) {
			a = 0;
			b = a + 1;
		}

		sleep(1000);

	}
	void makeConnection(Index a, Index b) {
		addOneWayConnection(a, b);
		addOneWayConnection(b, a);

		for (auto c : connections) {
			if ((c.a == a && c.b == b) || (c.a == b && c.b == a)) return;
		}
		connections.emplace_back(a, b);
	}
	void connectNeighborOfNeighbortoSelf() {

		Vector<Connection> cons;

		//for all nodes
		for (int i = 0; i < nodeCount; i++) {
			for (int n = 0; n < nodes[i].connected.size(); n++) {
				for (int c = 0; c < nodes[nodes[i].connected[n]].connected.size(); c++) {
					cons.emplaceBack(i, nodes[nodes[i].connected[n]].connected[c]);
				}
			}
		}

		for (auto c : cons) makeConnection(c.a, c.b);
	}
	void removeConnection(Index a, Index b) {
		for (int i = 0; i < nodes[a].connected.size(); i++) {
			if (nodes[a].connected[i] == b) {
				nodes[a].connected.erase(i);
				break;
			}
		}

		for (int i = 0; i < nodes[b].connected.size(); i++) {
			if (nodes[b].connected[i] == a) {
				nodes[b].connected.erase(i);
				break;
			}
		}

		for (int i = 0; i < connections.size(); i++) {
			Connection& c = connections[i];
			if ((c.a == a && c.b == b) || (c.b == a && c.a == b)) {
				connections.erase(connections.begin() + i);
				i--;
			}
		}
	}
	void addOneWayConnection(Index aid, Index bid) {
		//make sure connection is not to self
		if (aid == bid) return;
		//make sure connection is only once
		PhysicsParticle& a = nodes[aid];
		for (int i = 0; i < a.connected.size(); i++) {
			if (a.connected[i] == bid) return;
		}
		//add connection if everything is correct
		a.connected.pushBack(bid);
	}
	Float distanceBetweenTwoNodes(Index a, Index b) {
		return distance(nodes[a].position, nodes[b].position);
	}
	Bool doIntersect(Vec2 a, Vec2 b, Vec2 c, Vec2 d) {

		if (a == b || a == c || a == d || b == c || b == d || c == d) return false;

		Double px1 = a.x;
		Double px2 = b.x;
		Double px3 = c.x;
		Double px4 = d.x;

		Double py1 = a.y;
		Double py2 = b.y;
		Double py3 = c.y;
		Double py4 = d.y;

		Double ua = 0.0;
		Double ub = 0.0;
		Double ud = (py4 - py3) * (px2 - px1) - (px4 - px3) * (py2 - py1);


		if (ud != 0) {
			ua = ((px4 - px3) * (py1 - py3) - (py4 - py3) * (px1 - px3)) / ud;
			ub = ((px2 - px1) * (py1 - py3) - (py2 - py1) * (px1 - px3)) / ud;
			if (ua < 0.0 || ua > 1.0 || ub < 0.0 || ub > 1.0) ua = 0.0;
		}

		return ua;
	}
	Bool doIntersect(Index a, Index b, Index c, Index d) {
		return doIntersect(nodes[a].position, nodes[b].position, nodes[c].position, nodes[d].position);
	}

	///////////////////////////////////////////////////////////////

	Float lengthOfEntireRun() {
		Float length = 0;
		for (Index i = 0; i < nodeCount - 1; i++) {
			length += distanceBetweenTwoNodes(i, i + 1);
		}
		return length;
	}

	void swapNodesToShortest() {
		for (Index a = 0; a < nodeCount; a++) {
			for (Index b = a + 1; b < nodeCount; b++) {
				Float length = lengthOfEntireRun();

				PhysicsParticle tmp = nodes[a];
				nodes[a] = nodes[b];
				nodes[b] = tmp;

				Float currentLength = lengthOfEntireRun();

				if (currentLength > length) {
					PhysicsParticle tmp = nodes[a];
					nodes[a] = nodes[b];
					nodes[b] = tmp;
				}
				//else return;
			}
		}
	}

	void swapIntersecting() {
		for (Index a = 0; a < nodeCount; a++) {
			for (Index b = a + 2; b < nodeCount; b++) {
				if (a != b && doIntersect(a, a + 1, b, b + 1)) {
					Index start = min(a, b) + 1;
					Index end = max(a, b);

					std::vector<PhysicsParticle> reordered(nodeCount);

					for (int i = 0; i < nodeCount; i++) {
						if (i < start || i > end) {
							reordered[i] = nodes[i];
						}
						else {
							reordered[i] = nodes[end - (i - start)];
						}
					}

					nodes = reordered;
					//return;
				}
			}
		}
	}

	void start(ResourceManager& resourceManager) override {
		connectAllNodesInALine();
	}
	void update(Float delta) override {
		if (!paused) {

			Vec2 acceleration = gravity;
			for (PhysicsParticle& p : nodes) {
				verletPhysics(p.position, p.velocity, acceleration, delta);

				if (p.position.x < -1) { p.position.x = -1; p.velocity.x *= -1; }
				if (p.position.x > +1) { p.position.x = +1; p.velocity.x *= -1; }
				if (p.position.y < -1) { p.position.y = -1; p.velocity.y *= -1; }
				if (p.position.y > +1) { p.position.y = +1; p.velocity.y *= -1; }

				//loopWithinCentered(p.position, 1);
			}

			//connectByDistance();
			//intersectionBasedConnections();
			//neighborBasedConnections();
			//connectNeighborOfNeighbortoSelf();
			//removeIntersecting();
			//shorterFullConnection();

			//for (int i = 0; i < 1000; i++)
			//for (int i = 0; i < 1000; i++)
			swapNodesToShortest();
			swapIntersecting();

		}
	}
};

struct PhysicsPlayGroundRenderer : public GameRenderer {
	PhysicsPlaygroundSimulation* sim = nullptr;

	InputEvent pause = InputEvent(InputType::KeyBoard, P, 0);

	void inputEvent(Window& window, InputEvent input) override {
		if (input == pause) sim->paused = !sim->paused;
	};
	void render(ResourceManager& resourceManager, Window& window, TiledRectangle area) override {
		Renderer& renderer = window.renderer;

		renderer.clearColor(Color(0));

		renderer.aspectCorrectNormalizedSpace();

		renderer.fill(Color(1));
		renderer.useShader(resourceManager, "color");

		sim->update(renderer.deltaTime());

		for (PhysicsParticle& p : sim->nodes) {
			p.render(resourceManager, window, sim->nodes);
		}

		//for (Connection& c : sim->connections) {
		//	renderer.line(sim->nodes[c.a].position, sim->nodes[c.b].position, lineSize);
		//}

		Vector<Vec2> smoothCurve;

		for (Index i = 0; i < 1000; i++) {
			smoothCurve.pushBack(sim->getPointOnSmoothCurve(Float(i) / 1000.0f));
		}

		//render smooth curve
		for (Index i = 0; i < smoothCurve.size() - 1; i++) {
			renderer.line(smoothCurve[i], smoothCurve[i + 1], lineSize);
		}
	}
	void renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle area) override {};
};

struct PhysicsPlayground {
	UserInterface ui;
	ResourceManager resourceManager;
	PhysicsPlaygroundSimulation sim;

	PhysicsPlayGroundRenderer game;

	void run() {
		sim.start(resourceManager);

		game.sim = &sim;
		ui.window("Physics playground", 1024, 1024, WindowState::windowed, game, resourceManager);
		ui.run();

		//sim.stop();
	}
};