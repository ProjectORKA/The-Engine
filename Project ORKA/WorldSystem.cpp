
#include "Program.hpp"

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

void chunkIsInUse(std::shared_ptr<Chunk> chunk){
	chunk->expirationDate = std::chrono::steady_clock::now() + std::chrono::milliseconds(1000);
}

void generateTerrain(std::shared_ptr<Chunk> chunk) {
	Chunk& rChunk = *chunk;
	
	static siv::PerlinNoise noise;

	noise.reseed(12345);

	long double x = ((unsigned long long)(rChunk.location.x << (64 - rChunk.level)))/(long double)LLONG_MAX/4;
	long double y = ((unsigned long long)(rChunk.location.y << (64 - rChunk.level)))/(long double)LLONG_MAX/4;
	long double z = (unsigned long long)(rChunk.location.z << (64 - rChunk.level));

	long double lowBorder = (long double)(((unsigned long long)(rChunk.location.z)) << (64 - rChunk.level));
	long double highBorder = (long double)(((unsigned long long)(rChunk.location.z+1)) << (64 - rChunk.level));
	long double target = ((long double)noise.octaveNoise0_1(x, y, 16)) * (long double)LLONG_MAX/2;
	
	
	std::cout << x << "/" << y << std::endl;
	//std::cout << target << std::endl;

	if (rChunk.location.z == 0) {
		rChunk.terrain.hasTerrain = true;
		rChunk.terrain.height = (float)(target / pow(2,64 - rChunk.level));
	}

}

void renderChunk(ChunkRenderInfo& info, Renderer& renderer) {
	//render contents
	renderEntities(info, renderer);

	renderTerrain(info, renderer);

	//render debug
	if (renderer.settings.chunkBorders) {
		renderChunkBoundingBox(info, renderer);
	}
}

void renderWorld(WorldSystem& worldSystem, Renderer& renderer) {
	processChunkQueue(worldSystem.octreeRoot, renderer);
	renderChunkQueue(renderer);
}

void renderTerrain(ChunkRenderInfo & info, Renderer & renderer) {
	if (info.chunk->terrain.hasTerrain) {
		
		useShaderProgram(renderer.primitiveChunkShaderProgram);

		//apply unifforms
		glUniform1i(renderer.primitiveChunkShaderProgram.distortionID, renderer.settings.distortion);
		glUniform1f(renderer.primitiveChunkShaderProgram.timeID, renderer.renderTime.total());
		glUniform4fv(renderer.primitiveChunkShaderProgram.worldOffsetID, 1, glm::value_ptr(glm::vec4(info.chunk->location, info.chunk->level)));
		glUniformMatrix4fv(renderer.primitiveChunkShaderProgram.vpMatrixID, 1, GL_FALSE, glm::value_ptr(renderer.projectionMatrix * renderer.viewMatrix));
		glUniformMatrix4fv(renderer.primitiveChunkShaderProgram.mMatrixID, 1, GL_FALSE, glm::value_ptr(glm::translate(glm::mat4(1), glm::vec3(0, 0, info.chunk->terrain.height))));
		glUniform3fv(renderer.primitiveChunkShaderProgram.chunkOffsetVectorID, 1, glm::value_ptr(info.chunkOffsetVector));

		unsigned int meshIndex;
		getMeshIndexFromName(renderer.meshSystem, "terrain", meshIndex);
		bindMesh(renderer.meshSystem, meshIndex);
		renderMesh(renderer.meshSystem, meshIndex);
		unbindMesh();
	}
}

void dynamicallyAdjustValue(unsigned int& value, Renderer& renderer) {
	if (renderer.settings.adjustRenderVariables) {
		float minimumFrameTimeMatchup = renderer.renderTime.delta() * renderer.settings.minimumFrameRate;
		if (minimumFrameTimeMatchup > 1) {
			if (value != 0) {
				value /= 1.03;
			}
		}
		else {
			if (renderer.renderTime.delta() * renderer.settings.maximumFrameRate < 1 & value < 1000) {
				value++;
			}
		}
	}
}

