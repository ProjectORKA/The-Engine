
#include "Program.hpp"

void createChildChunk(Chunk& child, Chunk& parent, GameSimulation& gameSimulation, bool x, bool y, bool z) {
	child.level = parent.level + 1;
	child.location.x = (parent.location.x << 1) + x;
	child.location.y = (parent.location.y << 1) + y;
	child.location.z = (parent.location.z << 1) + z;

	generateEntities(child, gameSimulation);
}

void chunkIsInUse(Chunk & chunk){
	chunk.expirationDate = std::chrono::steady_clock::now() + std::chrono::milliseconds(100);
}

void generateEntities(Chunk & chunk, GameSimulation & gameSimulation) {
	if (chunk.location.z == 0) {

		chunk.entities.emplace_back();
		addTransformation(chunk.entities.back(), gameSimulation);
		addModelMatrix(chunk.entities.back(), gameSimulation.modelMatrixSystem);
		addRenderingComponent(chunk.entities.back(), gameSimulation, "terrain");
	}
}

void subdivideChunk(Chunk& chunk, GameSimulation& gameSimulation) {
	if (chunk.subdivided == false && chunk.level < CHUNK_LEVEL_MAX) {
		chunk.tfr = std::make_unique<Chunk>();
		chunk.tfl = std::make_unique<Chunk>();
		chunk.tbr = std::make_unique<Chunk>();
		chunk.tbl = std::make_unique<Chunk>();
		chunk.bfr = std::make_unique<Chunk>();
		chunk.bfl = std::make_unique<Chunk>();
		chunk.bbr = std::make_unique<Chunk>();
		chunk.bbl = std::make_unique<Chunk>();
		createChildChunk(*chunk.tfr, chunk, gameSimulation, 1, 1, 1);
		createChildChunk(*chunk.tfl, chunk, gameSimulation, 0, 1, 1);
		createChildChunk(*chunk.tbr, chunk, gameSimulation, 1, 0, 1);
		createChildChunk(*chunk.tbl, chunk, gameSimulation, 0, 0, 1);
		createChildChunk(*chunk.bfr, chunk, gameSimulation, 1, 1, 0);
		createChildChunk(*chunk.bfl, chunk, gameSimulation, 0, 1, 0);
		createChildChunk(*chunk.bbr, chunk, gameSimulation, 1, 0, 0);
		createChildChunk(*chunk.bbl, chunk, gameSimulation, 0, 0, 0);

		chunk.subdivided = true;
	}
};

void unsubdivideChunk(Chunk& chunk, GameSimulation & gameSimulation) {
	chunk.mutex.lock();
	chunk.subdivided = false;
	chunk.tfr.reset();
	chunk.tfl.reset();
	chunk.tbr.reset();
	chunk.tbl.reset();
	chunk.bfr.reset();
	chunk.bfl.reset();
	chunk.bbr.reset();
	chunk.bbl.reset();
	chunk.mutex.unlock();
};

