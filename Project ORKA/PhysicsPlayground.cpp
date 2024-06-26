#include "PhysicsPlayground.hpp"
#include "PhysicsSystem.hpp"

void PhysicsPlaygroundSimulation::destroy() {}

void PhysicsPlayGroundRenderer::update(Window& window) {}

void PhysicsPlayGroundRenderer::destroy(Window& window) {}

void PhysicsPlaygroundSimulation::swapIntersecting()
{
	if(nodes.size() >= 4)
	{
		for(Index a = 0; a < nodes.size() - 3; a++)
		{
			for(Index b = a + 2; b < nodes.size() - 1; b++)
			{
				if(a != b && doIntersect(a, a + 1, b, b + 1))
				{
					const Int start = min(a, b) + 1;
					const Int end   = max(a, b);

					Vector<PhysicsParticle> reordered(nodes.size());

					for(Int i = 0; i < static_cast<Int>(nodes.size()); i++)
					{
						if(i < start || i > end) reordered[i] = nodes[i];
						else reordered[i]                     = nodes[end - (i - start)];
					}

					nodes = reordered;
					// return;
				}
			}
		}
	}
}

void PhysicsPlaygroundSimulation::connectByDistance()
{
	connections.clear();

	for(Int a = 0; a < static_cast<Int>(nodes.size()); a++) for(Int b = a; b < static_cast<Int>(nodes.size()); b++) if(a != b) if(distance(nodes[a].position, nodes[b].position) < connectionDistance) connections.emplace_back(a, b);
}

void PhysicsPlaygroundSimulation::removeIntersecting()
{
	for(Int a = 0; a < connections.size(); a++)
	{
		for(Int b = a; b < connections.size(); b++)
		{
			const Vec2 p1 = nodes[connections[a].a].position;
			const Vec2 p2 = nodes[connections[a].b].position;
			const Vec2 p3 = nodes[connections[b].a].position;
			const Vec2 p4 = nodes[connections[b].b].position;

			if(linesIntersecting(p1, p2, p3, p4))
			{
				const Float d1 = distance(p1, p2);
				const Float d2 = distance(p3, p4);

				if(d1 < d2) connections.erase(connections.begin() + b);
				else connections.erase(connections.begin() + a);
			}
		}
	}
}

void PhysicsPlaygroundSimulation::swapNodesToShortest()
{
	for(Int a = 0; a < nodes.size(); a++)
	{
		for(Int b = 0; b < nodes.size(); b++)
		{
			const Int a1 = min(a + 1, nodes.size() - 1);
			const Int a2 = max(a - 1, 0);
			const Int b1 = min(b + 1, nodes.size() - 1);
			const Int b2 = max(b - 1, 0);

			Float currentLength = 0.0f;
			currentLength += distanceBetweenTwoNodes(a, a1);
			currentLength += distanceBetweenTwoNodes(a, a2);
			currentLength += distanceBetweenTwoNodes(b, b1);
			currentLength += distanceBetweenTwoNodes(b, b2);

			Float swappedLength = 0.0f;
			swappedLength += distanceBetweenTwoNodes(b, a1);
			swappedLength += distanceBetweenTwoNodes(b, a2);
			swappedLength += distanceBetweenTwoNodes(a, b1);
			swappedLength += distanceBetweenTwoNodes(a, b2);

			// const Float currentLength = lengthOfEntireRun();

			// const Vec2 tmp1    = nodes[a].position;
			// nodes[a].position = nodes[b].position;
			// nodes[b].position = tmp1;

			// const Float swappedLength = lengthOfEntireRun();

			// logDebug(swappedLength);

			if(currentLength > swappedLength)
			{
				const Vec2 tmp2   = nodes[a].position;
				nodes[a].position = nodes[b].position;
				nodes[b].position = tmp2;
			}
		}
	}
}

void PhysicsPlaygroundSimulation::shorterFullConnection()
{
	for(const auto& connection : connections)
	{
		const Index b = connection.a;
		const Index c = connection.b;
		Index       a = -1;
		Index       d = -1;

		for(const Index n : nodes[b].connected) if(n != c) a = n;
		for(const Index n : nodes[c].connected) if(n != b) d = n;
		if(a == -1 || d == -1) continue;

		if(a == b || a == c || a == d || b == c || b == d || c == d) logError("failure");

		const Vec2 ap = nodes[a].position;
		const Vec2 bp = nodes[b].position;
		const Vec2 cp = nodes[c].position;
		const Vec2 dp = nodes[d].position;

		const Float d1 = distance(ap, bp) + distance(bp, cp) + distance(cp, dp);
		const Float d2 = distance(ap, cp) + distance(cp, bp) + distance(bp, dp);

		if(d2 < d1)
		{
			removeConnection(a, b);
			removeConnection(b, c);
			removeConnection(c, d);
			makeConnection(a, c);
			makeConnection(c, b);
			makeConnection(b, d);
		}
	}

	// i++;
	//	if (i == nodeCount - 3) i = 0;
}

