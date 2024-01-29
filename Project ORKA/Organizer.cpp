#include "Organizer.hpp"
#include "Random.hpp"

void OrganizerRenderer::destroy(Window& window) {}

void OrganizerRenderer::connect(GameSimulation& simulation) {}

void OrganizerRenderer::update(Window& window)
{
	player.update(window);
}

void OrganizerRenderer::addTask(const Task& task)
{
	tasks.push_back(task);
}

void OrganizerRenderer::inputEvent(Window& window, const InputEvent input)
{
	if(input == enter) window.captureCursor();
	if(input == exit) window.releaseCursor();
	if(input == wireFrameToggle) window.renderer.wireframeMode = !window.renderer.wireframeMode;

	player.inputEvent(window, input);
}

void OrganizerRenderer::create(Window& window)
{
	// setup renderer
	player.camera.setLocation(Vec3(0, 0, 10));

	Index userGUId = 0;

	// load database
	addCompany(0, "Project ORKA", "project-orka.com");
	addUser(userGUId++, 0, "Firstname1", "Lastname1");
	addUser(userGUId++, 0, "Firstname2", "Lastname2");
	addUser(userGUId++, 0, "Firstname3", "Lastname3");
	addUser(userGUId++, 0, "Firstname4", "Lastname4");
	addUser(userGUId++, 0, "Firstname5", "Lastname5");
	addUser(userGUId++, 0, "Firstname6", "Lastname6");
	addUser(userGUId++, 0, "Firstname7", "Lastname7");
	addUser(userGUId++, 0, "Firstname8", "Lastname8");
	addUser(userGUId++, 0, "Firstname9", "Lastname9");
	addUser(userGUId++, 0, "Firstname11", "Lastname10");
	addUser(userGUId++, 0, "Firstname11", "Lastname11");
	addUser(userGUId++, 0, "Firstname12", "Lastname12");
	addUser(userGUId++, 0, "Firstname13", "Lastname13");
	addUser(userGUId++, 0, "Firstname14", "Lastname14");
	addUser(userGUId++, 0, "Firstname15", "Lastname15");
	addUser(userGUId++, 0, "Firstname16", "Lastname16");
	addUser(userGUId++, 0, "Firstname17", "Lastname17");
	addUser(userGUId++, 0, "Firstname18", "Lastname18");
	addUser(userGUId++, 0, "Firstname19", "Lastname19");
	addUser(userGUId++, 0, "Firstname21", "Lastname20");
	addUser(userGUId++, 0, "Firstname21", "Lastname21");
	addUser(userGUId++, 0, "Firstname22", "Lastname22");
	addUser(userGUId++, 0, "Firstname23", "Lastname23");
	addUser(userGUId++, 0, "Firstname24", "Lastname24");
	addUser(userGUId++, 0, "Firstname25", "Lastname25");
	addUser(userGUId++, 0, "Firstname26", "Lastname26");
	addUser(userGUId++, 0, "Firstname27", "Lastname27");
	addUser(userGUId++, 0, "Firstname28", "Lastname28");
	addUser(userGUId++, 0, "Firstname29", "Lastname29");

	for(const User& user1 : users)
	{
		for(const User& user2 : users)
		{
			Line3D c(user1.position, user2.position);
			connections.push_back(c);
		}
	}
}

void OrganizerRenderer::renderConnections(Renderer& r) const
{
	r.useShader("color");
	r.fill(Color(0.5, 0.5, 0.5, 1));
	r.lines(connections);
}

void OrganizerRenderer::render(Window& window, TiledRectangle area)
{
	Renderer& r = window.renderer;

	r.clearBackground(Color(1.0));
	r.setWireframeMode();
	r.setCulling(true);
	r.setDepthTest(true);

	// render scene
	player.render(window); // sets the location, rotation and projection

	for(const User& user : users)
	{
		r.useShader("normals"); // sets the color / material for the rendered objects
		r.uniforms().setMMatrix(matrixFromPosition(user.position));
		r.renderMesh("organizer user");
	}

	// render connections
	renderConnections(r);

	// render labels
	const Matrix vMatrix = r.uniforms().getVMatrix();
	const Matrix pMatrix = r.uniforms().getPMatrix();
	r.setDepthTest(false); // disables depth to always draw on top
	r.screenSpace(); // aligns coordinate system with screen space
	r.textRenderSystem.alignText(Alignment::center, Alignment::top);

	for(User& user : users)
	{
		r.fill(Color(0, 0, 0, 1));

		// calculate screen space position of 3d label text
		Vec4 pos = inverse(r.getScreenSpaceMatrix()) * pMatrix * vMatrix * Vec4(user.position, 0, 1);

		if(pos.z < 0) pos = Vec4(0);
		pos /= pos.z;
		pos.y -= 10;
		r.uniforms().setMMatrix(matrixFromPosition(Vec3(pos)));
		r.textRenderSystem.render(r, user.firstName + " " + user.lastName);
	}

	// text rendering
	r.uniforms().setMMatrix(Matrix(1));
	r.setDepthTest(false); // disables depth to always draw on top
	r.screenSpace(); // aligns coordinate system with screen space
	r.textRenderSystem.alignText(Alignment::left, Alignment::top);
	r.textRenderSystem.setSize(16.0f);
	r.textRenderSystem.setLetterSpacing(0.6f);
	r.textRenderSystem.render(r, "W A S D Q E to move"), Vec2(50, 250);
	r.textRenderSystem.render(r, "F for wire frame mode"), Vec2(50, 200);
	r.textRenderSystem.render(r, "Scroll to change speed"), Vec2(50, 150);
	r.textRenderSystem.render(r, "FPS: " + toString(static_cast<Int>(1.0f / r.time.getDelta()))), Vec2(50);
	// renders current frame rate to the screen
}

void OrganizerRenderer::addCompany(const Index guid, const String& companyName, const String& domain)
{
	Company company;
	company.companyGuid = guid;
	company.companyName = companyName;
	company.domain      = domain;
	companies.push_back(company);
}

void OrganizerRenderer::renderInteractive(Window& window, TiledRectangle area) {}

void OrganizerRenderer::addUser(const Index userGuid, const Index companyGuid, const String& firstName, const String& lastName)
{
	User user;
	user.companyGuid = companyGuid;
	user.userGuid    = userGuid;
	user.firstName   = firstName;
	user.lastName    = lastName;
	user.email       = firstName + "." + lastName + "@project-orka.com";
	user.position    = randomVec2Fast(-100.0f, 100.0f);
	users.push_back(user);
}
