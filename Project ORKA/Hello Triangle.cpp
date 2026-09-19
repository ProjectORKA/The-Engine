#include "Hello Triangle.hpp"
#include "Renderer.hpp"

void HelloTriangleRenderer::update(Window& window) {}

void HelloTriangleRenderer::destroy(Window& window)
{
	vao.destroy(window.renderer.openGlContext);
	indVBO.destroy(window.renderer.openGlContext);
	posVBO.destroy(window.renderer.openGlContext);
	GameRenderer::destroy(window);
}

void HelloTriangleRenderer::connect(GameSimulation& simulation) {}

void HelloTriangleRenderer::inputEvent(Window& window, InputEvent& input) {}

void HelloTriangleRenderer::create(Window& window)
{
	const String vertexShaderSource   = window.renderer.uniforms().getUniformShaderCode() + R"(
out vec3 vertColor;
void main()
{
	gl_Position = vec4(vertex.xyz, 1.0); vertColor = colors.xyz;
}
)";
	const String fragmentShaderSource = window.renderer.uniforms().getUniformShaderCode() + R"(
in vec3 vertColor;
out vec4 color; void main()
{
	color = vec4(vertColor, 1.0f);
}
)";
	glfwMakeContextCurrent(window.apiWindow); //[TODO] remove this

	window.renderer.shaderSystem.add(window.renderer, "HelloTriangle", vertexShaderSource, fragmentShaderSource);

	const Vector<Float> vertices = {-0.5f, -0.5f, 0.0f, +0.0f, +0.5f, 0.0f, +0.5f, -0.5f, 0.0f};

	const Vector<Float> vertColors = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};

	const Vector<UInt> indices = {0, 1, 2};

	posVBO.create(window.renderer.openGlContext, vertices, BufferUsage::StaticDraw, "Positions");
	colVBO.create(window.renderer.openGlContext, vertColors, BufferUsage::StaticDraw, "Colors");
	indVBO.create(window.renderer.openGlContext, indices, BufferUsage::StaticDraw, "Indices");

	vao.create(window.renderer.openGlContext);
	vao.bindVertexBuffer(window.renderer.openGlContext, 0, posVBO.getID(), 0, sizeof(Vec3), 0, 3, DataType::Float, false, 0);
	vao.bindVertexBuffer(window.renderer.openGlContext, 5, colVBO.getID(), 0, sizeof(Vec3), 0, 3, DataType::Float, false, 0);
	vao.bindIndexBuffer(window.renderer.openGlContext, indVBO.getID());

	GameRenderer::create(window);
}

void HelloTriangleRenderer::renderScene(Window& window)
{
	Renderer& r = window.renderer;

	framebuffer.clear(r);

	r.setRenderRegion(TiledRectangle(getPosition(), getSize()));
	r.clearBackground(Color(0, 0, 0, 0));
	r.useShader("HelloTriangle");

	vao.bind(window.renderer.openGlContext);
	vao.render(window.renderer.openGlContext, PrimitiveMode::Triangles, 3, nullptr);
}

void HelloTriangleRenderer::renderInteractive(Window& window) {}
