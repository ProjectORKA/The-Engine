#include "TripleNine.hpp"
#include "JobSystem.hpp"
#include "Window.hpp"

#ifdef TRIPLE_NINE_ENEMY
void TripleNineEnemy::update(Window& window) {}

void TripleNineEnemy::destroy(Window& window) {}

const Vector<TripleNineEnemy>& TripleNineGameState::getEnemies()
{
	return enemies;
}

void TripleNineSimulation::createEnemy()
{
	TripleNineEnemy& newEnemy =gameState.createEnemy();
}

TripleNineEnemy& TripleNineGameState::createEnemy()
{
	enemies.emplace_back();
	enemies.back().id = static_cast<Index>(enemies.size()) - static_cast<Index>(1);
	return enemies.back();
}

void TripleNineEnemy::inputEvent(Window& window, InputEvent input) {}

void TripleNineEnemy::create(Window& window) {}

void TripleNineEnemy::setPosition(const Vec3 position) {
	this->position = position;
}
#endif

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
		if (player.playerID == playerID)
		{
			return player.position;
		}
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

	#ifdef TRIPLE_NINE_ENEMY
	for (Int i = 0; i < 100; i++) createEnemy();
	#endif
}

void TripleNineSimulation::destroy()
{
	server.destroy();
}

void TripleNineSimulation::update(Float delta)
{
	if (isOnline)
	{
		if (isServer)
		{
			server.processEvents();
		}
		else
		{
			client.processEvents();
		}
	}
	else {}
}

void TripleNineSimulation::updatePlayer(const Byte playerId, const Vec3 position)
{
	gameState.updatePlayer(playerId, position);
}

// Renderer

void TripleNineRenderer::update(Window& window)
{
	client.processEvents();
	player.update(window);
	//if (gameState.getPlayerPosition(client.getClientToken()) != player.position)
	//{
	//	client.sendPlayerData(player.playerID, player.authToken, player.position, player.camera.getRotation(), gameState.getTick());
	//}
	//gameState.updatePlayer(client.getClientToken(), player.position);
}

void TripleNineRenderer::create(Window& window)
{
	client.create();

	Renderer& r = window.renderer;

	mainFramebuffer.create("MainFramebuffer", Area(1920, 1080));
	mainFramebuffer.add(r, WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Clamped);
	mainFramebuffer.add(r, WritePixelsFormat::Depth, DataType::Float, FramebufferAttachment::Depth, false, Wrapping::Clamped);
	mainFramebuffer.checkComplete();

	bloom1Framebuffer.create("Bloom1Framebuffer", Area(1920, 1080));
	bloom1Framebuffer.add(r, WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Clamped);
	bloom1Framebuffer.checkComplete();

	bloom2Framebuffer.create("Bloom2Framebuffer", Area(1920, 1080));
	bloom2Framebuffer.add(r, WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Clamped);
	bloom2Framebuffer.checkComplete();

	bloom3Framebuffer.create("Bloom3Framebuffer", Area(1920, 1080));
	bloom3Framebuffer.add(r, WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Clamped);
	bloom3Framebuffer.checkComplete();

	bloom4Framebuffer.create("Bloom4Framebuffer", Area(1920, 1080));
	bloom4Framebuffer.add(r, WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Clamped);
	bloom4Framebuffer.checkComplete();

	bloom5Framebuffer.create("Bloom5Framebuffer", Area(1920, 1080));
	bloom5Framebuffer.add(r, WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Clamped);
	bloom5Framebuffer.checkComplete();

	bloom6Framebuffer.create("Bloom6Framebuffer", Area(1920, 1080));
	bloom6Framebuffer.add(r, WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Clamped);
	bloom6Framebuffer.checkComplete();

	bloom7Framebuffer.create("Bloom7Framebuffer", Area(1920, 1080));
	bloom7Framebuffer.add(r, WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Clamped);
	bloom7Framebuffer.checkComplete();

	bloom8Framebuffer.create("Bloom8Framebuffer", Area(1920, 1080));
	bloom8Framebuffer.add(r, WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Clamped);
	bloom8Framebuffer.checkComplete();

	// set near and far clip distance for player
	player.camera.setNearClipValue(0.01f);
	player.camera.setFarClipValue(1000.0f);
}

