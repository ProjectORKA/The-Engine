
#include "Program.hpp"

void GameServerThread(GameServer & gameServer) {
	while (gameServer.keepThreadRunning) {
		updateTime(gameServer.time);
		gameServer.worldSystem.sky.skyColor = glm::vec3((1 + sin((float)gameServer.time.getTotal())) / 2.0f);
		gameServer.worldSystem.chunk.entityComponentSystem.transformationSystem.transformations[0].scale = glm::vec3(1);
		gameServer.worldSystem.chunk.entityComponentSystem.transformationSystem.transformations[0].location.z = sin(gameServer.time.getTotal());
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