#include "TripleNine.hpp"
#include "Crosshair.hpp"
#include <algorithm>

Index TripleNineEnemy::getId() const
{
	return id;
}

Vec3 TripleNineEnemy::getPosition() const
{
	return position;
}

Vector<TripleNineEnemy>& TripleNineGameState::getEnemies()
{
	return enemies;
}

const Vector<TripleNineEnemy>& TripleNineGameState::getEnemies() const
{
	return enemies;
}

void TripleNineSimulation::createEnemy()
{
	constexpr Float spawnRadius = 85.0f;
	TripleNineEnemy& newEnemy = gameState.createEnemy();
	newEnemy.setPosition(Vec3(randomVec2Fast(-spawnRadius, spawnRadius), 0.0f));
}

TripleNineEnemy& TripleNineGameState::createEnemy()
{
	enemies.emplace_back();
	enemies.back().id = static_cast<Index>(enemies.size()) - static_cast<Index>(1);
	return enemies.back();
}

void TripleNineEnemy::setPosition(const Vec3 newPosition)
{
	position = newPosition;
}

// GameState

void TripleNineGameState::updatePlayer(const PlayerID playerID, const Vec3 position)
{
	for (TripleNinePlayerData& player : players)
	{
		if (player.playerID == playerID)
		{
			player.position = position;
			return;
		}
	}
	players.push_back({playerID, position});
}

Vec3 TripleNineGameState::getPlayerPosition(const PlayerID playerID) const
{
	for (auto& player : players)
	{
		if (player.playerID == playerID) return player.position;
	}
	return Vec3(0);
}

ULL TripleNineGameState::getTick() const
{
	return gameTick;
}

// Simulation

void TripleNineSimulation::create()
{
	server.create();
	loaded = true;
	for (Int i = 0; i < 100; i++) createEnemy();
}

void TripleNineSimulation::destroy()
{
	server.destroy();
}

void TripleNineSimulation::update(Float delta)
{
	(void)delta;
	server.processEvents();
}

void TripleNineSimulation::updatePlayer(const Byte playerId, const Vec3 position)
{
	gameState.updatePlayer(playerId, position);
}

// Renderer

void TripleNineRenderer::update(Window& window)
{
	player.update(window);
	client.processEvents();
	client.sendPlayerData(player.position, Vec3(player.camera.getRotation()));
}

void TripleNineRenderer::create(Window& window)
{
	GameRenderer::create(window);
	client.create();

	Renderer& r = window.renderer;

	bloom1Framebuffer.create(r, "Bloom1Framebuffer", IVec2(1));
	bloom1Framebuffer.add(r, WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Clamped);
	bloom1Framebuffer.checkComplete(r);

	bloom2Framebuffer.create(r, "Bloom2Framebuffer", IVec2(1));
	bloom2Framebuffer.add(r, WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Clamped);
	bloom2Framebuffer.checkComplete(r);

	bloom3Framebuffer.create(r, "Bloom3Framebuffer", IVec2(1));
	bloom3Framebuffer.add(r, WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Clamped);
	bloom3Framebuffer.checkComplete(r);

	bloom4Framebuffer.create(r, "Bloom4Framebuffer", IVec2(1));
	bloom4Framebuffer.add(r, WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Clamped);
	bloom4Framebuffer.checkComplete(r);

	bloom5Framebuffer.create(r, "Bloom5Framebuffer", IVec2(1));
	bloom5Framebuffer.add(r, WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Clamped);
	bloom5Framebuffer.checkComplete(r);

	bloom6Framebuffer.create(r, "Bloom6Framebuffer", IVec2(1));
	bloom6Framebuffer.add(r, WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Clamped);
	bloom6Framebuffer.checkComplete(r);

	bloom7Framebuffer.create(r, "Bloom7Framebuffer", IVec2(1));
	bloom7Framebuffer.add(r, WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Clamped);
	bloom7Framebuffer.checkComplete(r);

	bloom8Framebuffer.create(r, "Bloom8Framebuffer", IVec2(1));
	bloom8Framebuffer.add(r, WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Clamped);
	bloom8Framebuffer.checkComplete(r);

	// set near and far clip distance for player
	player.camera.setNearClipValue(0.01f);
	player.camera.setFarClipValue(1000.0f);
	for (Int i = 0; i < 100; i++) createEnemy();
}

void TripleNineRenderer::createEnemy()
{
	TripleNineEnemy enemy;
	const Index enemyIndex = static_cast<Index>(enemies.size());
	enemy.id              = enemyIndex;

	const Float gridStep = (enemySpawnRadius * 2.0f) / static_cast<Float>(enemyGridSize - 1);
	const Int   gridX    = static_cast<Int>(enemyIndex % enemyGridSize);
	const Int   gridY    = static_cast<Int>(enemyIndex / enemyGridSize);
	const Vec3  spawnPos = Vec3(-enemySpawnRadius + static_cast<Float>(gridX) * gridStep, -enemySpawnRadius + static_cast<Float>(gridY) * gridStep, 0.0f);
	enemy.setPosition(spawnPos);
	enemies.push_back(enemy);
}