void TripleNineRenderer::destroy(Window& window)
{
	bloom8Framebuffer.destroy(window.renderer);
	bloom7Framebuffer.destroy(window.renderer);
	bloom6Framebuffer.destroy(window.renderer);
	bloom5Framebuffer.destroy(window.renderer);
	bloom4Framebuffer.destroy(window.renderer);
	bloom3Framebuffer.destroy(window.renderer);
	bloom2Framebuffer.destroy(window.renderer);
	bloom1Framebuffer.destroy(window.renderer);
	mainFramebuffer.destroy(window.renderer);

	client.destroy();
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
	mainFramebuffer.bindRead();
	mainFramebuffer.setAsTexture();
	bloom1Framebuffer.bindDraw(r);
	r.fullScreenShader("bloomPrefilter");

	bloom1Framebuffer.bindRead();
	bloom1Framebuffer.setAsTexture();
	bloom2Framebuffer.bindDraw(r);
	r.fullScreenShader("bloomDownsample");

	bloom2Framebuffer.bindRead();
	bloom2Framebuffer.setAsTexture();
	bloom3Framebuffer.bindDraw(r);
	r.fullScreenShader("bloomDownsample");

	bloom3Framebuffer.bindRead();
	bloom3Framebuffer.setAsTexture();
	bloom4Framebuffer.bindDraw(r);
	r.fullScreenShader("bloomDownsample");

	bloom4Framebuffer.bindRead();
	bloom4Framebuffer.setAsTexture();
	bloom5Framebuffer.bindDraw(r);
	r.fullScreenShader("bloomDownsample");

	bloom5Framebuffer.bindRead();
	bloom5Framebuffer.setAsTexture();
	bloom6Framebuffer.bindDraw(r);
	r.fullScreenShader("bloomDownsample");

	bloom6Framebuffer.bindRead();
	bloom6Framebuffer.setAsTexture();
	bloom7Framebuffer.bindDraw(r);
	r.fullScreenShader("bloomDownsample");

	bloom7Framebuffer.bindRead();
	bloom7Framebuffer.setAsTexture();
	bloom8Framebuffer.bindDraw(r);
	r.fullScreenShader("bloomDownsample");

	r.blendModeAdditive();

	bloom8Framebuffer.bindRead();
	bloom8Framebuffer.setAsTexture();
	bloom7Framebuffer.bindDraw(r);
	r.fullScreenShader("bloomUpsample");

	bloom7Framebuffer.bindRead();
	bloom7Framebuffer.setAsTexture();
	bloom6Framebuffer.bindDraw(r);
	r.fullScreenShader("bloomUpsample");

	bloom6Framebuffer.bindRead();
	bloom6Framebuffer.setAsTexture();
	bloom5Framebuffer.bindDraw(r);
	r.fullScreenShader("bloomUpsample");

	bloom5Framebuffer.bindRead();
	bloom5Framebuffer.setAsTexture();
	bloom4Framebuffer.bindDraw(r);
	r.fullScreenShader("bloomUpsample");

	bloom4Framebuffer.bindRead();
	bloom4Framebuffer.setAsTexture();
	bloom3Framebuffer.bindDraw(r);
	r.fullScreenShader("bloomUpsample");

	bloom3Framebuffer.bindRead();
	bloom3Framebuffer.setAsTexture();
	bloom2Framebuffer.bindDraw(r);
	r.fullScreenShader("bloomUpsample");

	bloom2Framebuffer.bindRead();
	bloom2Framebuffer.setAsTexture();
	bloom1Framebuffer.bindDraw(r);
	r.fullScreenShader("bloomUpsample");

	bloom1Framebuffer.bindRead();
	bloom1Framebuffer.setAsTexture();
	mainFramebuffer.bindDraw(r);
	r.fullScreenShader("lastBloomUpsample");

	r.setAlphaBlending(false);
}

