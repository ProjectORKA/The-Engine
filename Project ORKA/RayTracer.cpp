#include "RayTracer.hpp"

#include "CPUTexture.hpp"
#include "Image.hpp"
#include "JobSystem.hpp"
#include "Math.hpp"
#include "Random.hpp"
#include "Renderer.hpp"
#include "Window.hpp"

#include <atomic>
#include <cmath>

namespace
{
	constexpr Float kRayEpsilon     = 1e-4f;
	constexpr Float kShadowEpsilon  = 1e-3f;
	constexpr Float kInfinity       = 1e30f;
	constexpr Int kRussianRouletteStart = 5;

	struct Ray
	{
		Vec3 origin;
		Vec3 direction;
	};

	enum class MaterialType : UInt
	{
		Lambertian,
		Metal,
		Dielectric,
		Emissive
	};

	struct Material
	{
		MaterialType type     = MaterialType::Lambertian;
		Vec3         albedo   = Vec3(1);
		Vec3         emission = Vec3(0);
		Float        fuzz     = 0.0f;
		Float        ior      = 1.5f;
	};

	struct HitRecord
	{
		Vec3     point;
		Vec3     normal;
		Float    t          = kInfinity;
		UInt     materialId = 0;
		Bool     frontFace  = true;
	};

	struct AxisAlignedBox
	{
		Vec3 minBounds = Vec3(0);
		Vec3 maxBounds = Vec3(0);
		UInt materialId = 0;

		[[nodiscard]] Bool hit(const Ray& ray, Float tMin, Float tMax, HitRecord& out) const
		{
			Float t0 = tMin;
			Float t1 = tMax;

			for (Int axis = 0; axis < 3; ++axis)
			{
				const Float originComponent  = ray.origin[axis];
				const Float directionComponent = ray.direction[axis];
				Float       invDirection     = 0.0f;

				if (std::fabs(directionComponent) > kRayEpsilon) invDirection = 1.0f / directionComponent;

				Float tNear = (minBounds[axis] - originComponent) * invDirection;
				Float tFar  = (maxBounds[axis] - originComponent) * invDirection;

				if (invDirection < 0.0f) std::swap(tNear, tFar);

				t0 = tNear > t0 ? tNear : t0;
				t1 = tFar < t1 ? tFar : t1;

				if (t1 <= t0) return false;
			}

			const Float hitT = t0;
			if (hitT <= tMin || hitT >= tMax) return false;

			out.t          = hitT;
			out.point      = ray.origin + ray.direction * hitT;
			out.materialId = materialId;

			const Vec3 center = (minBounds + maxBounds) * 0.5f;
			const Vec3 extent = (maxBounds - minBounds) * 0.5f;
			const Vec3 local  = (out.point - center) / extent;

			Vec3 normal(0);
			Float absX = std::fabs(local.x);
			Float absY = std::fabs(local.y);
			Float absZ = std::fabs(local.z);

			if (absX > absY && absX > absZ) normal = Vec3(std::copysign(1.0f, local.x), 0, 0);
			else if (absY > absZ) normal = Vec3(0, std::copysign(1.0f, local.y), 0);
			else normal = Vec3(0, 0, std::copysign(1.0f, local.z));

			out.frontFace = dot(ray.direction, normal) < 0.0f;
			if (!out.frontFace) normal = -normal;
			out.normal = normal;
			return true;
		}
	};

	struct RtScene
	{
		Vector<AxisAlignedBox> boxes;
		Vector<Material>       materials;

		void addBox(const Vec3& minCorner, const Vec3& maxCorner, const Material& material)
		{
			materials.push_back(material);
			boxes.push_back({minCorner, maxCorner, static_cast<UInt>(materials.size() - 1)});
		}

		[[nodiscard]] Bool hit(const Ray& ray, Float tMin, Float tMax, HitRecord& out) const
		{
			HitRecord temp;
			Bool      hitAnything = false;
			Float     closest     = tMax;

			for (const AxisAlignedBox& box : boxes)
			{
				if (box.hit(ray, tMin, closest, temp))
				{
					hitAnything = true;
					closest     = temp.t;
					out         = temp;
				}
			}

			return hitAnything;
		}