void TripleNineRenderer::destroy(Window& window)
{
	client.destroy();
	bloom8Framebuffer.destroy(window.renderer);
	bloom7Framebuffer.destroy(window.renderer);
	bloom6Framebuffer.destroy(window.renderer);
	bloom5Framebuffer.destroy(window.renderer);
	bloom4Framebuffer.destroy(window.renderer);
	bloom3Framebuffer.destroy(window.renderer);
	bloom2Framebuffer.destroy(window.renderer);
	bloom1Framebuffer.destroy(window.renderer);
	GameRenderer::destroy(window);
}

void TripleNineRenderer::connect(GameSimulation& simulation) {}

void TripleNineRenderer::renderBloom(Renderer& r) const
{
	// setup rendering
	r.uniforms().reset();
	r.setDepthTest(false);
	r.setWireframeMode(false);
	r.setAlphaBlending(false);

	// prefilter (remove fireflies)
	framebuffer.bindRead(r);
	framebuffer.setAsTexture(r);
	bloom1Framebuffer.bindDraw(r);
	r.fullScreenShader("bloomPrefilter");

	bloom1Framebuffer.bindRead(r);
	bloom1Framebuffer.setAsTexture(r);
	bloom2Framebuffer.bindDraw(r);
	r.fullScreenShader("bloomDownsample");

	bloom2Framebuffer.bindRead(r);
	bloom2Framebuffer.setAsTexture(r);
	bloom3Framebuffer.bindDraw(r);
	r.fullScreenShader("bloomDownsample");

	bloom3Framebuffer.bindRead(r);
	bloom3Framebuffer.setAsTexture(r);
	bloom4Framebuffer.bindDraw(r);
	r.fullScreenShader("bloomDownsample");

	bloom4Framebuffer.bindRead(r);
	bloom4Framebuffer.setAsTexture(r);
	bloom5Framebuffer.bindDraw(r);
	r.fullScreenShader("bloomDownsample");

	bloom5Framebuffer.bindRead(r);
	bloom5Framebuffer.setAsTexture(r);
	bloom6Framebuffer.bindDraw(r);
	r.fullScreenShader("bloomDownsample");

	bloom6Framebuffer.bindRead(r);
	bloom6Framebuffer.setAsTexture(r);
	bloom7Framebuffer.bindDraw(r);
	r.fullScreenShader("bloomDownsample");

	bloom7Framebuffer.bindRead(r);
	bloom7Framebuffer.setAsTexture(r);
	bloom8Framebuffer.bindDraw(r);
	r.fullScreenShader("bloomDownsample");

	r.blendModeAdditive();

	bloom8Framebuffer.bindRead(r);
	bloom8Framebuffer.setAsTexture(r);
	bloom7Framebuffer.bindDraw(r);
	r.fullScreenShader("bloomUpsample");

	bloom7Framebuffer.bindRead(r);
	bloom7Framebuffer.setAsTexture(r);
	bloom6Framebuffer.bindDraw(r);
	r.fullScreenShader("bloomUpsample");

	bloom6Framebuffer.bindRead(r);
	bloom6Framebuffer.setAsTexture(r);
	bloom5Framebuffer.bindDraw(r);
	r.fullScreenShader("bloomUpsample");

	bloom5Framebuffer.bindRead(r);
	bloom5Framebuffer.setAsTexture(r);
	bloom4Framebuffer.bindDraw(r);
	r.fullScreenShader("bloomUpsample");

	bloom4Framebuffer.bindRead(r);
	bloom4Framebuffer.setAsTexture(r);
	bloom3Framebuffer.bindDraw(r);
	r.fullScreenShader("bloomUpsample");

	bloom3Framebuffer.bindRead(r);
	bloom3Framebuffer.setAsTexture(r);
	bloom2Framebuffer.bindDraw(r);
	r.fullScreenShader("bloomUpsample");

	bloom2Framebuffer.bindRead(r);
	bloom2Framebuffer.setAsTexture(r);
	bloom1Framebuffer.bindDraw(r);
	r.fullScreenShader("bloomUpsample");

	bloom1Framebuffer.bindRead(r);
	bloom1Framebuffer.setAsTexture(r);
	framebuffer.bindDraw(r);
	r.fullScreenShader("lastBloomUpsample");

	r.setAlphaBlending(false);
}

