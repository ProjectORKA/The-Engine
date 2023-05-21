#include "MeshSystem.hpp"
#include "Uniforms.hpp"

void MeshSystem::create(ResourceManager& resourceManager)
{
	CpuMesh standard;
	standard.name = "default";
	proceduralPlaneMesh(standard, 1, 1);
	addMesh(standard);
	CpuMesh boundingBox;
	boundingBox.name = "boundingBox";
	proceduralWireframeAxisLines(boundingBox);
	addMesh(boundingBox);
	basicMeshes.create(resourceManager);
}

void MeshSystem::destroy()
{
	for(GpuMesh& gpuMesh : gpuMeshes)
	{
		gpuMesh.unload();
	}
	gpuMeshes.clear();
	basicMeshes.destroy();
}

void MeshSystem::use(const Index meshId)
{
	currentMeshId = meshId;
}

void MeshSystem::use(ResourceManager& resourceManager, const Name& meshName)
{
	Index id;
	if(meshNames.find(meshName, id))
	{
		currentMeshId = id;
	}
	else
	{
		CpuMesh mesh;
		mesh.load(resourceManager, meshName);
		addMesh(mesh);
		if(meshNames.find(meshName, id))
		{
			currentMeshId = id;
		}
		else
		{
			logError("Mesh could not be loaded!");
		}
	}
}

void MeshSystem::addMesh(const CpuMesh& cpuMesh)
{
	if(cpuMesh.loaded)
	{
		GpuMesh gpuMesh;
		gpuMesh.upload(cpuMesh);
		gpuMeshes.push_back(gpuMesh);
		use(toUIntSafe(gpuMeshes.size() - 1));
		meshNames.add(cpuMesh.name, currentMeshId);
	}
	else
	{
		logWarning("Mesh could not be loaded!");
	}
}

void MeshSystem::render(Uniforms& uniforms, const Index meshId)
{
	use(meshId);
	currentMesh().render(uniforms);
}

void MeshSystem::render(ResourceManager& resourceManager, Uniforms& uniforms, const Name& meshName)
{
	use(resourceManager, meshName);
	currentMesh().render(uniforms);
}

void MeshSystem::renderInstanced(ResourceManager& resourceManager, Uniforms& uniforms, const Name& meshName, Vector<Matrix>& data)
{
	use(resourceManager, meshName);
	if(! data.empty())
	{
		currentMesh().vao.select();
		if(currentMesh().vao.instanced)
		{
			transforms.update(value_ptr(data[0]), data.size() * sizeof(Matrix));
		}
		else
		{
			constexpr UInt transformPos = 4;
			if(! transforms.loaded)
			{
				transforms.create(4, value_ptr(data[0]), data.size() * sizeof(Matrix), MeshDrawMode::StaticMode, 4);
			}
			apiBindBuffer(GL_ARRAY_BUFFER, transforms.bufferId);
			apiEnableVertexAttributeArray(transformPos + 0);
			apiEnableVertexAttributeArray(transformPos + 1);
			apiEnableVertexAttributeArray(transformPos + 2);
			apiEnableVertexAttributeArray(transformPos + 3);
			apiVertexAttributePointer(transformPos + 0, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix), nullptr);
			apiVertexAttributePointer(transformPos + 1, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix), reinterpret_cast<void*>(1 * sizeof(Vec4)));
			apiVertexAttributePointer(transformPos + 2, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix), reinterpret_cast<void*>(2 * sizeof(Vec4)));
			apiVertexAttributePointer(transformPos + 3, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix), reinterpret_cast<void*>(3 * sizeof(Vec4)));
			apiVertexAttributeDivisor(transformPos + 0, 1);
			apiVertexAttributeDivisor(transformPos + 1, 1);
			apiVertexAttributeDivisor(transformPos + 2, 1);
			apiVertexAttributeDivisor(transformPos + 3, 1);
			currentMesh().vao.instanced = true;
		}
	}
	currentMesh().renderInstances(uniforms, data.size());
}

void MeshSystem::renderFullscreen(Uniforms& uniforms)
{
	basicMeshes.fullscreenMesh.render(uniforms);
}

GpuMesh& MeshSystem::currentMesh()
{
	return gpuMeshes[currentMeshId];
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