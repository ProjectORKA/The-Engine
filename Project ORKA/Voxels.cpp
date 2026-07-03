#include "Voxels.hpp"

#include "GraphicsAPI.hpp"
#include "Math.hpp"
#include "Renderer.hpp"
#include "Window.hpp"
#include "Transform.hpp"
#include "Random.hpp"

namespace
{
	constexpr UInt voxelTextureSlot = 16; // avoid sampler2D slots 0..15 from uniforms.glsl
}

void GPUVoxelVolume::destroy(Renderer& renderer)
{
	if (!loaded) return;
	if (textureId != 0)
	{
		External::glDeleteTextures(1, &textureId);
		#ifdef DEBUG_OPENGL
		renderer.openGlContext.removeTexture(textureId);
		#endif
	}
	textureId = 0;
	loaded    = false;
}

void GPUVoxelVolume::upload(Renderer& renderer, const VoxelChunk& chunk)
{
	const IVec3 dims = chunk.dims();
	if (dims.x <= 0 || dims.y <= 0 || dims.z <= 0) return;
	if (chunk.voxels.empty()) return;

	destroy(renderer);

	size = dims;

	Int max3DTextureSize = 0;
	External::glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &max3DTextureSize);

	static Bool loggedMax3DTextureSize = false;
	if (!loggedMax3DTextureSize)
	{
		logDebug("Max 3D texture size supported by GPU: " + toString(max3DTextureSize));
		loggedMax3DTextureSize = true;
	}

	if (size.x > max3DTextureSize || size.y > max3DTextureSize || size.z > max3DTextureSize)
	{
		logWarning("Requested voxel volume " + toString(size) + " exceeds max 3D texture size " + toString(max3DTextureSize));
	}

	External::glCreateTextures(GL_TEXTURE_3D, 1, &textureId);
	#ifdef DEBUG_OPENGL
	renderer.openGlContext.addTexture(textureId);
	#endif

	OpenGL::apiObjectLabel(renderer.openGlContext, ObjectLabelType::Texture, textureId, "VoxelChunk3D");

	External::glTextureParameteri(textureId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	External::glTextureParameteri(textureId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	External::glTextureParameteri(textureId, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	External::glTextureParameteri(textureId, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	External::glTextureParameteri(textureId, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	External::glTextureStorage3D(textureId, 1, GL_R8, size.x, size.y, size.z);

	// Expand bit-packed voxels to bytes one slice at a time to avoid allocating a huge buffer.
	Vector<Byte> slice(static_cast<SizeT>(dims.x) * static_cast<SizeT>(dims.y));
	for (Int z = 0; z < dims.z; ++z)
	{
		for (Int y = 0; y < dims.y; ++y)
		{
			const SizeT rowOffset = static_cast<SizeT>(y) * static_cast<SizeT>(dims.x);
			for (Int x = 0; x < dims.x; ++x)
			{
				slice[rowOffset + static_cast<SizeT>(x)] = chunk.getVoxel(x, y, z) ? 255 : 0;
			}
		}

		External::glTextureSubImage3D(textureId, 0, 0, 0, z, dims.x, dims.y, 1, GL_RED, GL_UNSIGNED_BYTE, slice.data());
	}

	loaded = true;
}

void GPUVoxelVolume::bind(Renderer& renderer, const UInt slot) const
{
	if (!loaded) return;
	External::glBindTextureUnit(slot, textureId);
}

void VoxelSliceRenderer::destroy(Renderer& renderer)
{
	sliceGpuMesh.unload(renderer);
}

void VoxelSliceRenderer::build(Renderer& renderer, const VoxelChunk& chunk, const Vec3& chunkOrigin)
{
	const IVec3 dims = chunk.dims();
	const Vec3  minP = chunkOrigin;
	const Vec3  maxP = chunkOrigin + Vec3(dims);

	sliceCpuMesh.clearGeometry();
	sliceCpuMesh.name          = "voxel_slices_all_axes";
	sliceCpuMesh.primitiveMode = PrimitiveMode::Triangles;
	sliceCpuMesh.drawMode      = BufferUsage::StaticDraw;

	auto addSlice = [&](Int axis, Int step) {
		// axis: 0=x,1=y,2=z; step is slice index along positive face moving negative; skip final back face
		const Float pos = (axis == 0 ? maxP.x - static_cast<Float>(step) : (axis == 1 ? maxP.y - static_cast<Float>(step) : maxP.z - static_cast<Float>(step)));
		Vec3 n = Vec3(0);
		if (axis == 0) n.x = 1.0f;
		if (axis == 1) n.y = -1.0f;
		if (axis == 2) n.z = 1.0f;

		Vec3 v0, v1, v2, v3;
		Vec2 t0, t1, t2, t3;
		if (axis == 0)
		{
			v0 = Vec3(pos, minP.y, minP.z);
			v1 = Vec3(pos, maxP.y, minP.z);
			v2 = Vec3(pos, maxP.y, maxP.z);
			v3 = Vec3(pos, minP.y, maxP.z);
			t0 = Vec2(0, 0);
			t1 = Vec2(1, 0);
			t2 = Vec2(1, 1);
			t3 = Vec2(0, 1);
		}
		else if (axis == 1)
		{
			v0 = Vec3(minP.x, pos, minP.z);
			v1 = Vec3(maxP.x, pos, minP.z);
			v2 = Vec3(maxP.x, pos, maxP.z);
			v3 = Vec3(minP.x, pos, maxP.z);
			t0 = Vec2(0, 0);
			t1 = Vec2(1, 0);
			t2 = Vec2(1, 1);
			t3 = Vec2(0, 1);
		}
		else
		{
			v0 = Vec3(minP.x, minP.y, pos);
			v1 = Vec3(maxP.x, minP.y, pos);
			v2 = Vec3(maxP.x, maxP.y, pos);
			v3 = Vec3(minP.x, maxP.y, pos);
			t0 = Vec2(0, 0);
			t1 = Vec2(1, 0);
			t2 = Vec2(1, 1);
			t3 = Vec2(0, 1);
		}

		const Index base = static_cast<Index>(sliceCpuMesh.positions.size());
		sliceCpuMesh.positions.push_back(v0);
		sliceCpuMesh.positions.push_back(v1);
		sliceCpuMesh.positions.push_back(v2);
		sliceCpuMesh.positions.push_back(v3);
		sliceCpuMesh.textureCoordinates.push_back(t0);
		sliceCpuMesh.textureCoordinates.push_back(t1);
		sliceCpuMesh.textureCoordinates.push_back(t2);
		sliceCpuMesh.textureCoordinates.push_back(t3);
		sliceCpuMesh.vertexNormals.push_back(n);
		sliceCpuMesh.vertexNormals.push_back(n);
		sliceCpuMesh.vertexNormals.push_back(n);
		sliceCpuMesh.vertexNormals.push_back(n);

		// two triangles (v0,v1,v2) (v0,v2,v3)
		sliceCpuMesh.indices.push_back(base + 0);
		sliceCpuMesh.indices.push_back(base + 1);
		sliceCpuMesh.indices.push_back(base + 2);
		sliceCpuMesh.indices.push_back(base + 0);
		sliceCpuMesh.indices.push_back(base + 2);
		sliceCpuMesh.indices.push_back(base + 3);
	};

	// X slices: from +X face toward -X, skip the last (-X) face
	for (Int s = 0; s < dims.x + 1; s++) addSlice(0, s);
	// Y slices
	for (Int s = 0; s < dims.y + 1; s++) addSlice(1, s);
	// Z slices
	for (Int s = 0; s < dims.z + 1; s++) addSlice(2, s);

	sliceCpuMesh.checkIntegrity();
	sliceGpuMesh.upload(renderer, sliceCpuMesh);
}

void VoxelSliceRenderer::render(Renderer& renderer, const Vec3& chunkOrigin, const VoxelChunk& chunk, const Vec3& cameraPos)
{
	(void)chunkOrigin;
	(void)cameraPos;

	Transform t;
	//t.setRotation(0,0,renderer.time.getTotal()*10);
	renderer.uniforms().setCustomColor(Vec4(Vec3(chunk.dims()), 0.0f)); // w unused
	renderer.uniforms().setMMatrix(t.getMatrix());
	sliceGpuMesh.render(renderer);
	lastDrawnSlices = static_cast<Int>(sliceCpuMesh.indices.size() / 6); // quads
}

void VoxelRayRenderer::destroy(Renderer& renderer)
{
	cubeGpuMesh.unload(renderer);
}

void VoxelRayRenderer::build(Renderer& renderer, const VoxelChunk& chunk, const Vec3& chunkOrigin)
{
	const IVec3 dims = chunk.dims();
	const Vec3  minP = chunkOrigin;
	const Vec3  maxP = chunkOrigin + Vec3(dims);

	cubeCpuMesh.clearGeometry();
	cubeCpuMesh.name          = "voxel_ray_cube";
	cubeCpuMesh.primitiveMode = PrimitiveMode::Triangles;
	cubeCpuMesh.drawMode      = BufferUsage::StaticDraw;

	auto addFace = [&](const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& d, const Vec3& n) {
		const Index base = static_cast<Index>(cubeCpuMesh.positions.size());
		cubeCpuMesh.positions.push_back(a);
		cubeCpuMesh.positions.push_back(b);
		cubeCpuMesh.positions.push_back(c);
		cubeCpuMesh.positions.push_back(d);

		cubeCpuMesh.textureCoordinates.push_back(Vec2(0, 0));
		cubeCpuMesh.textureCoordinates.push_back(Vec2(1, 0));
		cubeCpuMesh.textureCoordinates.push_back(Vec2(1, 1));
		cubeCpuMesh.textureCoordinates.push_back(Vec2(0, 1));

		cubeCpuMesh.vertexNormals.push_back(n);
		cubeCpuMesh.vertexNormals.push_back(n);
		cubeCpuMesh.vertexNormals.push_back(n);
		cubeCpuMesh.vertexNormals.push_back(n);

		cubeCpuMesh.indices.push_back(base + 0);
		cubeCpuMesh.indices.push_back(base + 1);
		cubeCpuMesh.indices.push_back(base + 2);
		cubeCpuMesh.indices.push_back(base + 0);
		cubeCpuMesh.indices.push_back(base + 2);
		cubeCpuMesh.indices.push_back(base + 3);
	};

	// -Z face
	addFace(Vec3(minP.x, minP.y, minP.z), Vec3(maxP.x, minP.y, minP.z), Vec3(maxP.x, maxP.y, minP.z), Vec3(minP.x, maxP.y, minP.z), Vec3(0, 0, -1));
	// +Z face
	addFace(Vec3(minP.x, minP.y, maxP.z), Vec3(maxP.x, minP.y, maxP.z), Vec3(maxP.x, maxP.y, maxP.z), Vec3(minP.x, maxP.y, maxP.z), Vec3(0, 0, 1));
	// -X face
	addFace(Vec3(minP.x, minP.y, minP.z), Vec3(minP.x, maxP.y, minP.z), Vec3(minP.x, maxP.y, maxP.z), Vec3(minP.x, minP.y, maxP.z), Vec3(-1, 0, 0));
	// +X face
	addFace(Vec3(maxP.x, minP.y, minP.z), Vec3(maxP.x, maxP.y, minP.z), Vec3(maxP.x, maxP.y, maxP.z), Vec3(maxP.x, minP.y, maxP.z), Vec3(1, 0, 0));
	// +Y face
	addFace(Vec3(minP.x, maxP.y, minP.z), Vec3(maxP.x, maxP.y, minP.z), Vec3(maxP.x, maxP.y, maxP.z), Vec3(minP.x, maxP.y, maxP.z), Vec3(0, 1, 0));
	// -Y face
	addFace(Vec3(minP.x, minP.y, minP.z), Vec3(maxP.x, minP.y, minP.z), Vec3(maxP.x, minP.y, maxP.z), Vec3(minP.x, minP.y, maxP.z), Vec3(0, -1, 0));

	cubeCpuMesh.checkIntegrity();
	cubeGpuMesh.upload(renderer, cubeCpuMesh);
}

void VoxelRayRenderer::render(Renderer& renderer, const VoxelChunk& chunk, const Vec3& chunkOrigin)
{
	(void)chunkOrigin;

	renderer.uniforms().setCustomColor(Vec4(Vec3(chunk.dims()), 0.0f));
	renderer.uniforms().setMMatrix(Matrix(1));
	cubeGpuMesh.render(renderer);
}

void VoxelsRenderer::regenerateChunk(Renderer& renderer)
{
	// Single uniform edge length for simplicity (cube)
	chunk.edge = chunkSize;
	const IVec3 dims = chunk.dims();
	chunk.allocateStorage();

	const Vec3 center  = Vec3(dims) * 0.5f;
	const Float radius = static_cast<Float>(min(min(dims.x, dims.y), dims.z)) * 0.38f;

	for (Int z = 0; z < dims.z; ++z)
	{
		for (Int y = 0; y < dims.y; ++y)
		{
			for (Int x = 0; x < dims.x; ++x)
			{
				Bool filled = false;

				//const Vec3  p(static_cast<Float>(x) + 0.5f, static_cast<Float>(y) + 0.5f, static_cast<Float>(z) + 0.5f);
				//const Vec3  d = p - center;
				//const Float dist = length(d);
				//if(dist < radius) filled = true;
				//if (std::abs(d.x) < 1.5f && std::abs(d.z) < center.z * 0.65f) filled = true;
				//if (std::abs(d.y) < 1.5f && std::abs(d.z) < center.z * 0.65f) filled = true;
                //if (z == 0) filled = true;

				filled = randomFloatFast() < 0.1f;

				chunk.setVoxel(x, y, z, filled);
			}
		}
	}

	// Force voxels in all 8 corners
	const Int maxX = dims.x - 1;
	const Int maxY = dims.y - 1;
	const Int maxZ = dims.z - 1;
	const Int cornerCoords[8][3] = {
		{0,    0,    0   },
		{maxX, 0,    0   },
		{0,    maxY, 0   },
		{maxX, maxY, 0   },
		{0,    0,    maxZ},
		{maxX, 0,    maxZ},
		{0,    maxY, maxZ},
		{maxX, maxY, maxZ}
	};
	for (const auto& c : cornerCoords)
	{
		chunk.setVoxel(c[0], c[1], c[2], true);
	}

	chunkOrigin = -Vec3(dims) * 0.5f;
	needsUpload = true;
	sliceRenderer.build(renderer, chunk, chunkOrigin);
	rayRenderer.build(renderer, chunk, chunkOrigin);
}

void VoxelsRenderer::create(Window& window)
{
	Renderer & r = window.renderer;

	setName("VoxelsRenderer");
	regenerateChunk(r);
	const IVec3 dims = chunk.dims();
	player.camera.setPosition(Vec3(0.0f, -static_cast<Float>(dims.y), static_cast<Float>(dims.z) * 0.4f));
	GameRenderer::create(window);
    r.shaderSystem.add(r, sliceRenderer.shaderName);
    r.shaderSystem.add(r, rayRenderer.shaderName);
	volume.upload(r, chunk);
}

void VoxelsRenderer::update(Window& window)
{
	player.update(window);
}

void VoxelsRenderer::destroy(Window& window)
{
	sliceRenderer.destroy(window.renderer);
	rayRenderer.destroy(window.renderer);
	volume.destroy(window.renderer);
	GameRenderer::destroy(window);
}

void VoxelsRenderer::inputEvent(Window& window, InputEvent& input)
{
	if (input == enter) ui.capture(*this);
	if (input == exit) ui.releaseCapture();
	if (input == switchToSlices) renderMode = VoxelRenderMode::Slices;
	if (input == switchToRay)    renderMode = VoxelRenderMode::RayMarch;
	if (input == wireframeToggle) { window.renderer.wireframeMode = !window.renderer.wireframeMode; }
	if (input == reloadShaders)	{ window.renderer.shaderSystem.rebuild(window.renderer); }

	player.inputEvent(window, input);
}

void VoxelsRenderer::renderScene(Window& window)
{
	Renderer& renderer = window.renderer;
	renderer.setCulling(false);
	renderer.setDepthTest(true);
	renderer.setWireframeMode();

	framebuffer.clearColor(renderer, Color(0.08f, 0.08f, 0.1f, 1.0f));
	framebuffer.clearDepth(renderer);

	player.render(window); // sets camera matrices

	const Name shaderToUse = (renderMode == VoxelRenderMode::Slices) ? sliceRenderer.shaderName : rayRenderer.shaderName;
	renderer.useShader(shaderToUse);
	renderer.uniforms().setWorldOffset(Vec4(chunkOrigin, 0.0f));

	volume.bind(renderer, voxelTextureSlot);
	if (renderMode == VoxelRenderMode::Slices)
	{
		sliceRenderer.render(renderer, chunkOrigin, chunk, player.camera.getPosition());
	}
	else
	{
		rayRenderer.render(renderer, chunk, chunkOrigin);
	}

	// bounding box wireframe
	const Vec3 chunkCenter = chunkOrigin + Vec3(chunk.dims()) * 0.5f;
	renderer.useShader("color");
	renderer.uniforms().setCustomColor(Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	{
		const IVec3 dims = chunk.dims();
		Transform t;
		t.setPosition(chunkCenter);
		if (renderMode == VoxelRenderMode::Slices)
		{
			//t.setRotation(0,0,renderer.time.getTotal() * 10);
		}
		t.setSize(dims);
		renderer.uniforms().setMMatrix(t.getMatrix());
	}
	renderer.wireframeCubeCentered();

	// reset state for UI overlay
	renderer.uniforms().setCustomColor(Vec4(1.0f));
	renderer.uniforms().setMMatrix(Matrix(1));
	renderer.screenSpaceFromBottomLeft();
	renderer.setDepthTest(false);
	renderer.setCulling(false);
	renderer.fontSize(16.0f);
	renderer.letterSpacing(0.6f);
	renderer.textRenderSystem.alignText(Alignment::left, Alignment::bottom);

	constexpr Float textSpacing = 20.0f;
	Int line = 1;
	renderer.text("FPS: " + toString(static_cast<Int>(1.0f / renderer.time.getDelta())), Vec2(textSpacing, textSpacing * line++));
	{
		const IVec3 dims = chunk.dims();
		renderer.text("Chunk: " + toString(dims.x) + " x " + toString(dims.y) + " x " + toString(dims.z), Vec2(textSpacing, textSpacing * line++));
	}
	{
		const Vec3 camPos = player.camera.getPosition();
		const Vec3 camDir = player.camera.getForwardVector();
		renderer.text("Cam pos: " + toString(camPos), Vec2(textSpacing, textSpacing * line++));
		renderer.text("Cam dir: " + toString(camDir), Vec2(textSpacing, textSpacing * line++));
	}
	renderer.text(renderMode == VoxelRenderMode::Slices ? "Renderer: Slices [1]" : "Renderer: Raymarch [2]", Vec2(textSpacing, textSpacing * line++));
	if (renderMode == VoxelRenderMode::Slices)
	{
		renderer.text("Slices drawn: " + toString(sliceRenderer.lastDrawnSlices), Vec2(textSpacing, textSpacing * line++));
	}
	else
	{
		renderer.text("Raymarch: cube proxy, grid DDA traversal", Vec2(textSpacing, textSpacing * line++));
	}
	renderer.text("Controls: 1 slices, 2 raymarch, LMB capture, RMB release, F wireframe", Vec2(textSpacing, textSpacing * line++));
}