void TripleNineRenderer::renderScene(Window& window)
{
	Renderer& r = window.renderer;

	// resize if necessary
	if (frameSize != getSize())
	{
		frameSize = getSize();
		const auto clampSize = [](const IVec2& base, const Float scale)
		{
			const Vec2 scaled = Vec2(base) * scale;
			return IVec2(std::max(1, static_cast<Int>(scaled.x)), std::max(1, static_cast<Int>(scaled.y)));
		};

		framebuffer.resize(r, frameSize);
		bloom1Framebuffer.resize(r, clampSize(frameSize, pow(0.5f, 1.0f)));
		bloom2Framebuffer.resize(r, clampSize(frameSize, pow(0.5f, 2.0f)));
		bloom3Framebuffer.resize(r, clampSize(frameSize, pow(0.5f, 3.0f)));
		bloom4Framebuffer.resize(r, clampSize(frameSize, pow(0.5f, 4.0f)));
		bloom5Framebuffer.resize(r, clampSize(frameSize, pow(0.5f, 5.0f)));
		bloom6Framebuffer.resize(r, clampSize(frameSize, pow(0.5f, 6.0f)));
		bloom7Framebuffer.resize(r, clampSize(frameSize, pow(0.5f, 7.0f)));
		bloom8Framebuffer.resize(r, clampSize(frameSize, pow(0.5f, 8.0f)));
	}

	// clear screen
	framebuffer.clear(r);
	bloom1Framebuffer.clear(r);
	bloom2Framebuffer.clear(r);
	bloom3Framebuffer.clear(r);
	bloom4Framebuffer.clear(r);
	bloom5Framebuffer.clear(r);
	bloom6Framebuffer.clear(r);
	bloom7Framebuffer.clear(r);
	bloom8Framebuffer.clear(r);

	framebuffer.bindDraw(r);

	// default everything
	r.uniforms().reset();
	r.setCulling(false);
	r.setDepthTest(false);
	r.setAlphaBlending(false);
	// render sky
	player.camera.renderOnlyRot(r);
	r.uniforms().setSunDir(Vec4(sunDirection, 1));
	r.useTexture("tripleNineSky");
	r.useShader("tripleNineSky");
	r.renderMesh("tripleNineSky");
	r.setDepthTest(true);
	r.setCulling(true);
	// render scene
	player.render(window);
	r.fill(Vec3(1));
	r.useShader("tripleNineMap");
	// r.useTexture("tripleNineReflection");

	if (renderMap)
	{
		r.uniforms().setMMatrix(matrixFromSize(mapSize));
		r.useTexture("Map1 HDR");
		r.renderMesh("Map1");
		r.useTexture("Map2 HDR");
		r.renderMesh("Map2");
		r.useTexture("Map3 HDR");
		r.renderMesh("Map3");
		r.useTexture("Map4 HDR");
		r.renderMesh("Map4");
		r.useTexture("Map5 HDR");
		r.renderMesh("Map5");
		r.useTexture("Map6 HDR");
		r.renderMesh("Map6");
		r.useTexture("Map7 HDR");
		r.renderMesh("Map7");
		r.useTexture("Map8 HDR");
		r.renderMesh("Map8");
		r.useTexture("Map9 HDR");
		r.renderMesh("Map9");
	}

	r.useTexture("tripleNineTarget_baked");
	r.uniforms().setMMatrix(Matrix(1));
	for (const TripleNineEnemy& enemy : enemies)
	{
		r.uniforms().setMMatrix(matrixFromPositionAndSize(enemy.getPosition(), enemyRenderSize));
		r.renderMesh("tripleNineTarget");
	}
	for (const TripleNinePlayerData& remotePlayer : client.getPlayerData())
	{
		if (remotePlayer.playerID == client.getClientID()) continue;
		r.uniforms().setMMatrix(matrixFromPositionAndSize(remotePlayer.position, enemyRenderSize));
		r.renderMesh("tripleNineTarget");
	}

	// sphere
	r.useShader("color");
	r.uniforms().setCustomColor(Vec4(10000000));
	r.uniforms().setMMatrix(matrixFromPosition(Vec3(160, 50, 15 * pow(abs(sin(r.time.getTotal() * 2)), 0.5) + 1)));
	r.renderMesh("sphere");

	r.setDepthTest(false);

	// [TODO] reenable Bloom
	if (bloom) renderBloom(r);

	renderCrosshairDot(window);

	if (renderText)
	{
		// Reset text pass state explicitly so debug text is always visible.
		r.setDepthTest(false);
		r.setCulling(false);
		r.setAlphaBlending(true);
		r.screenSpaceFromBottomLeft();
		r.fill(1, 1, 1);
		Int             i       = 1;
		constexpr Float spacing = 30.0f;
		r.uniforms().setMMatrix(Matrix(1));
		r.fontSize(16.0f);
		r.textRenderSystem.setLetterSpacing(0.6f);
		r.textRenderSystem.alignText(Alignment::left, Alignment::bottom);
		r.textRenderSystem.render(r, "on Ground: " + toString(player.onGround), Vec2(spacing, static_cast<Float>(i++) * spacing));
		r.textRenderSystem.render(r, "horizontal speed: " + toString(length(player.velocity * Vec3(1, 1, 0))), Vec2(spacing, static_cast<Float>(i++) * spacing));
		r.textRenderSystem.render(r, "is moving: " + toString(player.isMoving), Vec2(spacing, static_cast<Float>(i++) * spacing));
		r.textRenderSystem.render(r, "full body lean: " + toString(player.fullBodyLeanAngle), Vec2(spacing, static_cast<Float>(i++) * spacing));
		r.textRenderSystem.render(r, "upper body lean: " + toString(player.upperBodyLeanAngle), Vec2(spacing, static_cast<Float>(i++) * spacing));
		r.textRenderSystem.render(r, "target id: " + toString(r.objectId), Vec2(spacing, static_cast<Float>(i++) * spacing));
		r.textRenderSystem.render(r, "vertical speed: " + toString(player.velocity.z), Vec2(spacing, static_cast<Float>(i++) * spacing));
		r.textRenderSystem.render(r, "double jump: " + toString(player.doubleJumpCharge), Vec2(spacing, static_cast<Float>(i++) * spacing));
		r.textRenderSystem.render(r, "max jump height: " + toString(player.debugCurrentMaxJumpHeight), Vec2(spacing, static_cast<Float>(i++) * spacing));
		r.textRenderSystem.render(r, "cameraForwardVector: " + toString(player.camera.getForwardVector()), Vec2(spacing, static_cast<Float>(i++) * spacing));
		r.textRenderSystem.render(r, "fps: " + toString(1.0f / r.time.getDelta()), Vec2(spacing, static_cast<Float>(i) * spacing));
	}

	if (vsync) limitFramerate(60);
}