void TripleNineRenderer::render(Window& window, const TiledRectangle area)
{
	Renderer& r = window.renderer;

	// resize if necessary
	if (frameSize != area.size)
	{
		frameSize = area.size;
		mainFramebuffer.resize(r, frameSize);
		bloom1Framebuffer.resize(r, frameSize * pow(0.5f, 1.0f));
		bloom2Framebuffer.resize(r, frameSize * pow(0.5f, 2.0f));
		bloom3Framebuffer.resize(r, frameSize * pow(0.5f, 3.0f));
		bloom4Framebuffer.resize(r, frameSize * pow(0.5f, 4.0f));
		bloom5Framebuffer.resize(r, frameSize * pow(0.5f, 5.0f));
		bloom6Framebuffer.resize(r, frameSize * pow(0.5f, 6.0f));
		bloom7Framebuffer.resize(r, frameSize * pow(0.5f, 7.0f));
		bloom8Framebuffer.resize(r, frameSize * pow(0.5f, 8.0f));
	}

	// clear screen
	mainFramebuffer.clear();
	bloom1Framebuffer.clear();
	bloom2Framebuffer.clear();
	bloom3Framebuffer.clear();
	bloom4Framebuffer.clear();
	bloom5Framebuffer.clear();
	bloom6Framebuffer.clear();
	bloom7Framebuffer.clear();
	bloom8Framebuffer.clear();

	mainFramebuffer.bindDraw(r);

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

	#ifdef TRIPLE_NINE_ENEMY
	r.useTexture("tripleNineTarget_baked");
	Vector<Matrix> enemies;
	for (const TripleNineEnemy& enemy : gameState.enemies)
	{
		enemies.push_back(matrixFromPosition(enemy.position));
	}
	r.renderMeshInstanced("tripleNineTarget", enemies);
	#endif

	// sphere
	r.useShader("color");
	r.uniforms().setCustomColor(Vec4(10000000));
	r.uniforms().setMMatrix(matrixFromPosition(Vec3(160, 50, 15 * pow(abs(sin(r.time.getTotal() * 2)), 0.5) + 1)));
	r.renderMesh("sphere");

	r.setDepthTest(false);

	// [TODO] reenable Bloom
	if (bloom) renderBloom(r);

	// crosshair
	r.uniforms().reset();
	r.screenSpace();
	r.fill(0.1f, 0.1f, 0.1f);
	r.useShader("color");
	r.uniforms().setMMatrix(matrixFromPositionAndSize(Vec2(window.getFrameSize() / 2), 3));
	r.renderMesh("circle");
	r.fill(1, 1, 1);
	r.uniforms().setMMatrix(matrixFromPositionAndSize(Vec2(window.getFrameSize() / 2), 2));
	r.renderMesh("circle");

	if (renderText)
	{
		r.screenSpace();
		Int             i       = 1;
		constexpr Float spacing = 30.0f;
		r.uniforms().setMMatrix(Matrix(1));
		r.textRenderSystem.setSize(16.0f);
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

	// render to window
	r.setDepthTest(false);
	mainFramebuffer.setAsTexture(0);
	r.drawToWindow();
	r.fullScreenShader("tonemapping");

	if (vsync) limitFramerate(60);
}

void TripleNineRenderer::inputEvent(Window& window, const InputEvent input)
{
	if (input == enter)
	{
		if (!window.capturing)
		{
			window.captureCursor();
		}
		else
		{
			#ifdef TRIPLE_NINE_ENEMY
			//if (window.renderer.objectId != static_cast<UInt>(-1)) killEnemy(window.renderer.objectId);
			#endif
		}
	}
	// if (input == exit) window.releaseCursor();
	if (input == reloadShaders) window.renderer.shaderSystem.rebuild();
	if (input == toggleBloom) bloom = !bloom;
	player.inputEvent(window, input);
}

void TripleNineRenderer::renderInteractive(Window& window, const TiledRectangle area)
{
	Renderer& r = window.renderer;

	r.idFramebuffer.resize(r, area.size);
	r.idFramebuffer.clearColor(IVec4(0));
	r.idFramebuffer.clearDepth();
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

	#ifdef TRIPLE_NINE_ENEMY
	r.uniforms().setMMatrix(Matrix(1));
	for (TripleNineEnemy& enemy : gameState.getEnemies())
	{
		r.uniforms().setMMatrix(matrixFromPositionAndSize(enemy.getPosition(), 1));
		r.uniforms().setObjectId(enemy.getId());
		r.renderMesh("tripleNineTarget");

		enemy.renderInteractive(window, area);
	}
	#endif

	r.idFramebuffer.bindRead();
	IVec4 idData;
	if (window.capturing)
	{
		idData = r.idFramebuffer.readPixelsAtCenterUIntRGB(FramebufferMode::Color0);
	}
	else
	{
		idData = r.idFramebuffer.readPixelsUIntRGB(static_cast<Int>(window.mousePosBotLeft.x), static_cast<Int>(window.mousePosBotLeft.y), FramebufferMode::Color0);
	}
	r.objectId    = idData.x;
	r.instanceId  = idData.y;
	r.primitiveId = idData.z;

	// render to window
	r.setDepthTest(false);
	r.idFramebuffer.bindRead();
	r.idFramebuffer.setAsTexture(0);
	r.drawToWindow();
	r.fullScreenShader("debugID");
}

// Server

void TripleNineServer::destroy()
{
	logDebug("Server: Shutting down...");
	disconnectAllClients();
	logDebug("Server: Shut down complete");
}

void TripleNineServer::sendMessageToClient(const ClientID clientID, TripleNineMessage& message)
{
	if (message.getServerEvent() == TripleNineServerToClientMessages::Unknown) logError("Message Event set up incorrectly!");

	message.timeStamp = now();
	for (TripleNineClientInfo& client : clients)
	{
		if (client.clientID == clientID)
		{
			client.lastSent                                         = now();
			Array<Char, sizeof(TripleNineMessage)> outMessageBuffer = *reinterpret_cast<Array<Char, sizeof(TripleNineMessage)>*>(&message);
			socket.send_to(asio::buffer(outMessageBuffer), client.endpoint);
			return;
		}
	}
}

void TripleNineServer::sendBackMessage(TripleNineMessage& message)
{
	if (message.getServerEvent() == TripleNineServerToClientMessages::Unknown) logError("Message Event set up incorrectly!");

	message.timeStamp = now();
	for (TripleNineClientInfo& client : clients)
	{
		if (client.endpoint == senderEndpoint)
		{
			client.lastSent                                         = now();
			Array<Char, sizeof(TripleNineMessage)> outMessageBuffer = *reinterpret_cast<Array<Char, sizeof(TripleNineMessage)>*>(&message);
			socket.send_to(asio::buffer(outMessageBuffer), senderEndpoint);
			return;
		}
	}
}

void TripleNineServer::processEvents()
{
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
		message.data.disconnectClientData.clientID = client.clientToken;
		broadcastMessage(message);
	}
	clients.clear();
}