void renderChunkBoundingBox(ChunkRenderInfo& info, Renderer& renderer) {
	//use shader
	useShaderProgram(renderer.primitiveChunkShaderProgram);

	glUniform1i(renderer.primitiveChunkShaderProgram.distortionID, renderer.settings.distortion);

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

void processChunkQueue(std::shared_ptr<Chunk> chunk, Renderer& renderer) {
	chunk->mutex.lock_shared();
	Chunk& rChunk = *chunk;
	
	//get location of chunk
	//unsigned short levelBasedBitShift = 64 - chunk.level;
	unsigned long long offset = ULLONG_MAX >> (rChunk.level + 1);

	long long x = rChunk.location.x << 64 - rChunk.level;	//converts to actual position
	long long y = rChunk.location.y << 64 - rChunk.level;	//converts to actual position
	long long z = rChunk.location.z << 64 - rChunk.level; 	//converts to actual position
	//unsigned long long z = rChunk.location.z << 64 - rChunk.level;

	//subtract location of camera to get delta/distance
	x -= renderer.camera.location.x - offset;
	y -= renderer.camera.location.y - offset;
	z -= renderer.camera.location.z - offset;

	//glm::vec3 delta(x, y, 0);

	//unsigned long long cameraLoc = renderer.camera.location.z - offset;

	//if (cameraLoc < z) {
	//	z = z - cameraLoc;
	//	delta.z = z;
	//}
	//else {
	//	z = cameraLoc - z;
	//	delta.z = z;
	//	delta.z *= -1;
	//}


	//turn integer to float
	glm::vec3 delta(x, y, z);

	//imitate scale by moving it closer/further instead of scaling (allows for constant clipping)
	delta /= pow(2, 64 - rChunk.level);

	//check visibility
	bool isLowestLevel = rChunk.level >= CHUNK_LEVEL_MAX;
	bool tooBig = rChunk.level < 4;
	bool hasRenderableContent = rChunk.terrain.hasTerrain | tooBig | (rChunk.entities.size() > 0);
	bool isInRenderDistance = glm::length(delta) < glm::pow(CHUNK_DISTANCE_MULTIPLIER, renderer.settings.worldSystemRenderDistance);
	//chunks need entities inside to be rendered
	if (hasRenderableContent) {
		if (tooBig | (isInRenderDistance & !isLowestLevel)) {
			//too close to be rendered
			//will be subdivided and checked
			chunkIsInUse(chunk);
			if (rChunk.subdivided) {
				processChunkQueue(rChunk.tfr, renderer);
				processChunkQueue(rChunk.tfl, renderer);
				processChunkQueue(rChunk.tbr, renderer);
				processChunkQueue(rChunk.tbl, renderer);
				processChunkQueue(rChunk.bfr, renderer);
				processChunkQueue(rChunk.bfl, renderer);
				processChunkQueue(rChunk.bbr, renderer);
				processChunkQueue(rChunk.bbl, renderer);

				//delta -= (renderer.camera.subChunkLocation / (float)pow(2, 64 - rChunk.level));
				//ChunkRenderInfo info;
				//info.chunk = chunk;
				//info.chunkOffsetVector = delta;
				//renderer.chunkRenderQueues[rChunk.level].push_back(info);
			}
			else {
				//no children to render
				//so we take this one instead
				delta -= (renderer.camera.subChunkLocation / (float)pow(2, 64 - rChunk.level));
				ChunkRenderInfo info;
				info.chunk = chunk;
				info.chunkOffsetVector = delta;
				renderer.chunkRenderQueues[rChunk.level].push_back(info);
			}
		}
		else {
			//just outside minimum distance
			//perfect
			//now render
			delta -= (renderer.camera.subChunkLocation / (float)pow(2, 64 - rChunk.level));
			ChunkRenderInfo info;
			info.chunk = chunk;
			info.chunkOffsetVector = delta;
			renderer.chunkRenderQueues[rChunk.level].push_back(info);
		}
	}
	rChunk.mutex.unlock_shared();
}

void subdivideChunk(std::shared_ptr<Chunk> chunk, GameSimulation& gameSimulation) {
	Chunk& rChunk = *chunk;
	if (rChunk.subdivided == false && rChunk.level < CHUNK_LEVEL_MAX) {
		rChunk.tfr = std::make_shared<Chunk>();
		rChunk.tfl = std::make_shared<Chunk>();
		rChunk.tbr = std::make_shared<Chunk>();
		rChunk.tbl = std::make_shared<Chunk>();
		rChunk.bfr = std::make_shared<Chunk>();
		rChunk.bfl = std::make_shared<Chunk>();
		rChunk.bbr = std::make_shared<Chunk>();
		rChunk.bbl = std::make_shared<Chunk>();
		createChildChunk(rChunk.tfr, chunk, gameSimulation, 1, 1, 1);
		createChildChunk(rChunk.tfl, chunk, gameSimulation, 0, 1, 1);
		createChildChunk(rChunk.tbr, chunk, gameSimulation, 1, 0, 1);
		createChildChunk(rChunk.tbl, chunk, gameSimulation, 0, 0, 1);
		createChildChunk(rChunk.bfr, chunk, gameSimulation, 1, 1, 0);
		createChildChunk(rChunk.bfl, chunk, gameSimulation, 0, 1, 0);
		createChildChunk(rChunk.bbr, chunk, gameSimulation, 1, 0, 0);
		createChildChunk(rChunk.bbl, chunk, gameSimulation, 0, 0, 0);

		rChunk.subdivided = true;


	}
};

void generateEntities(std::shared_ptr<Chunk> chunk, GameSimulation & gameSimulation) {
	Chunk& rChunk = *chunk;

	if (rChunk.location == glm::u64vec3(0) & rChunk.level) {
		rChunk.entities.emplace_back();
		addTransformation(rChunk.entities.back(), gameSimulation);
		gameSimulation.transformationSystem.transformations.back().setScale(0.01);
		gameSimulation.transformationSystem.transformations.back().setTranslation(glm::vec3(-0.5));
		addModelMatrix(rChunk.entities.back(), gameSimulation.modelMatrixSystem);
		addRenderingComponent(rChunk.entities.back(), gameSimulation, "icosphere");
	}
}

void unsubdivideChunk(std::shared_ptr<Chunk> chunk, GameSimulation & gameSimulation) {
	chunk->mutex.lock();
	Chunk& rChunk = *chunk;
	rChunk.subdivided = false;
	rChunk.tfr.reset();
	rChunk.tfl.reset();
	rChunk.tbr.reset();
	rChunk.tbl.reset();
	rChunk.bfr.reset();
	rChunk.bfl.reset();
	rChunk.bbr.reset();
	rChunk.bbl.reset();
	rChunk.mutex.unlock();
};

void createChildChunk(std::shared_ptr<Chunk> child, std::shared_ptr<Chunk> parent, GameSimulation& gameSimulation, bool x, bool y, bool z) {
	
	Chunk& rChild = *child;
	Chunk& rParent = *parent;
	
	rChild.level = rParent.level + 1;
	rChild.location.x = (rParent.location.x << 1) + x;
	rChild.location.y = (rParent.location.y << 1) + y;
	rChild.location.z = (rParent.location.z << 1) + z;

	generateEntities(child, gameSimulation);
	generateTerrain(child);
}