		static RtScene buildCornellBox()
		{
			RtScene scene;

			const Material whiteLambertian  = {MaterialType::Lambertian, Vec3(0.73f, 0.73f, 0.73f)};
			const Material redLambertian    = {MaterialType::Lambertian, Vec3(0.65f, 0.05f, 0.05f)};
			const Material greenLambertian  = {MaterialType::Lambertian, Vec3(0.12f, 0.45f, 0.15f)};
			const Material lightEmissive    = {MaterialType::Emissive, Vec3(0), Vec3(12.0f, 12.0f, 12.0f)};
			const Material glassDielectric  = {MaterialType::Dielectric, Vec3(1), Vec3(0), 0.0f, 1.5f};
			const Material goldMetal        = {MaterialType::Metal, Vec3(0.85f, 0.65f, 0.15f), Vec3(0), 0.15f};
			const Material copperMetal      = {MaterialType::Metal, Vec3(0.85f, 0.35f, 0.20f), Vec3(0), 0.25f};

			const Float wallThickness = 0.01f;
			const Vec3  roomMin(-1.0f, 0.0f, 0.0f);
			const Vec3  roomMax(1.0f, 2.0f, 2.0f);

			// Floor
			scene.addBox(Vec3(roomMin.x, roomMin.y, roomMin.z), Vec3(roomMax.x, roomMax.y, roomMin.z + wallThickness), whiteLambertian);
			// Ceiling panels (opening left for the area light)
			scene.addBox(Vec3(roomMin.x, roomMin.y, roomMax.z - wallThickness), Vec3(-0.30f, roomMax.y, roomMax.z), whiteLambertian);
			scene.addBox(Vec3(0.30f, roomMin.y, roomMax.z - wallThickness), Vec3(roomMax.x, roomMax.y, roomMax.z), whiteLambertian);
			scene.addBox(Vec3(-0.30f, roomMin.y, roomMax.z - wallThickness), Vec3(0.30f, 0.78f, roomMax.z), whiteLambertian);
			scene.addBox(Vec3(-0.30f, 1.22f, roomMax.z - wallThickness), Vec3(0.30f, roomMax.y, roomMax.z), whiteLambertian);
			// Back wall
			scene.addBox(Vec3(roomMin.x, roomMax.y - wallThickness, roomMin.z), Vec3(roomMax.x, roomMax.y, roomMax.z), whiteLambertian);
			// Left wall (red)
			scene.addBox(Vec3(roomMin.x, roomMin.y, roomMin.z), Vec3(roomMin.x + wallThickness, roomMax.y, roomMax.z), redLambertian);
			// Right wall (green)
			scene.addBox(Vec3(roomMax.x - wallThickness, roomMin.y, roomMin.z), Vec3(roomMax.x, roomMax.y, roomMax.z), greenLambertian);

			// Area light on ceiling
			scene.addBox(Vec3(-0.28f, 0.80f, 1.97f), Vec3(0.28f, 1.20f, 2.0f), lightEmissive);

			// Tall diffuse box
			scene.addBox(Vec3(0.15f, 0.55f, 0.0f), Vec3(0.85f, 1.35f, 0.95f), whiteLambertian);
			// Short metal box
			scene.addBox(Vec3(-0.85f, 0.0f, 0.0f), Vec3(-0.15f, 0.55f, 0.75f), copperMetal);
			// Glass sphere approximated as small cube (classic Cornell often uses boxes; add glass accent)
			scene.addBox(Vec3(-0.35f, 0.85f, 0.55f), Vec3(-0.05f, 1.15f, 0.85f), glassDielectric);
			// Gold accent on tall box
			scene.addBox(Vec3(0.55f, 1.05f, 0.55f), Vec3(0.75f, 1.25f, 0.75f), goldMetal);

			return scene;
		}
	};

	struct RtCamera
	{
		Vec3 origin       = Vec3(0.0f, -0.15f, 1.02f);
		Vec3 lowerLeft    = Vec3(0);
		Vec3 horizontal   = Vec3(0);
		Vec3 vertical     = Vec3(0);
		Vec3 uBasis       = Vec3(0);
		Vec3 vBasis       = Vec3(0);
		Vec3 wBasis       = Vec3(0);
		Float lensRadius  = 0.0f;

