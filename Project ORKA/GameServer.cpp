
#include "Program.hpp"

void GameServerThread(GameServer & gameServer) {

	gameServer.gameTime.paused = true;

	std::chrono::steady_clock::time_point t;

	while (gameServer.keepThreadRunning) {
		t = std::chrono::steady_clock::now();
		updateTime(gameServer.gameTime);
		updateWorld(gameServer.worldSystem, gameServer.gameTime);

		t += std::chrono::milliseconds(16); //game server running at 60 Hz
		std::this_thread::sleep_until(t);
	}
}

void updateWorld(WorldSystem & worldSystem, Time & time) {
	processTransformations(worldSystem.ecs.transformationSystem, time);
};

void processTransformations(TransformationSystem & transformationSystem, Time & time) {

	//apply motion to every object
	if (!time.paused) {
		for (int i = 0; i < transformationSystem.transformations.size(); i++) {

			float distance = sqrt(glm::length(transformationSystem.transformations[i].location));
			glm::mat4 rotMat = glm::rotate(glm::mat4(1), distance / 100.0f, glm::vec3(0, 0, 1));
			transformationSystem.transformations[i].location = rotMat * glm::vec4(transformationSystem.transformations[i].location, 1);
			
			//float randomSpeed = 100.0f;
			//transformationSystem.transformations[i].location += glm::vec3(randomFloat(-randomSpeed, randomSpeed), randomFloat(-randomSpeed, randomSpeed), 0);
			//transformationSystem.transformations[i].rotation += glm::vec3(randomFloat(-0.05, 0.05), randomFloat(-0.05, 0.05), randomFloat(-0.05, 0.05));
			
			transformationSystem.transformations[i].location.z = 30 * sin(0.01 * transformationSystem.transformations[i].location.x + (float)time.getTotal()) * sin(0.01 * transformationSystem.transformations[i].location.y + time.getTotal());
		}
	}

	//calculate modelMatrices
	transformationSystem.modelMatrices.resize(transformationSystem.transformations.size());
	for (int i = 0; i < transformationSystem.transformations.size(); i++) {
		calculateModelMatrix(transformationSystem.modelMatrices[i], transformationSystem.transformations[i]);
	}
}

GameServer::GameServer() {
	thread = std::make_unique<std::thread>(GameServerThread, std::ref(*this));
	debugPrint("|-GameServer was created!");
}

GameServer::~GameServer() {
	keepThreadRunning = false;
	thread->join();
	debugPrint("|-GameServer was destroyed!");
}