void TripleNineRenderer::inputEvent(Window& window, InputEvent& input)
{
	if (input == enter)
	{
		if (!ui.capturing) ui.capture(*this);
		else
		{
			if (window.renderer.objectId != static_cast<UInt>(-1)) killEnemy(window.renderer.objectId);
		}
	}
	// if (input == exit) ui.releaseCapture();
	if (input == reloadShaders) window.renderer.shaderSystem.rebuild(window.renderer);
	if (input == toggleBloom) bloom = !bloom;
	player.inputEvent(window, input);
}

void TripleNineRenderer::renderInteractive(Window& window)
{
	Renderer& r = window.renderer;

	r.idFramebuffer.resize(r, getSize());
	// Reserve UINT_MAX as "no hit" so background/sky clicks never map to enemy 0.
	r.idFramebuffer.clearColor(r, IVec4(-1));
	r.idFramebuffer.clearDepth(r);
	r.idFramebuffer.bindDraw(r);
	r.setWireframeMode(false);
	r.setDepthTest(true);
	r.setCulling(false);
	r.setAlphaBlending(false);
	player.render(window);
	r.useShader("idShader");

	if (renderMap)
	{
		r.uniforms().setMMatrix(matrixFromSize(mapSize));
		r.renderMesh("Map1");
		r.renderMesh("Map2");
		r.renderMesh("Map3");
		r.renderMesh("Map4");
		r.renderMesh("Map5");
		r.renderMesh("Map6");
		r.renderMesh("Map7");
		r.renderMesh("Map8");
		r.renderMesh("Map9");
	}

	r.uniforms().setMMatrix(Matrix(1));
	for (const TripleNineEnemy& enemy : enemies)
	{
		r.uniforms().setMMatrix(matrixFromPositionAndSize(enemy.getPosition(), enemyRenderSize));
		r.uniforms().setObjectId(enemy.getId());
		r.renderMesh("tripleNineTarget");
	}

	r.idFramebuffer.bindRead(r);
	IVec4 idData;
	if (ui.capturing) idData = r.idFramebuffer.readPixelsAtCenterUIntRGB(r, FramebufferMode::Color0);
	else idData                  = r.idFramebuffer.readPixelsUIntRGB(r, static_cast<Int>(window.mousePosBotLeft.x), static_cast<Int>(window.mousePosBotLeft.y), FramebufferMode::Color0);
	r.objectId    = idData.x;
	r.instanceId  = idData.y;
	r.primitiveId = idData.z;
}

// Server

void TripleNineServer::destroy()
{
	if (!socket.is_open()) return;
	logDebug("Server: Shutting down...");
	disconnectAllClients();
	ErrorCode errorCode;
	socket.close(errorCode);
	if (errorCode) logWarning("Server: Failed to close socket - " + errorCode.message());
	active = false;
	logDebug("Server: Shut down complete");
}