void processChunkQueue(Chunk& chunk, Renderer& renderer) {
	chunk.mutex.lock();
	//get location of chunk
	//unsigned short levelBasedBitShift = 64 - chunk.level;
	unsigned long long offset = ULLONG_MAX >> (chunk.level + 1);

	long long x = chunk.location.x << 64 - chunk.level;		//converts to actual position
	long long y = chunk.location.y << 64 - chunk.level;		//converts to actual position
	long long z = chunk.location.z << 64 - chunk.level; 	//converts to actual position

	//subtract location of camera to get delta/distance
	x -= renderer.camera.location.x - offset;
	y -= renderer.camera.location.y - offset;
	z -= renderer.camera.location.z - offset;

	//turn integer to float
	glm::vec3 delta(x, y, z);

	//imitate scale by moving it closer/further instead of scaling (allows for constant clipping)
	delta /= pow(2, 64 - chunk.level);

	//check visibility
	bool isLowest = chunk.level >= CHUNK_LEVEL_MAX;
	bool hasEntities = chunk.entities.size() > 0;
	bool tooBig = chunk.level < 4;
	bool isInRenderDistance = glm::length(delta) < glm::pow(CHUNK_DISTANCE_MULTIPLIER, renderer.worldSystemRenderDistance);
	//chunks need entities inside to be rendered
	if (hasEntities) {
		if (tooBig | (isInRenderDistance & !isLowest)) {
			//too close to be rendered
			//will be subdivided and checked
			chunkIsInUse(chunk);
			if (chunk.subdivided) {
				processChunkQueue(*chunk.tfr, renderer);
				processChunkQueue(*chunk.tfl, renderer);
				processChunkQueue(*chunk.tbr, renderer);
				processChunkQueue(*chunk.tbl, renderer);
				processChunkQueue(*chunk.bfr, renderer);
				processChunkQueue(*chunk.bfl, renderer);
				processChunkQueue(*chunk.bbr, renderer);
				processChunkQueue(*chunk.bbl, renderer);
			}
			else {
				//no children to render
				//so we take this one instead
				delta -= (renderer.camera.subChunkLocation / (float)pow(2, 64 - chunk.level));
				ChunkRenderInfo info;
				info.chunk = &chunk;
				info.chunkOffsetVector = delta;
				renderer.chunkRenderQueues[chunk.level].push_back(info);
			}
		}
		else {
			//just outside minimum distance
			//perfect
			//now render
			delta -= (renderer.camera.subChunkLocation / (float)pow(2, 64 - chunk.level));
			ChunkRenderInfo info;
			info.chunk = &chunk;
			info.chunkOffsetVector = delta;
			renderer.chunkRenderQueues[chunk.level].push_back(info);
		}
	}
	chunk.mutex.unlock();
}

void dynamicallyAdjustValue(unsigned int& value, Renderer& renderer) {
	if (renderer.adjustRenderVariables) {
		if (renderer.renderTime.delta() > (1.0 / renderer.minimumFrameRate)) {
			if(value != 0) value--;
		}
		else {
			if (renderer.renderTime.delta() < (1.0 / renderer.maximunFrameRate)) {
				value++;
			}
		}
	}
}

void renderWorld(WorldSystem& worldSystem, Renderer& renderer) {
	renderSky(worldSystem.sky);
	processChunkQueue(worldSystem.root, renderer);
	renderChunkQueue(renderer);
}

void renderChunkQueue(Renderer& renderer) {
	for (int level = 0; level < renderer.chunkRenderQueues.size(); level++) {
		for (int chunk = 0; chunk < renderer.chunkRenderQueues[level].size(); chunk++) {
			renderChunk(renderer.chunkRenderQueues[level][chunk], renderer);
			if (level == 2) renderGizmo(renderer.chunkRenderQueues[level][chunk], renderer);
		}
		renderer.chunkRenderQueues[level].clear();
		glClear(GL_DEPTH_BUFFER_BIT);
	}
}

void renderChunk(ChunkRenderInfo& info, Renderer& renderer) {
	//render contents
	renderEntities(info, renderer);

	//render debug
	if (renderer.chunkBorders) {
		renderChunkBoundingBox(info, renderer);
	}
}

void renderChunkBoundingBox(ChunkRenderInfo& info, Renderer& renderer) {
	//use shader
	useShaderProgram(renderer.primitiveChunkShaderProgram);

	glUniform1f(renderer.primitiveChunkShaderProgram.timeID, renderer.renderTime.total());

	//apply uniforms
	glm::vec4 worldOffset = glm::vec4(info.chunk->location, info.chunk->level);
	glUniform4fv(renderer.primitiveChunkShaderProgram.worldOffsetID, 1, glm::value_ptr(worldOffset));

	glm::mat4 mMatrix = glm::mat4(1);
	glUniformMatrix4fv(renderer.primitiveChunkShaderProgram.mMatrixID, 1, GL_FALSE, glm::value_ptr(mMatrix));

	glUniform3fv(renderer.primitiveChunkShaderProgram.chunkOffsetVectorID, 1, glm::value_ptr(info.chunkOffsetVector));

	glm::mat4 vpMatrix = renderer.projectionMatrix * renderer.viewMatrix;
	glUniformMatrix4fv(renderer.primitiveChunkShaderProgram.vpMatrixID, 1, GL_FALSE, glm::value_ptr(vpMatrix));

	//render meshes
	unsigned int meshIndex;
	getMeshIndexFromName(renderer.meshSystem, "bounding box", meshIndex);

	bindMesh(renderer.meshSystem, meshIndex);
	renderMesh(renderer.meshSystem, meshIndex);
	unbindMesh();

	glUseProgram(0);
}