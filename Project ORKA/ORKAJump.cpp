#include "ORKAJump.hpp"

Bool isColliding(const AABB& box1, const AABB& box2) {
	if (box1.maxX < box2.minX || box1.minX > box2.maxX ||
		box1.maxY < box2.minY || box1.minY > box2.maxY) {
		return false;
	}
	return true;
}

AABB ORKAJumpPlatform::getAABB() {
	AABB aabb;
	aabb.minX = pos.x - width / 2;
	aabb.maxX = pos.x + width / 2;
	aabb.minY = pos.y - height / 2;
	aabb.maxY = pos.y + height / 2;
	return aabb;
}

void ORKAJumper::update(Window& window, Float delta, ORKAJumpRenderer* game) {
	if (window.isKeyPressed(Key::A))
	{
		velocity.x -= speed * delta;
		if (velocity.x > 0) velocity.x = 0;
	}
	if (window.isKeyPressed(Key::D))
	{
		velocity.x += speed * delta;
		if (velocity.x < 0) velocity.x = 0;
	}

	velocity.y -= gravity * delta;

	pos.x += velocity.x * delta;

	aabb.maxY = pos.y;
	pos.y += velocity.y * delta;

	if (game->lavaLevel + 10 > pos.y) {
		game->paused = true;
		if (game->score >= game->highscore) game->highscore = game->score;
	}

	aabb.minY = pos.y - size / 2;
	aabb.minX = pos.x - size / 2;
	aabb.maxX = pos.x + size / 2;

	if (velocity.y <= 0) {
		for (auto p : game->platforms) {
			if (isColliding(aabb, p.getAABB())) {
				velocity.y = p.jumpForce;
			}
		}
	}

	velocity.x -= velocity.x * drag * delta;

	game->lavaLevel -= pos.y;

	for (auto& p : game->platforms) {
		p.pos -= pos;
		while (p.pos.y < -10.0) {
			p.randomizeX(game->platforms);
			p.pos.y += 20.0;
		}
		while (p.pos.y > +10.0) p.pos.y -= 20.0;
	}
	totalHeight += pos.y;
	if (totalHeight > game->score) game->score = totalHeight;
	pos = Vec2(0, 0);
}