void PhysicsPlaygroundSimulation::connectAllNodesInALine()
{
	for(Int i = 0; i < nodeCount - 1; i++) makeConnection(i, i + 1);
}

void PhysicsPlaygroundSimulation::update(const Float delta)
{
	constexpr Vec2 acceleration = gravity;
	for(PhysicsParticle& p : nodes)
	{
		verletPhysics(p.position, p.velocity, acceleration, delta);

		if(p.position.x < -1)
		{
			p.position.x = -1;
			p.velocity.x *= -1;
		}
		if(p.position.x > +1)
		{
			p.position.x = +1;
			p.velocity.x *= -1;
		}
		if(p.position.y < -1)
		{
			p.position.y = -1;
			p.velocity.y *= -1;
		}
		if(p.position.y > +1)
		{
			p.position.y = +1;
			p.velocity.y *= -1;
		}
		// loopWithinCentered(p.position, 1);
	}

	if(!paused)
	{
		// connectByDistance();
		// intersectionBasedConnections();
		// neighborBasedConnections();
		// connectNeighborOfNeighbortoSelf();
		// removeIntersecting();
		// shorterFullConnection();

		// for (int i = 0; i < 1000; i++)
		// for (int i = 0; i < 1000; i++)
		swapNodesToShortest();
		swapIntersecting();
	}
}

void PhysicsPlaygroundSimulation::neighborBasedConnections()
{
	static Bool once             = true;
	static Int  currentNodeIndex = 1;

	if(once)
	{
		for(Int i = 0; i < nodes.size(); i++)
		{
			makeConnection(i, 0);
			makeConnection(i, 1);
		}
		once = false;

		// connectNeighbors(0);

		// testAllConnectionsAndRemoveIntersecting();
	}
}

Float PhysicsPlaygroundSimulation::lengthOfEntireRun() const
{
	Float length = 0;
	for(Index i = 0; i < nodes.size() - 1; i++) length += distanceBetweenTwoNodes(i, i + 1);
	return length;
}

void PhysicsPlaygroundSimulation::intersectionBasedConnections()
{
	static Index a = 0;
	static Index b = a + 1;

	Int failsafe = 0;

	Bool intersect = false;

	for(Index c = 0; c < connections.size(); c++)
	{
		const Vec2 aPos  = nodes[a].position;
		const Vec2 bPos  = nodes[b].position;
		const Vec2 cAPos = nodes[connections[c].a].position;
		const Vec2 cBPos = nodes[connections[c].b].position;

		if(linesIntersecting(aPos, bPos, cAPos, cBPos))
		{
			const Float d1 = distance(aPos, bPos);
			const Float d2 = distance(cAPos, cBPos);

			if(d1 < d2)
			{
				logDebug("A: " + toString(d1));
				logDebug("B: " + toString(d2));

				connections.erase(connections.begin() + c);
				return;
			}
			intersect = true;
		}
	}

	if(!intersect) makeConnection(a, b);

	b++;
	if(b == nodeCount)
	{
		a++;
		b = a + 1;
	}
	if(a == nodeCount - 1)
	{
		a = 0;
		b = a + 1;
	}

	sleep(1000);
}

void PhysicsPlaygroundSimulation::makeConnection(Index a, Index b)
{
	addOneWayConnection(a, b);
	addOneWayConnection(b, a);

	for(const auto c : connections) if((c.a == a && c.b == b) || (c.a == b && c.b == a)) return;
	connections.emplace_back(a, b);
}

void PhysicsPlayGroundRenderer::connect(GameSimulation& simulation)
{
	this->sim = static_cast<PhysicsPlaygroundSimulation*>(&simulation);
}

void PhysicsPlaygroundSimulation::connectNeighborOfNeighborToSelf()
{
	Vector<PhysicsPlaygroundConnection> cons;

	// for all nodes
	for(Int i = 0; i < nodeCount; i++) for(Int n = 0; n < nodes[i].connected.size(); n++) for(int c = 0; c < nodes[nodes[i].connected[n]].connected.size(); c++) cons.emplace_back(i, nodes[nodes[i].connected[n]].connected[c]);

	for(const auto c : cons) makeConnection(c.a, c.b);
}

