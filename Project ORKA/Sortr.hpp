#pragma once

#include "Game.hpp"
#include "ResourceManager.hpp"

struct SortrResource {
	Path path = Path("");
	Float priority = 0;
	Index index = 0;
	CPUTexture cpuTexture;
	GpuTexture gpuTexture;

	void destroy();
	void calculateRating(Index currentImageIndex, UInt resourceCount);
};

// [TODO] add to filesystem after deleteFile() is fixed
//inline void moveFile(Path& source, Path destination) {
//	copyFile(source, destination);
//	UInt sourceFileSize = getFileSize(source);
//	String fileName = getFileName(source);
//	Path destFile = destination.string() + fileName;
//	UInt destFileSize = getFileSize(destFile);
//	logDebug(destFile);
//	logDebug(sourceFileSize);
//	logDebug(destFileSize);
//	
//	Bool fileValid = doesPathExist(destination);
//
//	//comp sizes
//	if (fileValid && sourceFileSize == destFileSize) {
//		//deleteFile(source); // [TODO] add back when recycle bin is fixed
//		std::filesystem::remove(source);
//	}
//	else {
//		logError("File size corrupted during moving, doesn't match!");
//	};
//};

struct SortrFolderElement {
	Path path = "";
	String folderName;
	InputEvent keybind;
	Vec2 position = Vec2(0,0);
	Float size = 0.0;
	SortrFolderElement() {};
};



struct SortrFolderRing {

	Vector<SortrFolderElement> elements;
	Bool shapeToggle = false;


	void render(Renderer& renderer, ResourceManager& resourceManager) {
		for (int i = 0; i < elements.size(); i++) {
			renderer.uniforms().mMatrix() = matrixFromLocationAndSize(elements[i].position, elements[i].size);//mMatrix(Matrix(1));
			renderer.fill(Color(0.2, 0.2, 0.65, 1));
			renderer.aspectCorrectNormalizedSpace();
			renderer.useShader(resourceManager, "color");
			renderer.renderMesh(resourceManager, "centeredPlane");
			renderer.fill(Color(0, 0.5, 0.5, 1));
			renderer.textRenderSystem.alignText(Alignment::middle, Alignment::top);
			fonts.paragraph.setSize(0.3);
			renderer.textRenderSystem.setStyle(fonts.paragraph);
			renderer.textRenderSystem.render(resourceManager, renderer, String("image"), Vec2(0,0));
		};

	};

	void add() {
		SortrFolderElement tmp;
		elements.push_back(tmp);
		updatePosition();
	};

	void updatePosition() {

		for (int i = 0; i < elements.size(); i++) {
			Float angle = 2*PI*Float(i)/Float(elements.size());
			Float radius = 0.8;
			Vec2 position = Vec2(sin(angle), cos(angle));
			Float maxDist = max(abs(position.x), abs(position.y));
			if (shapeToggle) position /= maxDist;
			position *= radius;
			elements[i].position = position;
			////////////////////////////////////////////////////////
			elements[i].size = 3.0f / Float(elements.size());
		};

	};

	SortrFolderRing() {
		for (int i = 0; i < 26; i++) {
			add();
		}
	};
};

struct SortrRenderer : public GameRenderer {
	SortrFolderRing ring;
	Vector<SortrResource> images;
	UInt cpuLoadedCount = 0;
	UInt gpuLoadedCount = 0;
	UInt cpuMaxPreloadCount = 2;
	UInt gpuMaxPreloadCount = 1;
	Int currentImageId = 0;
	Float frameRate = 60;
	Float holdingDelay = 0.5;
	Int zoomLevel = 0;
	Float zoom = 1;
	Float zoomfactor = 1.2;
	Float smoothness = 0.1;
	TimePoint lastImageRefresh;
	TimePoint lastButtonInput;
	Vec2 offset = Vec2(0);
	Matrix actualMatrix = Matrix(1);
	Float holdDelay = 1;
	InputEvent shapeToggler = InputEvent(InputType::KeyBoard, O, true);
	InputEvent zoomIn = InputEvent(InputType::Scroll, 1, true);
	InputEvent zoomOut = InputEvent(InputType::Scroll, 1, false);
	InputEvent resetView = InputEvent(InputType::Mouse, MMB, false);
	InputEvent deleteImage = InputEvent(InputType::KeyBoard, DEL, false);
	InputEvent left = InputEvent(InputType::KeyBoard, LEFT, false);
	InputEvent right = InputEvent(InputType::KeyBoard, RIGHT, false);
	InputId mouseDown = InputId(InputType::Mouse, LMB);

	void sortLeft() {
		//moveFile(images[currentImageId].path, "C:\\Users\\15739\\Downloads\\testfold\\testfold2\\");
	};
	void sortRight() {
	};

	void updateZoom();
	void preloadImages();
	void showNextImage();
	void showPrevImage();
	void loadGPUResource();
	void loadCPUResource();
	void calculatePriorities();
	void update(Window& window) override;
	void loadCPUImageWithHighestPriority();
	void loadGPUImageWithHighestPriority();
	void unloadGPUImageWithLowestPriority();
	void unloadCPUImageWithLowestPriority();
	Index indexOfCPUImageWithLowestPriority() const;
	Index indexOfGPUImageWithLowestPriority() const;
	Index indexOfCPUImageWithHighestPriority() const;
	Index indexOfGPUImageWithHighestPriority() const;
	void preloadImage(const Path& path, GpuTexture& texture);
	Vector<Index> indicesOfImagesSortedByPriority() const;
	void inputEvent(Window& window, InputEvent input) override;
	SortrRenderer(){};
	void render(ResourceManager& resourceManager, Window& window, TiledRectangle area) override;
};






struct Sortr {
	ResourceManager resourceManager;
	UserInterface ui;
	SortrRenderer renderer;

	Sortr(const Int argc, Char* argv[]);
};