		void lookAt(const Vec3& lookFrom, const Vec3& lookAtPoint, const Vec3& worldUp, const Float verticalFovDegrees, const Float aspectRatio, const Float aperture, const Float focusDist)
		{
			const Float theta     = verticalFovDegrees * (PI / 180.0f);
			const Float halfHeight = std::tan(theta * 0.5f);
			const Float viewportHeight = 2.0f * halfHeight * focusDist;
			const Float viewportWidth  = viewportHeight * aspectRatio;

			wBasis = normalize(lookAtPoint - lookFrom);
			uBasis = normalize(cross(wBasis, worldUp));
			vBasis = cross(wBasis, uBasis);

			origin     = lookFrom;
			horizontal = uBasis * viewportWidth;
			vertical   = vBasis * viewportHeight;
			lowerLeft  = origin - wBasis * focusDist - horizontal * 0.5f - vertical * 0.5f;
			lensRadius = aperture;
		}

		[[nodiscard]] Ray getRay(const Float s, const Float t, const Vec2& lensSample) const
		{
			const Vec3 offset = lensRadius > 0.0f ? (uBasis * lensSample.x + vBasis * lensSample.y) * lensRadius : Vec3(0);
			const Vec3 direction = lowerLeft + horizontal * s + vertical * t - (origin + offset);
			return {origin + offset, normalize(direction)};
		}
	};

	[[nodiscard]] Float schlickApproximation(const Float cosine, const Float refractionIndex)
	{
		Float r0 = (1.0f - refractionIndex) / (1.0f + refractionIndex);
		r0       = r0 * r0;
		return r0 + (1.0f - r0) * std::pow(1.0f - cosine, 5.0f);
	}

	[[nodiscard]] Vec3 randomUnitVector()
	{
		while (true)
		{
			const Vec3 point = randomVec3Fast(-1.0f, 1.0f);
			if (dot(point, point) < 1.0f) return point;
		}
	}

	[[nodiscard]] Vec3 randomInUnitDisk()
	{
		while (true)
		{
			const Vec2 point = randomVec2Fast(-1.0f, 1.0f);
			if (dot(point, point) < 1.0f) return Vec3(point.x, point.y, 0.0f);
		}
	}

	[[nodiscard]] Vec3 randomInHemisphere(const Vec3& normal)
	{
		const Vec3 inUnitSphere = randomUnitVector();
		if (dot(inUnitSphere, normal) > 0.0f) return inUnitSphere;
		return -inUnitSphere;
	}

	[[nodiscard]] Vec3 reflectVector(const Vec3& vector, const Vec3& normal)
	{
		return vector - normal * 2.0f * dot(vector, normal);
	}

	[[nodiscard]] Bool refractVector(const Vec3& incident, const Vec3& normal, const Float etaRatio, Vec3& refracted)
	{
		const Float cosTheta = std::fmin(dot(-incident, normal), 1.0f);
		const Float sinTheta = std::sqrt(1.0f - cosTheta * cosTheta);
		const Float sinT     = etaRatio * sinTheta;
		if (sinT > 1.0f) return false;
		refracted = etaRatio * incident + (etaRatio * cosTheta - std::sqrt(1.0f - sinT * sinT)) * normal;
		return true;
	}