void TripleNineServer::sendMessageToClient(const ClientID clientID, TripleNineMessage& message)
{
	if (!active) return;
	if (message.getServerEvent() == TripleNineServerToClientMessages::Unknown) logError("Message Event set up incorrectly!");

	message.timeStamp = now();
	for (TripleNineClientInfo& client : clients)
	{
		if (client.clientID == clientID)
		{
			client.lastSent                                         = now();
			Array<Char, sizeof(TripleNineMessage)> outMessageBuffer = *reinterpret_cast<Array<Char, sizeof(TripleNineMessage)>*>(&message);
			ErrorCode errorCode;
			socket.send_to(asio::buffer(outMessageBuffer), client.endpoint, 0, errorCode);
			if (errorCode) logWarning("Server: Failed to send message to client - " + errorCode.message());
			return;
		}
	}
}

void TripleNineServer::sendBackMessage(TripleNineMessage& message)
{
	if (!active) return;
	if (message.getServerEvent() == TripleNineServerToClientMessages::Unknown) logError("Message Event set up incorrectly!");

	message.timeStamp = now();
	for (TripleNineClientInfo& client : clients)
	{
		if (client.endpoint == senderEndpoint)
		{
			client.lastSent                                         = now();
			Array<Char, sizeof(TripleNineMessage)> outMessageBuffer = *reinterpret_cast<Array<Char, sizeof(TripleNineMessage)>*>(&message);
			ErrorCode errorCode;
			socket.send_to(asio::buffer(outMessageBuffer), senderEndpoint, 0, errorCode);
			if (errorCode) logWarning("Server: Failed to send message back to sender - " + errorCode.message());
			return;
		}
	}
}

void TripleNineServer::receivePlayerData(const PlayerStateData& playerStateData)
{
	TripleNineClientInfo* sourceClient = nullptr;
	for (TripleNineClientInfo& client : clients)
	{
		if (client.endpoint == senderEndpoint && client.clientID == playerStateData.clientID && client.clientToken == playerStateData.clientToken)
		{
			sourceClient = &client;
			break;
		}
	}
	if (sourceClient == nullptr)
	{
		logWarning("Server: Rejected player data from unauthenticated client");
		return;
	}

	sourceClient->position     = Vec3(playerStateData.positionX, playerStateData.positionY, playerStateData.positionZ);
	sourceClient->rotation     = Vec3(playerStateData.rotationX, playerStateData.rotationY, playerStateData.rotationZ);
	sourceClient->lastReceived = now();

	TripleNineMessage message;
	message.setServerEvent(TripleNineServerToClientMessages::BroadcastPlayerData);
	message.data.playerStateData.clientID    = sourceClient->clientID;
	message.data.playerStateData.clientToken = sourceClient->clientToken;
	message.data.playerStateData.positionX   = sourceClient->position.x;
	message.data.playerStateData.positionY   = sourceClient->position.y;
	message.data.playerStateData.positionZ   = sourceClient->position.z;
	message.data.playerStateData.rotationX   = sourceClient->rotation.x;
	message.data.playerStateData.rotationY   = sourceClient->rotation.y;
	message.data.playerStateData.rotationZ   = sourceClient->rotation.z;
	broadcastMessage(message);
}

void TripleNineServer::processEvents()
{
	if (!active) return;
	Array<Char, sizeof(TripleNineMessage)> inMessageBuffer;
	while (socket.available())
	{
		socket.receive_from(asio::buffer(inMessageBuffer, sizeof(TripleNineMessage)), senderEndpoint);
		switch (const TripleNineMessage message = *reinterpret_cast<TripleNineMessage*>(&inMessageBuffer); message.getClientEvent())
		{
		case TripleNineClientToServerMessages::WantsToConnect: clientWantsToConnect();
			break;
		case TripleNineClientToServerMessages::WantsToDisconnect: disconnectClient(message.data.disconnectClientData);
			break;
		case TripleNineClientToServerMessages::ReturnPing: receivedPing(message.timeStamp);
			break;
		case TripleNineClientToServerMessages::SendPlayerData: receivePlayerData(message.data.playerStateData);
			break;
		default: logWarning("Unknown event!");
			break;
		}
	}

	// remove dead clients
	for (Int i = 0; i < clients.size(); i++)
	{
		if (now() - clients[i].lastReceived > deadClientTimeout)
		{
			logDebug("Server: Client (" + toString(clients[i].clientToken) + ") timed out");
			clients.erase(clients.begin() + i);
			i--;
		}
	}

	//send keep-alive message
	if (now() - lastPingSent > pingInterval)
	{
		TripleNineMessage message;
		lastPingSent = now();
		message.setServerEvent(TripleNineServerToClientMessages::RequestPing);
		broadcastMessage(message);
	}
}

//void TripleNineServer::broadcastPlayerData(ClientID currentClientID, TripleNinePlayerData& data)
//{
//	for (TripleNineClientInfo& client : clients)
//	{
//		if (client.clientID == currentClientID)
//		{
//			client.playerData.position = inMessage.playerData.position;
//			client.playerData.rotation = inMessage.playerData.rotation;
//			continue;
//		}
//		outMessage.event.serverEvent = TripleNineServerToClientMessages::BroadcastPlayerData;
//		outMessage.clientToken       = client.clientToken;
//		outMessage.playerData        = inMessage.playerData;
//		sendMessage();
//	}
//}

