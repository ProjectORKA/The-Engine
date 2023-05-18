#include "Organizer.hpp"

void OrganizerRenderer::addCompany(const Index guid, const String& companyName, const String& domain) {
	Company company;
	company.companyGuid = guid;
	company.companyName = companyName;
	company.domain = domain;
	companies.push_back(company);
}

void OrganizerRenderer::addUser(const Index userGuid, const Index companyGuid, const String& firstName,
                                const String& lastName) {
	User user;
	user.companyGuid = companyGuid;
	user.userGuid = userGuid;
	user.firstName = firstName;
	user.lastName = lastName;
	user.email = firstName + "." + lastName + "@project-orka.com";
	user.position = randomVec2(-100, 100);
	users.push_back(user);
}

void OrganizerRenderer::addTask(const Task& task) { tasks.push_back(task); }

void OrganizerRenderer::update(Window& window) { player.update(window); }

void OrganizerRenderer::inputEvent(Window& window, const InputEvent input) {
	if (input == enter) window.captureCursor();
	if (input == exit) window.uncaptureCursor();
	if (input == wireFrameToggle) window.renderer.wireframeMode = !window.renderer.wireframeMode;

	player.inputEvent(window, input);
}

void OrganizerRenderer::create(ResourceManager& resourceManager, Window& window) {
	//setup renderer
	player.camera.location = Vec3(0, 0, 10);

	Index userGUID = 0;

	//load database
	addCompany(0, "Project ORKA", "project-orka.com");
	addUser(userGUID++, 0, "Firstname1", "Lastname1");
	addUser(userGUID++, 0, "Firstname2", "Lastname2");
	addUser(userGUID++, 0, "Firstname3", "Lastname3");
	addUser(userGUID++, 0, "Firstname4", "Lastname4");
	addUser(userGUID++, 0, "Firstname5", "Lastname5");
	addUser(userGUID++, 0, "Firstname6", "Lastname6");
	addUser(userGUID++, 0, "Firstname7", "Lastname7");
	addUser(userGUID++, 0, "Firstname8", "Lastname8");
	addUser(userGUID++, 0, "Firstname9", "Lastname9");
	addUser(userGUID++, 0, "Firstname11", "Lastname10");
	addUser(userGUID++, 0, "Firstname11", "Lastname11");
	addUser(userGUID++, 0, "Firstname12", "Lastname12");
	addUser(userGUID++, 0, "Firstname13", "Lastname13");
	addUser(userGUID++, 0, "Firstname14", "Lastname14");
	addUser(userGUID++, 0, "Firstname15", "Lastname15");
	addUser(userGUID++, 0, "Firstname16", "Lastname16");
	addUser(userGUID++, 0, "Firstname17", "Lastname17");
	addUser(userGUID++, 0, "Firstname18", "Lastname18");
	addUser(userGUID++, 0, "Firstname19", "Lastname19");
	addUser(userGUID++, 0, "Firstname21", "Lastname20");
	addUser(userGUID++, 0, "Firstname21", "Lastname21");
	addUser(userGUID++, 0, "Firstname22", "Lastname22");
	addUser(userGUID++, 0, "Firstname23", "Lastname23");
	addUser(userGUID++, 0, "Firstname24", "Lastname24");
	addUser(userGUID++, 0, "Firstname25", "Lastname25");
	addUser(userGUID++, 0, "Firstname26", "Lastname26");
	addUser(userGUID++, 0, "Firstname27", "Lastname27");
	addUser(userGUID++, 0, "Firstname28", "Lastname28");
	addUser(userGUID++, 0, "Firstname29", "Lastname29");

	for (User& user1 : users) {
		for (User& user2 : users) {
			Line3D c;
			c.start = Vec3(user1.position, 0);
			c.end = Vec3(user2.position, 0);
			connections.push_back(c);
			logDebug(c.start);
			logDebug(c.end);
		}
	}
}

void OrganizerRenderer::renderConnections(ResourceManager& resourceManager, Renderer& r) const {
	r.useShader(resourceManager, "color");
	r.fill(Color(0.5, 0.5, 0.5, 1));
	//for (const OrganizerConnection& c : connections) {
	//	r.line(c.start, c.end, 0.05f);
	//}

	r.lines(connections);
}

void OrganizerRenderer::render(ResourceManager& resourceManager, Window& window, TiledRectangle area) {
	Renderer& r = window.renderer;

	r.draw("main");
	r.clearColor(Color(1.0));
	r.setWireframeMode();
	r.setCulling(true);
	r.setDepthTest(true);
	r.clearDepth();

	//render scene
	player.render(resourceManager, window); // sets the location, rotation and projection

	for (const User& user : users) {
		r.useShader(resourceManager, "normals"); //sets the color / material for the rendered objects
		r.uniforms().mMatrix(matrixFromLocation(user.position));
		r.renderMesh(resourceManager, "organizer user");
	}

	//render connections
	renderConnections(resourceManager, r);

	//render labels
	const Matrix vMatrix = r.uniforms().vMatrix();
	const Matrix pMatrix = r.uniforms().pMatrix();
	r.setDepthTest(false); //disables depth to always draw on top
	r.screenSpace(); // aligns coordinate system with screen space
	r.textRenderSystem.alignText(Alignment::center, Alignment::top);

	for (User& user : users) {
		r.fill(Color(0, 0, 0, 1));

		//calculate screen space position of 3d label text
		Vec4 pos = inverse(r.getScreenSpaceMatrix()) * pMatrix * vMatrix * Vec4(user.position, 0, 1);

		if (pos.z < 0) pos = Vec4(0);
		pos /= pos.z;
		pos.y -= 10;
		r.uniforms().mMatrix(matrixFromLocation(Vec3(pos)));
		r.textRenderSystem.render(resourceManager, r, String(user.firstName).append(" ").append(user.lastName));
	}

	//text rendering
	r.uniforms().mMatrix(Matrix(1));
	r.setDepthTest(false); //disables depth to always draw on top
	r.screenSpace(); // aligns coordinate system with screen space
	r.textRenderSystem.alignText(Alignment::end, Alignment::end);
	r.textRenderSystem.setStyle(fonts.paragraph);
	r.textRenderSystem.render(resourceManager, r, String("W A S D Q E to move"), Vec2(50, 250));
	r.textRenderSystem.render(resourceManager, r, String("F for wire frame mode"), Vec2(50, 200));
	r.textRenderSystem.render(resourceManager, r, String("Scroll to change speed"), Vec2(50, 150));
	r.textRenderSystem.render(resourceManager, r,
	                          String("FPS: ").append(toString(static_cast<Int>(1.0f / r.time.delta))), Vec2(50));
	// renders current frame rate to the screen
}

void Organizer::run() {
	resourceManager.create();

	ui.create();

	ui.window("Organizer", Area(1920, 1080), true, WindowState::windowed, renderer, resourceManager);

	ui.run();
}
