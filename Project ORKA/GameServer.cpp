
#include "Program.hpp"

void processTransformations(std::vector<Transformation> & transformations, Time & time) {
	for (int i = 0; i < transformations.size(); i++) {
		//transformations[i].location.z = sin(0.1 * transformations[i].location.x + 0.1 * transformations[i].location.y + time.getTotal());
		transformations[i].location += glm::vec3(randomFloat(-0.1, 0.1), randomFloat(-0.1, 0.1), randomFloat(-0.1, 0.1));
	}
}

void GameServerThread(GameServer & gameServer) {

	std::chrono::steady_clock::time_point t;

	while (gameServer.keepThreadRunning) {
		t = std::chrono::steady_clock::now();
		updateTime(gameServer.gameTime);
		updateTime(gameServer.serverTime);
		
		if (!gameServer.gameTime.paused) {
			processTransformations(gameServer.worldSystem.chunk.entityComponentSystem.transformationSystem.transformations, gameServer.gameTime);
		}
		
		pocessCameras(gameServer.worldSystem.chunk.entityComponentSystem.cameraSystem);

		t += std::chrono::milliseconds(16);
		std::this_thread::sleep_until(t);
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