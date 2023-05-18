#include "Collision.hpp"

HitResult2D sphereToInfiniteWallCollision(CircleCollider c, InfiniteWallCollider2D w) {
	const HitResult2D hit;

	//

	return hit;
}

Bool aabbCollision(const AABB a, const AABB b) {
	if (a.e < b.w || a.w > b.e || a.s < b.n || a.n > b.s) return false;
	return true;
}
