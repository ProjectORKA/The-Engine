
#include "Collision.hpp"

HitResult2D sphereToInfiniteWallCollision(CircleCollider c, InfiniteWallCollider2D w) {
	HitResult2D hit;

	//

	return hit;
}

Bool aabbCollision(AABB a, AABB b) {
	if (a.e < b.w || a.w > b.e || a.s < b.n || a.n > b.s) return false;
	else return true;
}