void PhysicsPlaygroundSimulation::connectNeighbors(const Index node)
{
	for(Int a = 0; a < nodes[node].connected.size(); a++) for(int b = a; b < nodes[node].connected.size(); b++) makeConnection(a, b);
}

void PhysicsPlaygroundSimulation::create()
{
	while(nodes.size() < nodeCount) nodes.emplace_back();
	connectAllNodesInALine();
}

Vec2 PhysicsPlaygroundSimulation::getPointOnSmoothCurve(const Float value) const
{
	Vector<Vec2> curve;

	curve.resize(nodes.size());

	for(Index i = 0; i < nodes.size(); i++) curve[i] = nodes[i].position;

	while(curve.size() > 1)
	{
		for(Index i = 0; i < curve.size() - 1; i++) curve[i] = lerp(curve[i], curve[i + 1], value);
		curve.pop_back();
	}

	return curve.front();
}

void PhysicsPlaygroundSimulation::removeConnection(const Index a, const Index b)
{
	for(Int i = 0; i < nodes[a].connected.size(); i++)
	{
		if(nodes[a].connected[i] == b)
		{
			nodes[a].connected.erase(nodes[a].connected.begin() + i);
			break;
		}
	}

	for(Int i = 0; i < nodes[b].connected.size(); i++)
	{
		if(nodes[b].connected[i] == a)
		{
			nodes[b].connected.erase(nodes[b].connected.begin() + i);
			break;
		}
	}

	for(Int i = 0; i < connections.size(); i++)
	{
		const PhysicsPlaygroundConnection& c = connections[i];
		if((c.a == a && c.b == b) || (c.b == a && c.a == b))
		{
			connections.erase(connections.begin() + i);
			i--;
		}
	}
}

void PhysicsPlayGroundRenderer::create(Window& window) {}

void PhysicsPlayGroundRenderer::inputEvent(Window& window, const InputEvent input)
{
	if(input == pause) sim->paused = !sim->paused;
}

PhysicsPlaygroundConnection::PhysicsPlaygroundConnection(const Index a, const Index b)
{
	this->a = a;
	this->b = b;
}

void PhysicsPlaygroundSimulation::addOneWayConnection(const Index aid, const Index bid)
{
	// make sure connection is not to self
	if(aid == bid) return;
	// make sure connection is only once
	PhysicsParticle& a = nodes[aid];
	for(const unsigned int i : a.connected) if(i == bid) return;
	// add connection if everything is correct
	a.connected.push_back(bid);
}

Float PhysicsPlaygroundSimulation::distanceBetweenTwoNodes(const Index a, const Index b) const
{
	return distance(nodes[a].position, nodes[b].position);
}

void PhysicsPlayGroundRenderer::render(Window& window, TiledRectangle area)
{
	Renderer& renderer = window.renderer;

	renderer.clearBackground(Color(0, 0, 0, 1));

	renderer.aspectCorrectNormalizedSpace();

	renderer.fill(Color(1));
	renderer.useShader("color");

	for(PhysicsParticle& p : sim->nodes) p.render(window, sim->nodes);

	Vector<Vec2> lines;
	if(smoothCurve)
	{
		Vector<Vec2> smoothCurve;

		for(Index i = 0; i < 1000; i++) smoothCurve.push_back(sim->getPointOnSmoothCurve(static_cast<Float>(i) / 1000.0f));

		for(Index i = 0; i < smoothCurve.size() - 1; i++){
			lines.emplace_back(smoothCurve[i]);
			lines.emplace_back(smoothCurve[i+1]);
		};
	}
	else
	{
		for(const PhysicsPlaygroundConnection& c : sim->connections){
			lines.emplace_back(sim->nodes[c.a].position);
			lines.emplace_back(sim->nodes[c.b].position);
		}
	}
	renderer.fill(Color(1));
	renderer.useShader("color");
	renderer.lines(lines);
}

Bool PhysicsPlaygroundSimulation::doIntersect(const Index a, const Index b, const Index c, const Index d) const
{
	return linesIntersecting(nodes[a].position, nodes[b].position, nodes[c].position, nodes[d].position);
}

void PhysicsPlayGroundRenderer::renderInteractive(Window& window, TiledRectangle area) {}

void PhysicsParticle::render(Window& window, Vector<PhysicsParticle>& particles) const
{
	Renderer& renderer = window.renderer;
	renderer.uniforms().setMMatrix(matrixFromPositionAndSize(position, Vec2(particleSize)));
	renderer.plane();
}