void TripleNineServer::broadcastMessage(TripleNineMessage& message)
{
	if (message.getServerEvent() == TripleNineServerToClientMessages::Unknown) logError("Message Event set up incorrectly!");

	message.timeStamp = now();
	for (TripleNineClientInfo& client : clients)
	{
		client.lastSent                                         = now();
		Array<Char, sizeof(TripleNineMessage)> outMessageBuffer = *reinterpret_cast<Array<Char, sizeof(TripleNineMessage)>*>(&message);
		socket.send_to(asio::buffer(outMessageBuffer), client.endpoint);
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

void TripleNineServer::create() const
{
	logDebug("Server: Starting...");
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
			break;
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

void TripleNineServer::getServerInfo()
{
	//[TODO] send back info
	// players
	// ping
	// host name
}

// Client

void TripleNineClient::disconnectFromServer()
{
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
	serverEndpoint = *UDPResolver(context).resolve(asio::ip::udp::v4(), host, toString(port)).begin();
	logDebug(serverEndpoint.address().to_string() + ":" + toString(serverEndpoint.port()));
	socket.open(asio::ip::udp::v4());
	lastReceived = now();
}

void TripleNineClient::destroy()
{
	disconnectFromServer();
}

void TripleNineClient::connectToServer()
{
	logDebug("Client (?): Client trying to connect...");
	TripleNineMessage message;
	message.setClientEvent(TripleNineClientToServerMessages::WantsToConnect);
	sendMessage(message);
	Sleep(1000);
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
	if (!connected) connectToServer();

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
		//case TripleNineServerToClientMessages::BroadcastPlayerData: playerDataReceived();
		//	break;
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
	if (message.getClientEvent() == TripleNineClientToServerMessages::Unknown) logError("Message Event set up incorrectly!");

	lastSent                                                = now();
	message.timeStamp                                       = lastSent;
	Array<Char, sizeof(TripleNineMessage)> outMessageBuffer = *reinterpret_cast<Array<Char, sizeof(TripleNineMessage)>*>(&message);
	socket.send_to(asio::buffer(outMessageBuffer), serverEndpoint);
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
		ui.window("Triple Nine", Area(1920, 1080), true, false, WindowState::Windowed, intro);
	}
	else
	{
		ui.window("Triple Nine", Area(1920, 1080), true, true, WindowState::Windowed, renderer1);

		Sleep(3000);

		ui.window("Triple Nine", Area(1920, 1080), true, true, WindowState::Windowed, renderer2);
	}

	ui.run();
	simulation.stop();
}
