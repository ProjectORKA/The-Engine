#include "PlanetRenderSystem.hpp"

void PlanetRenderSystem::count()
{
	UInt count = 0;
	for (Vector<ChunkRenderInfo>& vecs : chunkLevels) {
		for (ChunkRenderInfo& chunkInfo : vecs) {
			count++;
		}
	};
	logDebug(count);
}
void PlanetRenderSystem::create()
{
	chunkLevels.clear();
	for (int i = 0; i < 64; i++) {
		chunkLevels.emplace_back();
	}
}
void PlanetRenderSystem::update(WorldChunk& chunk, Camera& camera)
{
	//check if starting from top
	if (chunk.level == 0) {

		//fresh run, clear everything
		for (Vector<ChunkRenderInfo>& vecs : chunkLevels) {
			vecs.clear();
		}
	}

	//magic math stuff
	////////////////////////////////////////////////////////////////////////////////////////
	ULL wx = chunk.xLocation << (64 - chunk.level);										   
	ULL wy = chunk.yLocation << (64 - chunk.level);										   
	ULL wz = chunk.zLocation << (64 - chunk.level);		
	ULL cx = camera.chunkLocation.x;
	ULL cy = camera.chunkLocation.y;
	ULL cz = camera.chunkLocation.z;

	LL  lx = wx - cx;
	LL  ly = wy - cy;

	Vec3 chunkOffset;
	chunkOffset.x = lx;
	chunkOffset.y = ly;

	if (wz > cz) {
		ULL uz = wz - cz;
		chunkOffset.z = uz;
	}
	else {
		ULL uz = cz - wz;
		chunkOffset.z = uz;
		chunkOffset.z = -chunkOffset.z;
	};

	chunkOffset -= camera.location;

	if (chunkOffset.x + pow(2, 63 - chunk.level) > +pow(2, 63)) chunkOffset.x -= pow(2, 64);
	if (chunkOffset.y + pow(2, 63 - chunk.level) > +pow(2, 63)) chunkOffset.y -= pow(2, 64);
	if (chunkOffset.x + pow(2, 63 - chunk.level) < -pow(2, 63)) chunkOffset.x += pow(2, 64);
	if (chunkOffset.y + pow(2, 63 - chunk.level) < -pow(2, 63)) chunkOffset.y += pow(2, 64);
	chunkOffset /= Vec3(pow(2, 64 - chunk.level));											
	////////////////////////////////////////////////////////////////////////////////////////

	Bool shouldRender = true;

	//check if within render range
	if (glm::length(chunkOffset + Vec3(0.5)) < renderDistance) {
		
		chunk.isInUse();
		
		shouldRender = false;

		//now do the same with all children
		if (chunk.subdivided) {
			update(*chunk.c000, camera);
			update(*chunk.c001, camera);
			update(*chunk.c010, camera);
			update(*chunk.c011, camera);
			update(*chunk.c100, camera);
			update(*chunk.c101, camera);
			update(*chunk.c110, camera);
			update(*chunk.c111, camera);
		}
		else {
			shouldRender = true;
		}
	}

	if (shouldRender) {
		if (chunk.zLocation == 0) {
			//create renderInfo
			ChunkRenderInfo tmp;
			tmp.chunkOffset = chunkOffset;
			tmp.chunk = &chunk;
			
			//add it to the render queue
			chunkLevels[chunk.level].push_back(tmp);
		}
	}
}
void PlanetRenderSystem::render(Uniforms& uniforms, RenderObjectSystem& renderObjectSystem)
{
	//count();
	uniforms.bools["distortion"] = worldDistortion;
	for (Vector<ChunkRenderInfo>& vecs : chunkLevels) {
		for (ChunkRenderInfo& chunkInfo : vecs) {
			chunkInfo.render(uniforms, renderObjectSystem, chunkBorders);
		}
		glClear(GL_DEPTH_BUFFER_BIT);
	}
}

void ChunkRenderInfo::render(Uniforms& uniforms, RenderObjectSystem& renderObjectSystem, Bool chunkBorders)
{
	uniforms.setVec3("chunkOffsetVector", chunkOffset);
	uniforms.setVec4("worldOffset", Vec4(chunk->xLocation, chunk->yLocation, chunk->zLocation,chunk->level));
	if (chunk->zLocation == 0) renderObjectSystem.render("terrain");
	if (chunkBorders)renderObjectSystem.render("boundingBox");
}