
#pragma once

#include "Game.hpp"

struct ImageViewer : public GameRenderer{
	Path imagePath = "";
	CPUTexture cimage;
	GPUTexture gimage;
	
	Int zoomLevel = 0;
	Float zoom = 1;
	Float zoomfactor = 1.2;
	Float smoothness = 10;

	Vec2 offset = Vec2(0);

	Float actualzoom = 1;
	Vec2 actualOffset = Vec2(0);

	InputEvent zoomIn = InputEvent(InputType::Scroll, 1, 1);
	InputEvent zoomOut = InputEvent(InputType::Scroll, 1, 0);

	InputID mouseDown = InputID(InputType::Mouse, LMB);

	void showImage(Path path) {
		cimage.load(path, "imageViewerImage");
		cimage.nearFilter = Filter::nearest;
	}
	
	void update(Window& window) override{
		if (window.droppedFilePaths.size()) {
			cimage.unload();
			cimage.load(window.droppedFilePaths.last(), "imageViewerImage");
			cimage.nearFilter = Filter::nearest;
			gimage.load(cimage);
			window.droppedFilePaths.popBack();
		}
	}

	void render(Engine& engine, Window& window, TiledRectangle area) override {
		Renderer& renderer = window.renderer;

		renderer.clearColor(Color(0,0,0,1));

		if (window.pressed(mouseDown)) {
			offset += window.mouseDelta;
		}

		gimage.use(0);
		renderer.useShader(engine, "imageViewer");
		renderer.normalizedScreenSpace();

		//calculate image size and position
		Float winX = window.renderer.framebufferSystem.windowSize.x;
		Float winY = window.renderer.framebufferSystem.windowSize.y;

		Float imgX = cimage.width;
		Float imgY = cimage.height;

		Float aspW = winX / winY;
		Float aspI = imgX / imgY;

		Float targetX = 0;
		Float targetY = 0;

		if (aspW >= aspI) {
			targetX = winY * aspI;
			targetY = winY;
		}
		else {
			targetX = winX;
			targetY = winX * (1/aspI);
		}

		approach(actualzoom, zoom, window.renderer.deltaTime() * smoothness);
		actualOffset = offset;


		Matrix centerMatrix = translate(Matrix(1), Vec3(winX / 2, winY / 2,0));
		Matrix zoomMatrix = scale(Matrix(1), Vec3(actualzoom));
		Matrix offsetMatrix = translate(Matrix(1),Vec3(offset,0));
		Matrix imageScale = scale(Matrix(1), Vec3(targetX, targetY, 0));



		Matrix finalMatrix = offsetMatrix * zoomMatrix * imageScale * centerMatrix;
		finalMatrix = imageScale * centerMatrix;

		renderer.uniforms().mMatrix(finalMatrix);

		renderer.renderMesh(engine, "centeredPlane");
	};

	void inputEvent(Window& window, InputEvent input) override {
		if (input == zoomIn) {
			zoomLevel++;
			
			Float oldZoom = zoom;

			zoom = pow(zoomfactor, zoomLevel);

			offset *= zoom / oldZoom;

		}
		if (input == zoomOut) {
			zoomLevel--;

			Float oldZoom = zoom;

			zoom = pow(zoomfactor, zoomLevel);

			offset *= zoom / oldZoom;
		};
	};
};