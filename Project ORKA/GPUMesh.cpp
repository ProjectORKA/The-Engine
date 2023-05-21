#include "GPUMesh.hpp"
#include "Uniforms.hpp"

void GpuMesh::unload()
{
	//make unavailable for rendering
	if(loaded)
	{
		vao.unload();
		loaded = false;
	}
}

void GpuMesh::upload(CpuMesh cpuMesh)
{
	if(! loaded)
	{
		if(cpuMesh.loaded)
		{
			if(! cpuMesh.positions.empty())
			{
				primitiveMode = cpuMesh.primitiveMode;

				vao.create(cpuMesh);

				loaded = true;
			}
		}
		else
		{
			logDebug("CpuMesh not loaded! Cant upload!");
		}
	}
	else
	{
		logDebug("GPUMesh already loaded!");
	}
}

void GpuMesh::render(Uniforms& uniforms) const
{
	if(loaded)
	{
		uniforms.instanced(false);
		uniforms.upload();
		vao.select();
		apiDrawElements(primitiveMode, vao.indexBuffer.indexCount, nullptr);
	}
}

void GpuMesh::renderInstances(Uniforms& uniforms, const UInt instanceCount) const
{
	if(loaded)
	{
		uniforms.instanced(true);
		uniforms.upload();
		vao.select();
		apiDrawElementsInstanced(primitiveMode, vao.indexBuffer.indexCount, nullptr, instanceCount);
	}
}