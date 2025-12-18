#include "MeshSystem.hpp"

#include "Renderer.hpp"
#include "Uniforms.hpp"

void MeshSystem::create(Renderer& renderer)
{
	CpuMesh standard;
	standard.name = "default";
	proceduralPlaneMesh(standard, 1, 1);
	addMesh(renderer, standard);

	CpuMesh boundingBox;
	boundingBox.name = "boundingBox";
	proceduralWireframeAxisLines(boundingBox);
	addMesh(renderer, boundingBox);
	basicMeshes.create(renderer);
}

void BasicMeshes::create(Renderer& renderer)
{
	CpuMesh fullScreenTriangleMesh;
	fullScreenTriangleMesh.drawMode           = BufferUsage::StaticDraw;
	fullScreenTriangleMesh.name               = "FullScreenTriangle";
	fullScreenTriangleMesh.primitiveMode      = PrimitiveMode::Triangles;
	fullScreenTriangleMesh.positions          = {Vec3(-1, -1, 0), Vec3(5, -1, 0), Vec3(-1, 5, 0)};
	fullScreenTriangleMesh.textureCoordinates = {Vec2(0, 0), Vec2(3, 0), Vec2(0, 3)};
	fullScreenTriangleMesh.indices            = {0, 1, 2};
	fullScreenTriangleMesh.checkIntegrity();
	fullscreenMesh.upload(renderer, fullScreenTriangleMesh);
}

void MeshSystem::destroy(Renderer& renderer)
{
	for(GPUMesh& gpuMesh : gpuMeshes) gpuMesh.unload(renderer);
	gpuMeshes.clear();
	basicMeshes.destroy(renderer);
}

void BasicMeshes::destroy(Renderer& renderer)
{
	fullscreenMesh.unload(renderer);
}

GPUMesh& MeshSystem::currentMesh()
{
	return gpuMeshes[currentMeshId];
}

void MeshSystem::use(const Index meshId)
{
	currentMeshId = meshId;
}

void MeshSystem::addMesh(Renderer& renderer, const CpuMesh& cpuMesh)
{
	if(cpuMesh.isLoaded())
	{
		gpuMeshes.emplace_back();
		gpuMeshes.back().upload(renderer, cpuMesh);
		use(toUIntSafe(gpuMeshes.size() - 1));
		meshNames.add(cpuMesh.name, currentMeshId);
	}
	else logWarning("Mesh could not be loaded! (" + toString(cpuMesh.name) + ")");
}

void NameTable::add(const Name& name, const Index id, const Bool failedFlag)
{
	names.push_back(name);
	indices.push_back(id);
	failed.push_back(failedFlag);
}

Bool NameTable::find(const Name& name, Index& id, Bool& failedFlag) const
{
	id = -1;
	failedFlag = false;
	for(Index i = 0; i < names.size(); i++)
	{
		if(names[i] == name)
		{
			id = indices[i];
			failedFlag = failed[i];
			return true;
		}
	}
	return false;
}

void NameTable::markFailed(const Name& name)
{
	for(Index i = 0; i < names.size(); i++)
	{
		if(names[i] == name)
		{
			failed[i] = true;
			return;
		}
	}
	add(name, -1, true);
}

void MeshSystem::renderFullscreen(Renderer& renderer) const
{
	basicMeshes.fullscreenMesh.render(renderer);
}

void MeshSystem::render(Renderer& renderer, const Index meshId)
{
	use(meshId);
	currentMesh().render(renderer);
}

Bool MeshSystem::use(Renderer& renderer, const Name& name)
{
	Index id;
	Bool  failed = false;
	if(meshNames.find(name, id, failed))
	{
		if(failed) return false;
		currentMeshId = id;
		return true;
	}

	CpuMesh mesh;
	mesh.load(name);
	if(!mesh.isLoaded())
	{
		logWarning("Mesh could not be loaded! (" + toString(name) + "), skipping render.");
		meshNames.markFailed(name);
		return false;
	}

	addMesh(renderer, mesh);
	if(meshNames.find(name, id, failed) && !failed)
	{
		currentMeshId = id;
		return true;
	}

	logWarning("Mesh could not be loaded! (" + toString(name) + "), skipping render.");
	meshNames.markFailed(name);
	return false;
}

void MeshSystem::render(Renderer& renderer, const Name& meshName)
{
	if (!use(renderer, meshName)) return;
	currentMesh().render(renderer);
}

void MeshSystem::renderInstanced(Renderer& renderer, const Name& meshName, const Vector<Matrix>& transforms)
{
	if (!use(renderer, meshName)) return;
	currentMesh().renderInstances(renderer, transforms);
}