	[[nodiscard]] Bool scatter(const Material& material, const Ray& rayIn, const HitRecord& hit, Vec3& attenuation, Ray& scattered)
	{
		switch (material.type)
		{
		case MaterialType::Lambertian:
		{
			const Vec3 scatterDirection = hit.normal + randomUnitVector();
			if (dot(scatterDirection, scatterDirection) < kRayEpsilon) scattered.direction = hit.normal;
			else scattered.direction = normalize(scatterDirection);
			scattered.origin    = hit.point + hit.normal * kShadowEpsilon;
			attenuation         = material.albedo;
			return true;
		}
		case MaterialType::Metal:
		{
			const Vec3 reflected = reflectVector(normalize(rayIn.direction), hit.normal) + material.fuzz * randomUnitVector();
			scattered.origin    = hit.point + hit.normal * kShadowEpsilon;
			scattered.direction = normalize(reflected);
			attenuation         = material.albedo;
			return dot(scattered.direction, hit.normal) > 0.0f;
		}
		case MaterialType::Dielectric:
		{
			attenuation          = Vec3(1);
			scattered.origin     = hit.point;
			const Float refRatio = hit.frontFace ? 1.0f / material.ior : material.ior;
			const Vec3 unitDirection = normalize(rayIn.direction);
			const Float cosTheta     = std::fmin(dot(-unitDirection, hit.normal), 1.0f);
			const Float sinTheta     = std::sqrt(1.0f - cosTheta * cosTheta);
			const Bool cannotRefract = refRatio * sinTheta > 1.0f;

			if (cannotRefract || schlickApproximation(cosTheta, refRatio) > randomFloatFast())
				scattered.direction = reflectVector(unitDirection, hit.normal);
			else
			{
				Vec3 refracted;
				if (!refractVector(unitDirection, hit.normal, refRatio, refracted)) return false;
				scattered.direction = refracted;
			}
			scattered.origin = hit.frontFace ? hit.point - hit.normal * kShadowEpsilon : hit.point + hit.normal * kShadowEpsilon;
			return true;
		}
		case MaterialType::Emissive:
		default:
			return false;
		}
	}

	[[nodiscard]] Vec3 emitted(const Material& material, const HitRecord& hit, const Ray& ray)
	{
		if (material.type != MaterialType::Emissive) return Vec3(0);
		if (dot(ray.direction, hit.normal) < 0.0f) return material.emission;
		return Vec3(0);
	}

	[[nodiscard]] Vec3 traceColor(const Ray& ray, const RtScene& scene, Int depth)
	{
		if (depth <= 0) return Vec3(0);

		HitRecord hit;
		if (!scene.hit(ray, kShadowEpsilon, kInfinity, hit)) return Vec3(0);

		const Material& material = scene.materials[hit.materialId];
		Vec3            color    = emitted(material, hit, ray);

		if (depth <= kRussianRouletteStart)
		{
			const Float survivalProbability = std::fmax(material.albedo.x, std::fmax(material.albedo.y, material.albedo.z));
			if (randomFloatFast() > survivalProbability) return color;
		}

		Vec3 attenuation;
		Ray  scattered;
		if (scatter(material, ray, hit, attenuation, scattered))
		{
			const Float weight = material.type == MaterialType::Lambertian && depth <= kRussianRouletteStart
				? 1.0f / std::fmax(attenuation.x, std::fmax(attenuation.y, attenuation.z))
				: 1.0f;
			color += attenuation * traceColor(scattered, scene, depth - 1) * weight;
		}

		return color;
	}

	[[nodiscard]] Vec3 acesToneMap(Vec3 color)
	{
		color *= 0.6f;
		return Vec3(
			(color.x * (2.51f * color.x + 0.03f)) / (color.x * (2.43f * color.x + 0.59f) + 0.14f),
			(color.y * (2.51f * color.y + 0.03f)) / (color.y * (2.43f * color.y + 0.59f) + 0.14f),
			(color.z * (2.51f * color.z + 0.03f)) / (color.z * (2.43f * color.z + 0.59f) + 0.14f));
	}

	[[nodiscard]] Vec3 displayColor(const Vec3& accumulated, const UInt samples)
	{
		if (samples == 0) return Vec3(0);
		Vec3 color = accumulated / static_cast<Float>(samples);
		color      = acesToneMap(color);
		color      = Vec3(std::sqrt(clamp(color.x, 0.0f, 1.0f)), std::sqrt(clamp(color.y, 0.0f, 1.0f)), std::sqrt(clamp(color.z, 0.0f, 1.0f)));
		return color;
	}

	struct TraceContext
	{
		const RtScene*         scene         = nullptr;
		const RtCamera*        camera        = nullptr;
		Vector<Vec3>*          accumulation  = nullptr;
		Vector<UInt>*          samples       = nullptr;
		std::atomic<UInt>*     completedRows = nullptr;
		IVec2                  size;
		UInt                   seed;
		Int                    maxDepth;
		Int                    samplesPerFrame;
	};

