#include "Hello Triangle.hpp"

void HelloTriangle::run()
{
	resourceManager.create();
	ui.create();
	ui.window("Hello Triangle", Area(1920, 1080), true, true, WindowState::Windowed, renderer, resourceManager);
	ui.run();
}

void HelloTriangleRenderer::update(Window& window) {}

void HelloTriangleRenderer::destroy(Window& window)
{
	vao.destroy();
	indVBO.destroy();
	posVBO.destroy();
	mainFramebuffer.destroy();
}

void HelloTriangleRenderer::connect(GameSimulation& simulation) {}

void HelloTriangleRenderer::inputEvent(Window& window, InputEvent input) {}

void HelloTriangleRenderer::create(ResourceManager& rm, Window& window)
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
	glfwMakeContextCurrent(window.apiWindow);

	window.renderer.shaderSystem.add("HelloTriangle", vertexShaderSource, fragmentShaderSource);

	const Vector<Float> vertices = {-0.5f, -0.5f, 0.0f, +0.0f, +0.5f, 0.0f, +0.5f, -0.5f, 0.0f};

	const Vector<Float> vertColors = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};

	const Vector<UInt> indices = {0, 1, 2};

	posVBO.create(vertices, BufferUsage::StaticDraw, "Positions");
	colVBO.create(vertColors, BufferUsage::StaticDraw, "Colors");
	indVBO.create(indices, BufferUsage::StaticDraw, "Indices");

	vao.create();
	vao.bindVertexBuffer(0, posVBO.getID(), 0, sizeof(Vec3), 0, 3, DataType::Float, false, 0);
	vao.bindVertexBuffer(5, colVBO.getID(), 0, sizeof(Vec3), 0, 3, DataType::Float, false, 0);
	vao.bindIndexBuffer(indVBO.getID());

	mainFramebuffer.create("Main Framebuffer", Area(1920, 1080));
	mainFramebuffer.add(WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Clamped);
	mainFramebuffer.add(WritePixelsFormat::Depth, DataType::Float, FramebufferAttachment::Depth, false, Wrapping::Clamped);
	mainFramebuffer.checkComplete();
}

void HelloTriangleRenderer::render(ResourceManager& rm, Window& window, const TiledRectangle area)
{
	Renderer& r = window.renderer;

	r.drawToWindow();

	r.setRenderRegion(area);

	r.clearBackground(Color(0, 0, 0, 1));

	r.useShader(rm, "HelloTriangle");

	vao.bind();
	vao.render(PrimitiveMode::Triangles, 3, nullptr);
}

void HelloTriangleRenderer::renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle area) {}
