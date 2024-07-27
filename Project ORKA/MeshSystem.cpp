#include "MeshSystem.hpp"
#include "Uniforms.hpp"

void MeshSystem::create()
{
	CpuMesh standard;
	standard.name = "default";
	proceduralPlaneMesh(standard, 1, 1);
	addMesh(standard);

	CpuMesh boundingBox;
	boundingBox.name = "boundingBox";
	proceduralWireframeAxisLines(boundingBox);
	addMesh(boundingBox);
	basicMeshes.create();
}

void BasicMeshes::create()
{
	CpuMesh fullScreenTriangleMesh;
	fullScreenTriangleMesh.drawMode           = BufferUsage::StaticDraw;
	fullScreenTriangleMesh.name               = "FullScreenTriangle";
	fullScreenTriangleMesh.primitiveMode      = PrimitiveMode::Triangles;
	fullScreenTriangleMesh.positions          = {Vec3(-1, -1, 0), Vec3(5, -1, 0), Vec3(-1, 5, 0)};
	fullScreenTriangleMesh.textureCoordinates = {Vec2(0, 0), Vec2(3, 0), Vec2(0, 3)};
	fullScreenTriangleMesh.indices            = {0, 1, 2};
	fullScreenTriangleMesh.checkIntegrity();
	fullscreenMesh.upload(fullScreenTriangleMesh);
}

void MeshSystem::destroy()
{
	for(GpuMesh& gpuMesh : gpuMeshes) gpuMesh.unload();
	gpuMeshes.clear();
	basicMeshes.destroy();
}

void BasicMeshes::destroy()
{
	fullscreenMesh.unload();
}

GpuMesh& MeshSystem::currentMesh()
{
	return gpuMeshes[currentMeshId];
}

void MeshSystem::use(const Index meshId)
{
	currentMeshId = meshId;
}

void MeshSystem::addMesh(const CpuMesh& cpuMesh)
{
	if(cpuMesh.isLoaded())
	{
		gpuMeshes.emplace_back();
		gpuMeshes.back().upload(cpuMesh);
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

void MeshSystem::use(const Name& name)
{
	Index id;
	if(meshNames.find(name, id)) currentMeshId = id;
	else
	{
		CpuMesh mesh;
		mesh.load(name);
		addMesh(mesh);
		if(meshNames.find(name, id)) currentMeshId = id;
		else logError("Mesh could not be loaded! (" + toString(name) + ")");
	}
}

void MeshSystem::render(Uniforms& uniforms, const Name& meshName)
{
	use(meshName);
	currentMesh().render(uniforms);
}

void MeshSystem::renderInstanced(Uniforms& uniforms, const Name& meshName, const Vector<Matrix>& transforms)
{
	use(meshName);
	currentMesh().renderInstances(uniforms, transforms);
}
