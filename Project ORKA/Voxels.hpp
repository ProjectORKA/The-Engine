#pragma once
#include "Game.hpp"
#include "Player.hpp"
#include "CPUMesh.hpp"
#include "GPUMesh.hpp"

struct Renderer;
struct Window;

struct VoxelChunk
{
	Int          edge   = 64;
	Vector<Byte> voxels;

	[[nodiscard]] IVec3 dims() const { return IVec3(edge); }

	[[nodiscard]] SizeT voxelCount() const
	{
		const SizeT e = static_cast<SizeT>(edge);
		return e * e * e;
	}

	void allocateStorage()
	{
		const SizeT byteCount = (voxelCount() + 7) / 8;
		voxels.assign(byteCount, 0);
	}

	[[nodiscard]] SizeT index(Int x, Int y, Int z) const
	{
		const SizeT e = static_cast<SizeT>(edge);
		return static_cast<SizeT>(x) + e * (static_cast<SizeT>(y) + e * static_cast<SizeT>(z));
	}

	void setVoxel(Int x, Int y, Int z, Bool filled)
	{
		const SizeT bitIndex  = index(x, y, z);
		const SizeT byteIndex = bitIndex >> 3;
		const Byte  mask      = static_cast<Byte>(1u << (bitIndex & 7u));
		if (filled)
		{
			voxels[byteIndex] |= mask;
		}
		else
		{
			voxels[byteIndex] &= static_cast<Byte>(~mask);
		}
	}

	[[nodiscard]] Bool getVoxel(Int x, Int y, Int z) const
	{
		const SizeT bitIndex  = index(x, y, z);
		const SizeT byteIndex = bitIndex >> 3;
		const Byte  mask      = static_cast<Byte>(1u << (bitIndex & 7u));
		return (voxels[byteIndex] & mask) != 0;
	}
};

struct GPUVoxelVolume
{
	IVec3 size      = IVec3(0);
	UInt  textureId = 0;
	Bool  loaded    = false;

	void upload(Renderer& renderer, const VoxelChunk& chunk);
	void destroy(Renderer& renderer);
	void bind(Renderer& renderer, UInt slot) const;
};

enum class VoxelRenderMode
{
	Slices  = 0,
	RayMarch = 1
};

struct VoxelSliceRenderer final
{
	CpuMesh sliceCpuMesh;
	GPUMesh sliceGpuMesh;
	Name    shaderName = "voxelSlices";
	Int     lastDrawnSlices = 0;

	void build(Renderer& renderer, const VoxelChunk& chunk, const Vec3& chunkOrigin);
	void render(Renderer& renderer, const Vec3& chunkOrigin, const VoxelChunk& chunk, const Vec3& cameraPos);
	void destroy(Renderer& renderer);
};

struct VoxelRayRenderer final
{
	CpuMesh cubeCpuMesh;
	GPUMesh cubeGpuMesh;
	Name    shaderName = "voxelRay";

	void build(Renderer& renderer, const VoxelChunk& chunk, const Vec3& chunkOrigin);
	void render(Renderer& renderer, const VoxelChunk& chunk, const Vec3& chunkOrigin);
	void destroy(Renderer& renderer);
};

struct VoxelsRenderer final : GameRenderer
{
	Mutex       mutex;
	DebugPlayer player;
	VoxelChunk  chunk;
	GPUVoxelVolume volume;
	VoxelSliceRenderer sliceRenderer;
	VoxelRayRenderer   rayRenderer;

	Int   chunkSize    = 256;
	Vec3  chunkOrigin   = Vec3(0);
	Bool  needsUpload   = true;
	VoxelRenderMode renderMode = VoxelRenderMode::RayMarch;
	InputEvent enter    = InputEvent(InputType::Mouse, Lmb, true);
	InputEvent exit     = InputEvent(InputType::Mouse, Rmb, false);
	InputEvent switchToSlices   = InputEvent(InputType::KeyBoard, One, true);
	InputEvent switchToRay      = InputEvent(InputType::KeyBoard, Two, true);
	InputEvent wireframeToggle = InputEvent(InputType::KeyBoard, F, true);
	InputEvent reloadShaders    = InputEvent(InputType::KeyBoard, T, true);

	void create(Window& window) override;
	void update(Window& window) override;
	void destroy(Window& window) override;
	void renderScene(Window& window) override;
	void renderInteractive(Window& window) override {}
	void connect(GameSimulation& simulation) override {}
	void inputEvent(Window& window, InputEvent input) override;

private:
	void regenerateChunk(Renderer& renderer);
};

struct Voxels
{
	VoxelsRenderer renderer;

	void run()
	{
		Window& window = ui.window("ORKA Voxels", settings.defaultWindowWidth, settings.defaultWindowHeight, true, true, WindowState::Windowed);
		window.addElement(renderer);
		ui.run();
	}
};