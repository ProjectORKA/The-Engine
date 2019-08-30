
#include "Program.hpp"

//WORLD SYSTEM
WorldSystem::WorldSystem()
{
}

void renderWorld(WorldSystem & worldSystem, Renderer & renderer) {
	renderSky(worldSystem.sky);
	renderGizmo(renderer);
	renderChunksByLevel(worldSystem.root, renderer);
}

void renderGizmo(Renderer & renderer) {
	useShader(renderer.primitiveShader);

	std::vector<int> meshIndices;
	getMeshIndicesFromName(renderer.meshSystem, "gizmo", meshIndices);
	for (int i = 0; i < meshIndices.size(); i++) {

		bindMesh(renderer.meshSystem, meshIndices[i]);

		glm::mat4 modelMatrix = glm::mat4(1);
		glm::mat4 mvpMatrix = renderer.projectionMatrix * renderer.viewMatrix * modelMatrix;
		glUniformMatrix4fv(renderer.primitiveShader.mvpMatrixID, 1, GL_FALSE, &mvpMatrix[0][0]);

		renderMesh(renderer.meshSystem, meshIndices[i]);

		unbindMesh();
	}
}

//CHUNK
void subdivideChunk(Chunk & chunk)
{
	int newLevel = chunk.level + 1;
	int newPosXn = chunk.posX << 1;
	int newPosYn = chunk.posY << 1;
	int newPosZn = chunk.posZ << 1;
	int newPosXp = newPosXn + 1;
	int newPosYp = newPosYn + 1;
	int newPosZp = newPosZn + 1;

	if (chunk.pXpYpZ == nullptr) chunk.pXpYpZ = new Chunk(newLevel, newPosXp, newPosYp, newPosZp);
	if (chunk.nXpYpZ == nullptr) chunk.nXpYpZ = new Chunk(newLevel, newPosXn, newPosYp, newPosZp);
	if (chunk.pXnYpZ == nullptr) chunk.pXnYpZ = new Chunk(newLevel, newPosXp, newPosYn, newPosZp);
	if (chunk.nXnYpZ == nullptr) chunk.nXnYpZ = new Chunk(newLevel, newPosXn, newPosYn, newPosZp);
	if (chunk.pXpYnZ == nullptr) chunk.pXpYnZ = new Chunk(newLevel, newPosXp, newPosYp, newPosZn);
	if (chunk.nXpYnZ == nullptr) chunk.nXpYnZ = new Chunk(newLevel, newPosXn, newPosYp, newPosZn);
	if (chunk.pXnYnZ == nullptr) chunk.pXnYnZ = new Chunk(newLevel, newPosXp, newPosYn, newPosZn);
	if (chunk.nXnYnZ == nullptr) chunk.nXnYnZ = new Chunk(newLevel, newPosXn, newPosYn, newPosZn);
}

void unsubdivideChunk(Chunk & chunk) {
	delete chunk.pXpYpZ;
	delete chunk.nXpYpZ;
	delete chunk.pXnYpZ;
	delete chunk.nXnYpZ;
	delete chunk.pXpYnZ;
	delete chunk.nXpYnZ;
	delete chunk.pXnYnZ;
	delete chunk.nXnYnZ;
};

void renderChunkBoundingBox(Chunk & chunk, Renderer & renderer) {
	useShader(renderer.primitiveShader);

	std::vector<int> meshIndices;
	getMeshIndicesFromName(renderer.meshSystem, "bounds", meshIndices);
	for (int i = 0; i < meshIndices.size(); i++) {

		bindMesh(renderer.meshSystem, meshIndices[i]);

		glm::mat4 modelMatrix = glm::mat4(1);
		glm::mat4 mvpMatrix = renderer.projectionMatrix * renderer.chunkAdjustedViewMatrix * modelMatrix;
		glUniformMatrix4fv(renderer.primitiveShader.mvpMatrixID, 1, GL_FALSE, &mvpMatrix[0][0]);

		renderMesh(renderer.meshSystem, meshIndices[i]);

		unbindMesh();
	}
}

void renderChunksByLevel(Chunk & chunk, Renderer & renderer) {

	std::vector<Chunk *> chunksToRender;
	chunksToRender.push_back(&chunk);
	std::vector<Chunk *> children;

	while (chunksToRender.size() > 0) {
		glClear(GL_DEPTH_BUFFER_BIT);

		for (int i = 0; i < chunksToRender.size(); i++) {
			renderChunk(*chunksToRender[i], renderer, children);
		}

		chunksToRender = children;
		children.clear();
	}
}

void renderChunk(Chunk & chunk, Renderer & renderer, std::vector<Chunk *> & children) {
	renderer.chunkAdjustedViewMatrix = renderer.viewMatrix;
	renderer.chunkAdjustedViewMatrix = glm::translate(renderer.chunkAdjustedViewMatrix, glm::vec3(1 - pow(2, chunk.level)) * renderer.camera->cameraLocation);
	renderer.chunkAdjustedViewMatrix = glm::translate(renderer.chunkAdjustedViewMatrix, glm::vec3(1 + 2 * chunk.posX, 1 + 2 * chunk.posY, 1 + 2 * chunk.posZ));

	//renderEntities(chunk.ecs, renderer);
	renderChunkBoundingBox(chunk, renderer);

	if (chunk.pXpYpZ != nullptr) children.push_back(chunk.pXpYpZ);
	if (chunk.nXpYpZ != nullptr) children.push_back(chunk.nXpYpZ);
	if (chunk.pXnYpZ != nullptr) children.push_back(chunk.pXnYpZ);
	if (chunk.nXnYpZ != nullptr) children.push_back(chunk.nXnYpZ);
	if (chunk.pXpYnZ != nullptr) children.push_back(chunk.pXpYnZ);
	if (chunk.nXpYnZ != nullptr) children.push_back(chunk.nXpYnZ);
	if (chunk.pXnYnZ != nullptr) children.push_back(chunk.pXnYnZ);
	if (chunk.nXnYnZ != nullptr) children.push_back(chunk.nXnYnZ);
}

Chunk::Chunk()
{
	spawnEntity(ecs, "terrain");

	//spawnEntity(entityComponentSystem, "monkey");
	int gridSize = 50;

	for (int x = 0; x < gridSize; x++) {
		for (int y = 0; y < gridSize; y++) {

			static int flopper = 0;

			spawnEntity(ecs, ecs.entityTypes.names[flopper]);

			if (ecs.entityTypes.structure[flopper][0]) {
				ecs.transformationSystem.transformations.back().location.x = 3.0f * (x - gridSize / 2.0f);
				ecs.transformationSystem.transformations.back().location.y = 3.0f * (y - gridSize / 2.0f);
			}

			flopper++;

			if (flopper >= ecs.entityTypes.numberOfEntityTypes) {
				flopper = 0;
			}
		}
	}

	debugPrint("|--Chunk was created!");
}

Chunk::Chunk(int level, int x, int y, int z)
{
	this->level = level;
	this->posX = x;
	this->posY = y;
	this->posZ = z;

	//if(posZ == 0) spawnEntity(entityComponentSystem, "terrain");

	//spawnEntity(entityComponentSystem, "monkey");

	debugPrint("|--Chunk was created!");
}

Chunk::~Chunk()
{
	unsubdivideChunk(*this);

	debugPrint("|--Chunk was destroyed!");
}
Sky::Sky() {
	skyColor = glm::vec3(0.0f);
}