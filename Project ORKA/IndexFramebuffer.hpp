
#pragma once

#include "Settings.hpp"
#include "Basics.hpp"
#include "TiledMath.hpp"
#include "GPUTexture.hpp"

struct PixelIDs {
	UInt objectID = 0.0f;
	UInt instanceID = 0.0f;
	UInt primitiveID = 0.0f;
};

struct IDFrameBuffer {
	Area size = Area(100, 100);

	Index framebufferID = 0;

	GPUTexture idTexture;
	GPUTexture depthTexture;

	void use();
	void destroy();
	Float aspectRatio();
	void create(Area size);
	void resize(Area resolution);
	PixelIDs getIDsAtCenter()
	{
		return getIDsAtLocation(size.x / 2 , size.y / 2);
	}
	PixelIDs getIDsAtLocation(UInt x, UInt y)
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, framebufferID);
		glReadBuffer(GL_COLOR_ATTACHMENT0);

		PixelIDs data;
		glReadPixels(x, y, 1, 1, GL_RGB_INTEGER, GL_UNSIGNED_INT, &data);

		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

		return data;
	}
};