void TripleNineServer::disconnectAllClients()
{
	TripleNineMessage message;
	logDebug("Server: Disconnecting clients...");
	for (const TripleNineClientInfo& client : clients)
	{
		logDebug("Server: Disconnecting client (" + toString(client.clientToken) + ")");
		message.setServerEvent(TripleNineServerToClientMessages::BroadcastDisconnectClient);
		message.data.disconnectClientData.clientID    = client.clientID;
		message.data.disconnectClientData.clientToken = client.clientToken;
		broadcastMessage(message);
	}
	clients.clear();
}

void TripleNineServer::broadcastMessage(TripleNineMessage& message)
{
	if (!active) return;
	if (message.getServerEvent() == TripleNineServerToClientMessages::Unknown) logError("Message Event set up incorrectly!");

	message.timeStamp = now();
	for (TripleNineClientInfo& client : clients)
	{
		client.lastSent                                         = now();
		Array<Char, sizeof(TripleNineMessage)> outMessageBuffer = *reinterpret_cast<Array<Char, sizeof(TripleNineMessage)>*>(&message);
		ErrorCode errorCode;
		socket.send_to(asio::buffer(outMessageBuffer), client.endpoint, 0, errorCode);
		if (errorCode) logWarning("Server: Failed to broadcast message - " + errorCode.message());
	}
}

void TripleNineServer::disconnectClient(const DisconnectClientData& disconnectClientData)
{
	logDebug("Server: Client (" + toString(disconnectClientData.clientID) + ") disconnecting");
	for (Int i = 0; i < clients.size(); i++)
	{
		if (clients[i].clientID == disconnectClientData.clientID && clients[i].clientToken == disconnectClientData.clientToken)
		{
			clients.erase(clients.begin() + i);
			return;
		}
	}
	logWarning("Server: Client (" + toString(disconnectClientData.clientID) + ") that was not connected wanted to disconnect with token (" + toString(disconnectClientData.clientToken) + ")!");
}

void TripleNineServer::receivedPing(const TimePoint timeStamp)
{
	const Duration ping = now() - timeStamp;

	for (TripleNineClientInfo& client : clients)
	{
		if (client.endpoint == senderEndpoint)
		{
			client.lastReceived = now();
			client.ping         = static_cast<Float>(ping.count());
		}
	}
}

void TripleNineServer::create()
{
	logDebug("Server: Starting...");
	ErrorCode errorCode;
	socket.open(asio::ip::udp::v4(), errorCode);
	if (errorCode)
	{
		logWarning("Server: Failed to open socket - " + errorCode.message());
		return;
	}
	socket.bind(UDPEndpoint(asio::ip::udp::v4(), port), errorCode);
	if (errorCode)
	{
		logWarning("Server: Host port unavailable, running as client-only instance (" + errorCode.message() + ")");
		socket.close(errorCode);
		return;
	}
	socket.non_blocking(true, errorCode);
	if (errorCode) logWarning("Server: Failed to enable non-blocking mode - " + errorCode.message());
	active = true;
	logDebug("Server: Listening on UDP port " + toString(port));
}

void TripleNineServer::clientWantsToConnect()
{
	logDebug("Server: Registering new client from (" + senderEndpoint.address().to_string() + ":" + toString(senderEndpoint.port()) + ")");

	TripleNineMessage message;

	for (TripleNineClientInfo& client : clients)
	{
		if (client.endpoint == senderEndpoint)
		{
			//send him his already existing id
			message.setServerEvent(TripleNineServerToClientMessages::ConfirmConnection);
			message.data.connectionConfirmedData.clientID  = client.clientID;
			message.data.connectionConfirmedData.authToken = client.clientToken;
			logDebug("Server: Client (" + toString(client.clientID) + ") already exists, sending again");
			sendBackMessage(message);
			return;
		}
	}

	const Token newClientToken = generateRandomToken();
	ClientID    newClientID    = 0;
	Bool        duplicate      = false;
	do
	{
		newClientID++;
		duplicate = false;
		for (const TripleNineClientInfo& client : clients)
		{
			if (client.clientID == newClientID) duplicate = true;
		}
	}
	while (duplicate);

	// store info
	TripleNineClientInfo info;
	info.clientID    = newClientID;
	info.clientToken = newClientToken;
	info.endpoint    = senderEndpoint;
	clients.push_back(info);

	// give client its new id
	message.setServerEvent(TripleNineServerToClientMessages::ConfirmConnection);
	message.data.connectionConfirmedData.clientID  = newClientID;
	message.data.connectionConfirmedData.authToken = newClientToken;
	logDebug("Server: New client registered with id (" + toString(newClientID) + ") and token (" + toString(newClientToken) + ")");
	sendBackMessage(message);
}