	void traceRows(const TraceContext& ctx, const Int startRow, const Int endRow)
	{
		randomizeSeed();
		const RtScene&  scene  = *ctx.scene;
		const RtCamera& camera = *ctx.camera;
		const Float denomX = static_cast<Float>(max(1, ctx.size.x - 1));
		const Float denomY = static_cast<Float>(max(1, ctx.size.y - 1));

		for (Int y = startRow; y < endRow; ++y)
		{
			for (Int x = 0; x < ctx.size.x; ++x)
			{
				Vec3 pixelColor(0);
				for (Int sample = 0; sample < ctx.samplesPerFrame; ++sample)
				{
					const Float u = (static_cast<Float>(x) + randomFloatFast()) / denomX;
					const Float v = (static_cast<Float>(y) + randomFloatFast()) / denomY;
					const Vec2  lensSample = randomInUnitDisk();
					const Ray   ray        = camera.getRay(u, v, lensSample);
					pixelColor += traceColor(ray, scene, ctx.maxDepth);
				}

				const SizeT index = static_cast<SizeT>(y) * static_cast<SizeT>(ctx.size.x) + static_cast<SizeT>(x);
				(*ctx.accumulation)[index] += pixelColor;
				(*ctx.samples)[index] += static_cast<UInt>(ctx.samplesPerFrame);
			}
		}

		if (ctx.completedRows) ctx.completedRows->fetch_add(1, std::memory_order_relaxed);
	}
}

void RayTracerRenderer::resetAccumulation(const IVec2& size)
{
	const SizeT pixelCount = static_cast<SizeT>(size.x) * static_cast<SizeT>(size.y);
	colorAccumulation.assign(pixelCount, Vec3(0));
	sampleCounts.assign(pixelCount, 0);
	frameSamples = 0;
	sceneDirty   = false;
	renderSize   = size;
}

void RayTracerRenderer::markSceneDirty()
{
	sceneDirty = true;
}

void RayTracerRenderer::traceFrame(const IVec2& size, const UInt seed)
{
	if (size.x <= 0 || size.y <= 0) return;

	const SizeT pixelCount = static_cast<SizeT>(size.x) * static_cast<SizeT>(size.y);
	if (colorAccumulation.size() != pixelCount || renderSize != size || sceneDirty)
	{
		resetAccumulation(size);
	}

	const RtScene scene = RtScene::buildCornellBox();
	const Float   aspect = static_cast<Float>(size.x) / static_cast<Float>(size.y);

	RtCamera camera;
	camera.lookAt(
		player.camera.getPosition(),
		player.camera.getPosition() + player.camera.getForwardVector(),
		Vec3(0, 0, 1),
		verticalFov,
		aspect,
		enableDepthOfField ? lensRadius : 0.0f,
		focusDistance);

	const Int           rowCount    = size.y;
	const UInt threadCount = max(static_cast<UInt>(1), static_cast<UInt>(jobSystem.numThreads()));
	std::atomic<UInt>   completedRows(0);
	const TraceContext  ctx{
		&scene,
		&camera,
		&colorAccumulation,
		&sampleCounts,
		&completedRows,
		size,
		seed,
		maxPathDepth,
		samplesPerFrame};

	const Int rowsPerJob = max(1, rowCount / static_cast<Int>(threadCount * 2));
	for (Int row = 0; row < rowCount; row += rowsPerJob)
	{
		const Int endRow = min(row + rowsPerJob, rowCount);
		jobSystem.enqueue([ctx, row, endRow]() { traceRows(ctx, row, endRow); });
	}

	while (completedRows.load(std::memory_order_relaxed) < static_cast<UInt>((rowCount + rowsPerJob - 1) / rowsPerJob))
	{
		std::this_thread::yield();
	}

	frameSamples += static_cast<UInt>(samplesPerFrame);
}

