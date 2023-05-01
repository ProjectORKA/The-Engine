#include "Organizer.hpp"

void OrganizerRenderer::addTask(Task task) {
	tasks.pushBack(task);
}

void OrganizerRenderer::update(Window& window) {
	player.update(window);
}

void OrganizerRenderer::inputEvent(Window& window, InputEvent input) {
	if (input == enter) window.captureCursor();
	if (input == exit) window.uncaptureCursor();
	if (input == wireframeToogle) window.renderer.wireframeMode = !window.renderer.wireframeMode;

	player.inputEvent(window, input);
}

void OrganizerRenderer::create(ResourceManager& resourceManager, Window& window) {
	//setup renderer
	player.camera.location = Vec3(0, 0, 10);

	Index userGUID = 0;

	//load database
	addCompany(0, "Project ORKA", "project-orka.com");
	addUser(userGUID++, 0, "Blendurian", "Blend", "blendurian@project-orka.com");
	addUser(userGUID++, 0, "Deg", "Degster", "degster@project-orka.com");
	addUser(userGUID++, 0, "Blendurian", "Blend", "blendurian@project-orka.com");
	addUser(userGUID++, 0, "Deg", "Degster", "degster@project-orka.com");
	addUser(userGUID++, 0, "Blendurian", "Blend", "blendurian@project-orka.com");
	addUser(userGUID++, 0, "Deg", "Degster", "degster@project-orka.com");
	addUser(userGUID++, 0, "Blendurian", "Blend", "blendurian@project-orka.com");
	addUser(userGUID++, 0, "Deg", "Degster", "degster@project-orka.com");
	addUser(userGUID++, 0, "Blendurian", "Blend", "blendurian@project-orka.com");
	addUser(userGUID++, 0, "Deg", "Degster", "degster@project-orka.com");
	addUser(userGUID++, 0, "Blendurian", "Blend", "blendurian@project-orka.com");
	addUser(userGUID++, 0, "Deg", "Degster", "degster@project-orka.com");
	addUser(userGUID++, 0, "Blendurian", "Blend", "blendurian@project-orka.com");
	addUser(userGUID++, 0, "Deg", "Degster", "degster@project-orka.com");
	addUser(userGUID++, 0, "Blendurian", "Blend", "blendurian@project-orka.com");
	addUser(userGUID++, 0, "Deg", "Degster", "degster@project-orka.com");
	addUser(userGUID++, 0, "Blendurian", "Blend", "blendurian@project-orka.com");
	addUser(userGUID++, 0, "Deg", "Degster", "degster@project-orka.com");
	addUser(userGUID++, 0, "Blendurian", "Blend", "blendurian@project-orka.com");
	addUser(userGUID++, 0, "Deg", "Degster", "degster@project-orka.com");
	addUser(userGUID++, 0, "Blendurian", "Blend", "blendurian@project-orka.com");
	addUser(userGUID++, 0, "Deg", "Degster", "degster@project-orka.com");
	addUser(userGUID++, 0, "Blendurian", "Blend", "blendurian@project-orka.com");
	addUser(userGUID++, 0, "Deg", "Degster", "degster@project-orka.com");
	addUser(userGUID++, 0, "Blendurian", "Blend", "blendurian@project-orka.com");
	addUser(userGUID++, 0, "Deg", "Degster", "degster@project-orka.com");
	addUser(userGUID++, 0, "Blendurian", "Blend", "blendurian@project-orka.com");
	addUser(userGUID++, 0, "Deg", "Degster", "degster@project-orka.com");
	addUser(userGUID++, 0, "Blendurian", "Blend", "blendurian@project-orka.com");
	addUser(userGUID++, 0, "Deg", "Degster", "degster@project-orka.com");

	for (User& user1 : users) {
		for (User& user2 : users) {
			if (user1.userGUID > user2.userGUID && randomFloat() > 0.95) {
				Connection c;
				c.start = Vec3(user1.position, 0);
				c.end = Vec3(user2.position, 0);
				connections.push_back(c);
			}
		}
	}
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

	for (User& user : users) {
		r.useShader(resourceManager, "normals"); //sets the color / material for the rendered objects
		r.uniforms().mMatrix(matrixFromLocation(user.position));
		r.renderMesh(resourceManager, "organizer user");
	}

	//render connections
	r.useShader(resourceManager, "color");
	r.fill(Color(0.5, 0.5, 0.5, 1));
	for (Connection& c : connections) {
		r.line(c.start, c.end, 0.05);
	}

	//render labels
	Matrix vMatrix = r.uniforms().vMatrix();
	Matrix pMatrix = r.uniforms().pMatrix();
	r.setDepthTest(false); //disables depth to always draw on top
	r.screenSpace(); // aligns coordinate system with screenspace
	r.textRenderSystem.alignText(Alignment::center, Alignment::top);

	for (User& user : users) {
		r.fill(Color(0, 0, 0, 1));

		//calculate screenspace position of 3d label text
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
	r.screenSpace(); // aligns coordinate system with screenspace
	r.textRenderSystem.alignText(Alignment::end, Alignment::end);
	r.textRenderSystem.setStyle(fonts.paragraph);
	r.textRenderSystem.render(resourceManager, r, String("W A S D Q E to move"), Vec2(50, 250));
	r.textRenderSystem.render(resourceManager, r, String("F for wireframe mode"), Vec2(50, 200));
	r.textRenderSystem.render(resourceManager, r, String("Scroll to change speed"), Vec2(50, 150));
	r.textRenderSystem.render(resourceManager, r, String("FPS: ").append(toString(Int(1.0f / r.time.delta))), Vec2(50)); // renders current framerate to the screen
}
