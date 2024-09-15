#include "TriangulationPrototype.hpp"
#include "DelaunayTriangulation.hpp"

TriangulationPrototype::Connection::Connection(const Index a, const Index b, const Float distance) {
	this->a        = a;
	this->b        = b;
	this->distance = distance;
}

Bool TriangulationPrototype::Connection::operator<(const Connection& other) const
{
	const Index x = min(a, b);
	const Index y = max(a, b);
	const Index z = min(other.a, other.b);
	const Index w = max(other.a, other.b);

	if (x == z && y == w) return false;
	if (x < z) return true;
	if (x == z && y < w) return true;
	return false;
}

void TriangulationPrototype::addConnection(Index a, Index b) {
	connections.emplace_back(a, b, distance(positions[a], positions[b]));
}

void TriangulationPrototype::create() {
	Vector<Double> points;

	for (int i = 0; i < pointCount; i++)
	{
		positions.push_back(randomVec2Fast(-10, 10));
		points.push_back(positions.back().x);
		points.push_back(positions.back().y);
	}

	Delaunator::Delaunator d(points);

	for (SizeT i = 0; i < d.triangles.size(); i += 3)
	{
		Index a = d.triangles[i];
		Index b = d.triangles[i + 1];
		Index c = d.triangles[i + 2];
		addConnection(a, b);
		addConnection(b, c);
		addConnection(c, a);
	}

	//make unique
	std::set uniqueConnections(connections.begin(), connections.end());
	connections.assign(uniqueConnections.begin(), uniqueConnections.end());
}

void TriangulationPrototype::render(Renderer& r, const Player& player) {
	r.uniforms().setMMatrix(Matrix(1));

	r.points(positions);

	r.uniforms().setMMatrix(Matrix(1));

	Vec2Vector lines;
	for (const Connection& c : connections)
	{
		lines.push_back(positions[c.a]);
		lines.push_back(positions[c.b]);
	}
	r.lines(lines);
}