void RayTracerRenderer::uploadBuffer(Renderer& renderer, const IVec2& size)
{
	if (colorAccumulation.empty()) return;

	Vector<Vec3> displayBuffer;
	displayBuffer.resize(colorAccumulation.size());

	for (SizeT i = 0; i < colorAccumulation.size(); ++i)
		displayBuffer[i] = displayColor(colorAccumulation[i], sampleCounts[i]);

	const Memory pixelMemory(displayBuffer.data(), displayBuffer.size() * sizeof(Vec3));
	const Image  image(size.x, size.y, Channels::RGB, ImageDataType::Float, pixelMemory);

	CpuTexture cpuTexture;
	cpuTexture.load(image, Filter::Linear, Filter::Linear, Wrapping::Clamped, textureName);
	displayTexture.load(renderer, cpuTexture);
}

void RayTracerRenderer::drawOverlay(Renderer& renderer)
{
	renderer.screenSpaceFromBottomLeft();
	renderer.setDepthTest(false);
	renderer.setCulling(false);
	renderer.uniforms().setMMatrix(Matrix(1));
	renderer.fontSize(16.0f);
	renderer.letterSpacing(0.6f);
	renderer.textRenderSystem.alignText(Alignment::left, Alignment::bottom);

	constexpr Float textSpacing = 20.0f;
	Int             line        = 1;
	renderer.text("Cornell Box — CPU path tracer", Vec2(textSpacing, textSpacing * line++));
	renderer.text("Samples: " + toString(frameSamples), Vec2(textSpacing, textSpacing * line++));
	renderer.text("LMB capture mouse · R reset · D depth of field", Vec2(textSpacing, textSpacing * line++));
	renderer.text("WASD QE move · scroll speed · mouse look", Vec2(textSpacing, textSpacing * line++));
}

void RayTracerRenderer::create(Window& window)
{
	GameRenderer::create(window);
	setName("RayTracerRenderer");
	player.camera.setPosition(Vec3(0.0f, -0.15f, 1.02f));
	player.camera.setRotation(0.0, 0.0, 0.0);
	markSceneDirty();
}

void RayTracerRenderer::destroy(Window& window)
{
	displayTexture.unload(window.renderer);
	GameRenderer::destroy(window);
}

void RayTracerRenderer::update(Window& window)
{
	const Vec3  previousPosition = player.camera.getPosition();
	const DVec3 previousRotation = player.camera.getRotation();
	player.update(window);

	if (player.camera.getPosition() != previousPosition || player.camera.getRotation() != previousRotation) markSceneDirty();
}

void RayTracerRenderer::renderScene(Window& window)
{
	Renderer& r        = window.renderer;
	const IVec2 fbSize = framebuffer.getSize();
	if (fbSize.x <= 0 || fbSize.y <= 0) return;

	const UInt seed = static_cast<UInt>(r.time.getTotal() * 1000.0) + frameSamples;
	traceFrame(fbSize, seed);
	uploadBuffer(r, fbSize);

	framebuffer.clearColor(r, Color(0, 0, 0, 1));
	framebuffer.clearDepth(r);

	r.setCulling(false);
	r.setDepthTest(false);
	r.normalizedSpace();
	r.useShader("texture");
	r.uniforms().setCustomInt1(0);
	r.uniforms().setMMatrix(scale(Matrix(1), Vec3(2.0f, 2.0f, 1.0f)));

	displayTexture.useTextureInSlot(r, 0);
	r.plane();
}

void RayTracerRenderer::renderInteractive(Window& window)
{
	drawOverlay(window.renderer);
}

void RayTracerRenderer::connect(GameSimulation& simulation) {}

void RayTracerRenderer::inputEvent(Window& window, InputEvent& input)
{
	if (input == captureMouse) ui.capture(*this);
	if (input == releaseMouse) ui.releaseCapture();
	if (input == resetSamples) markSceneDirty();
	if (input == toggleDof)
	{
		enableDepthOfField = !enableDepthOfField;
		markSceneDirty();
	}

	player.inputEvent(window, input);
}

void RayTracerGame::run()
{
	Window& window = ui.window("Cornell Box Ray Tracer", settings.defaultWindowWidth, settings.defaultWindowHeight, true, true, WindowState::Windowed);
	window.addElement(renderer);
	ui.run();
}
