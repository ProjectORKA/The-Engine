#include "Orbital Oblivion.hpp"
#include "Renderer.hpp"

void OrbitalOblivionRenderer::create(Window& window)
{
	GameRenderer::create(window);
	window.renderer.meshSystem.use(window.renderer, "unit");
	unitMesh = &window.renderer.meshSystem.currentMesh();
}

void OrbitalOblivionRenderer::destroy(Window& window)
{
	GameRenderer::destroy(window);
}

void OrbitalOblivionRenderer::renderScene(Window& window)
{
	Renderer& r = window.renderer;

	r.setWireframeMode(r.wireframeMode);

	r.clearBackground(Color(0, 0, 0, 1));
	r.setDepthTest(true);
	r.setCulling(true);

	player.update(window);
	player.render(window);

	r.useTexture("orbitalOblivionReflection");
	r.useShader("orbitalOblivion");

	Vector<Matrix> positions = Vector<Matrix>(sim->units.size());
	for (SizeT i = 0; i < sim->units.size(); i++) positions[i] = matrixFromPositionAndDirection(sim->units[i].position, sim->units[i].direction);
	r.renderMeshInstanced("unit", positions);

	r.useMesh("sphere");
	for (OOPlanet& p : sim->planets) p.render(r);

	//if(bloom) renderBloom(r);

	r.setWireframeMode(false);
	r.screenSpaceFromBottomLeft();
	r.uniforms().setMMatrix(Matrix(1));
	r.fontSize(16);
	r.letterSpacing(0.6f);
	r.text("Framerate: " + toString(1 / r.time.getDelta()), Vec2(50, 50));
	r.textRenderSystem.render(r, "Ship Count: " + toString(static_cast<Int>(sim->units.size())), Vec2(50, 100));
}