void TripleNineServer::getServerInfo() const
{
	//[TODO] send back info
	// players
	// ping
	// host name
}

// Client

void TripleNineClient::disconnectFromServer()
{
	if (!connected || !socket.is_open()) return;
	logDebug("Client (" + toString(clientID) + "): Disconnecting...");
	TripleNineMessage message;
	message.setClientEvent(TripleNineClientToServerMessages::WantsToDisconnect);
	message.data.disconnectClientData.clientToken = authToken;
	message.data.disconnectClientData.clientID    = clientID;
	sendMessage(message);
	connected = false;
}

void TripleNineClient::create()
{
	if (socket.is_open()) return;
	ErrorCode errorCode;
	const auto endpoints = UDPResolver(context).resolve(asio::ip::udp::v4(), host, toString(port), errorCode);
	if (errorCode)
	{
		logWarning("Client (?): Failed to resolve server endpoint - " + errorCode.message());
		return;
	}
	if (endpoints.begin() == endpoints.end())
	{
		logWarning("Client (?): Server endpoint resolve returned no addresses");
		return;
	}
	serverEndpoint = *endpoints.begin();
	logDebug(serverEndpoint.address().to_string() + ":" + toString(serverEndpoint.port()));
	socket.open(asio::ip::udp::v4(), errorCode);
	if (errorCode)
	{
		logWarning("Client (?): Failed to open socket - " + errorCode.message());
		return;
	}
	socket.non_blocking(true, errorCode);
	if (errorCode) logWarning("Client (?): Failed to enable non-blocking mode - " + errorCode.message());
	lastReceived = now();
}

void TripleNineClient::destroy()
{
	disconnectFromServer();
	ErrorCode errorCode;
	socket.close(errorCode);
	if (errorCode) logWarning("Client (" + toString(clientID) + "): Failed to close socket - " + errorCode.message());
	playerData.clear();
}

void TripleNineClient::connectToServer()
{
	if (!socket.is_open()) return;
	logDebug("Client (?): Client trying to connect...");
	TripleNineMessage message;
	message.setClientEvent(TripleNineClientToServerMessages::WantsToConnect);
	sendMessage(message);
}

void TripleNineClient::connectionDeclined()
{
	logDebug("Client (" + toString(clientID) + "): Server refused connection");
	connected = false;
}

//void TripleNineClient::playerDataReceived(PlayerID playerID, const TripleNinePlayerData& playerData)
//{
//	// update player data
//	for (TripleNineClientInfo& client : clients)
//	{
//		if (client.clientToken == clientToken)
//		{
//			client.playerData = playerData;
//			return;
//		}
//	}
//
//	// must be new client
//	TripleNineClientInfo newClient;
//	newClient.clientToken = inMessage.clientToken;
//	newClient.playerData  = inMessage.playerData;
//	clients.push_back(newClient);
//}

void TripleNineClient::disconnect(const DisconnectClientData& disconnectClientData)
{
	if (this->clientID == disconnectClientData.clientID)
	{
		connected = false;
		logDebug("Client (" + toString(clientID) + "): Server requested disconnect!");
	}
	else disconnectPlayer(disconnectClientData.clientID);
}

void TripleNineClient::receiveServerInfo(const ServerInfoData& serverInfoData)
{
	//[TODO] receive server info
}

void TripleNineClient::requestServerInfo()
{
	//[TODO] request server info event
}

void TripleNineClient::disconnectPlayer(const PlayerID playerID)
{
	for (Int i = 0; i < playerData.size(); i++)
	{
		if (playerData[i].playerID == playerID)
		{
			playerData.erase(playerData.begin() + i);
			logDebug("Client (" + toString(clientID) + "): Player (" + toString(playerID) + ") disconnected");
			return;
		}
	}
}

void TripleNineClient::playerDataReceived(const PlayerStateData& playerStateData)
{
	if (playerStateData.clientID == clientID) return;
	for (TripleNinePlayerData& data : playerData)
	{
		if (data.playerID == playerStateData.clientID)
		{
			data.position = Vec3(playerStateData.positionX, playerStateData.positionY, playerStateData.positionZ);
			data.rotation = Vec3(playerStateData.rotationX, playerStateData.rotationY, playerStateData.rotationZ);
			return;
		}
	}
	TripleNinePlayerData newData;
	newData.playerID = playerStateData.clientID;
	newData.position = Vec3(playerStateData.positionX, playerStateData.positionY, playerStateData.positionZ);
	newData.rotation = Vec3(playerStateData.rotationX, playerStateData.rotationY, playerStateData.rotationZ);
	playerData.push_back(newData);
}

