#include "MeshSystem.hpp"
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
	for(GpuMesh& gpuMesh : gpuMeshes) gpuMesh.unload(renderer);
	gpuMeshes.clear();
	basicMeshes.destroy(renderer);
}

void BasicMeshes::destroy(Renderer& renderer)
{
	fullscreenMesh.unload(renderer);
}

GpuMesh& MeshSystem::currentMesh()
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
	else
	{
		logWarning("Mesh could not be loaded! (" + toString(cpuMesh.name) + ")");
	}
}

void NameTable::add(const Name& name, const Index id)
{
	names.push_back(name);
	indices.push_back(id);
}

Bool NameTable::find(const Name& name, Index& id) const
{
	id = -1;
	for(Index i = 0; i < names.size(); i++)
	{
		if(names[i] == name)
		{
			id = indices[i];
			return true;
		}
	}
	return false;
}

void MeshSystem::renderFullscreen(Uniforms& uniforms) const
{
	basicMeshes.fullscreenMesh.render(uniforms);
}

void MeshSystem::render(Uniforms& uniforms, const Index meshId)
{
	use(meshId);
	currentMesh().render(uniforms);
}

void MeshSystem::use(Renderer& renderer, const Name& name)
{
	Index id;
	if(meshNames.find(name, id))
	{
		currentMeshId = id;
	}
	else
	{
		CpuMesh mesh;
		mesh.load(name);
		addMesh(renderer, mesh);
		if(meshNames.find(name, id))
		{
			currentMeshId = id;
		}
		else
		{
			logError("Mesh could not be loaded! (" + toString(name) + ")");
		}
	}
}

void MeshSystem::render(Renderer& renderer, Uniforms& uniforms, const Name& meshName)
{
	use(renderer, meshName);
	currentMesh().render(uniforms);
}

void MeshSystem::renderInstanced(Renderer& renderer, Uniforms& uniforms, const Name& meshName, const Vector<Matrix>& transforms)
{
	use(renderer, meshName);
	currentMesh().renderInstances(uniforms, transforms);
}
