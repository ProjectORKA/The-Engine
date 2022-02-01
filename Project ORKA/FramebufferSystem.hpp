
#pragma once

#include "Framebuffer.hpp"
#include "IndexFramebuffer.hpp"

struct Renderer;

struct FramebufferSystem {
	Area framebufferSize;
	Vector<Framebuffer *> framebuffers;
	Map<Name, Index> framebufferNames;
	Index currentFramebufferIndex = 0;

	IDFrameBuffer * idFbPtr = nullptr;

	void destroy();
	void deselect();
	void update(Area area);
	Framebuffer& current();
	void add(Framebuffer * framebuffer);
	void create(Renderer& renderer, Area size) {
		framebuffers.push_back(new GBuffer());
		
		IDFrameBuffer* pointer = new IDFrameBuffer();
		idFbPtr = pointer;
		framebuffers.push_back(pointer);
		idFramebuffer().add(4, dataTypeUInt, 0);
		idFramebuffer().add(5, dataTypeFloat, 1);
	};
	IDFrameBuffer& idFramebuffer() {
		return *idFbPtr;
	};
	void use(Renderer& renderer, Index framebufferIndex);
};