void TripleNineClient::connectionConfirmed(const ConnectionConfirmedData& connectionConfirmedData)
{
	logDebug("Client (?): Server confirmed connection");
	logDebug("Client (?): Server requested ID change");
	clientID  = connectionConfirmedData.clientID;
	authToken = connectionConfirmedData.authToken;

	logDebug("Client (?): Client was assigned ID:" + toString(clientID));
	logDebug("Client (" + toString(clientID) + "): Client was assigned token:" + toString(authToken));
	if (!connected)
	{
		logDebug("Client (" + toString(clientID) + "): Connected...");
		connected = true;
	}
}

//Token TripleNineClient::getClientToken() const
//{
//	return authToken;
//}

void TripleNineClient::processEvents()
{
	create();
	if (!connected && now() - lastSent > Milliseconds(1000)) connectToServer();
	if (!socket.is_open()) return;

	while (socket.available())
	{
		Array<Char, sizeof(TripleNineMessage)> inMessageBuffer;

		socket.receive_from(asio::buffer(inMessageBuffer, sizeof(TripleNineMessage)), serverEndpoint);
		TripleNineMessage message = *reinterpret_cast<TripleNineMessage*>(&inMessageBuffer);

		switch (message.getServerEvent())
		{
		case TripleNineServerToClientMessages::DeclineConnection: connectionDeclined();
			break;
		case TripleNineServerToClientMessages::ConfirmConnection: connectionConfirmed(message.data.connectionConfirmedData);
			break;
		case TripleNineServerToClientMessages::BroadcastPlayerData: playerDataReceived(message.data.playerStateData);
			break;
		case TripleNineServerToClientMessages::BroadcastDisconnectClient: disconnect(message.data.disconnectClientData);
			break;
		case TripleNineServerToClientMessages::RequestPing: respondToPing();
			break;
		default: logWarning("Unknown event!");
			break;
		}
	}
}

void TripleNineClient::sendMessage(TripleNineMessage& message)
{
	if (!socket.is_open()) return;
	if (message.getClientEvent() == TripleNineClientToServerMessages::Unknown) logError("Message Event set up incorrectly!");

	lastSent                                                = now();
	message.timeStamp                                       = lastSent;
	Array<Char, sizeof(TripleNineMessage)> outMessageBuffer = *reinterpret_cast<Array<Char, sizeof(TripleNineMessage)>*>(&message);
	try
	{
		socket.send_to(asio::buffer(outMessageBuffer), serverEndpoint);
	}
	catch (std::exception& e)
	{
		logError(e.what());
	}
}

void TripleNineClient::sendPlayerData(const Vec3& position, const Vec3& rotation)
{
	if (!connected) return;
	TripleNineMessage message;
	message.setClientEvent(TripleNineClientToServerMessages::SendPlayerData);
	message.data.playerStateData.clientID    = clientID;
	message.data.playerStateData.clientToken = authToken;
	message.data.playerStateData.positionX   = position.x;
	message.data.playerStateData.positionY   = position.y;
	message.data.playerStateData.positionZ   = position.z;
	message.data.playerStateData.rotationX   = rotation.x;
	message.data.playerStateData.rotationY   = rotation.y;
	message.data.playerStateData.rotationZ   = rotation.z;
	sendMessage(message);
}

void TripleNineClient::respondToPing()
{
	logDebug("Client (" + toString(clientID) + "): I am alive");
	TripleNineMessage message;
	message.setClientEvent(TripleNineClientToServerMessages::ReturnPing);
	message.data.pingEventData.clientID = clientID;
	sendMessage(message);
}

void TripleNineClient::waitTillConnected() const
{
	while (!connected) {}
}

ClientID TripleNineClient::getClientID() const
{
	return clientID;
}

const Vector<TripleNinePlayerData>& TripleNineClient::getPlayerData() const
{
	return playerData;
}

//void TripleNineClient::sendPlayerData(const PlayerID playerID, const Token playerToken, const Vec3 position, const Vec3 rotation, const ULL gameTick)
//{
//	TripleNineMessage message;
//	message.timeStamp           = now();
//	message.clientToken         = authToken;
//	message.playerData.playerID = playerID;
//	message.playerData.position = position;
//	message.playerData.rotation = rotation;
//	message.tick                = gameTick;
//	message.event.clientEvent   = TripleNineClientToServerMessages::SendPlayerData;
//	sendMessage(message);
//}

// Game

void TripleNine::run()
{
	simulation.start();

	if (useIntro)
	{
		intro.init(renderer1);
		ui.window("Triple Nine", settings.defaultWindowWidth, settings.defaultWindowHeight, true, false, WindowState::Windowed, intro);
	}
	else
	{
		ui.window("Triple Nine", settings.defaultWindowWidth, settings.defaultWindowHeight, true, true, WindowState::Windowed, renderer1);

		if (useSecondWindow)
		{
			ui.window("Triple Nine", settings.defaultWindowWidth, settings.defaultWindowHeight, true, true, WindowState::Windowed, renderer2);
		}
	}

	ui.run();
	simulation.stop();
}
