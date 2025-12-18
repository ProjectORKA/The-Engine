#pragma once

#include "FileSystem.hpp"
#include "Game.hpp"
#include "ImageViewerResource.hpp"

struct SortrFolderElement
{
	Path       path = "";
	Float      size = 0.0;
	String     folderName;
	Vec2       position = Vec2(0, 0);
	InputEvent keyBind  = InputEvent(InputType::Unknown, Unknown, true);

	void setPath(const Path& path);
};

struct SortrFolderRing
{
	Vector<SortrFolderElement> elements;
	Bool                       shapeToggle = false;

	void add();
	void create();
	void updatePosition();
	void render(Renderer& renderer) const;
	void renderInteractive(Renderer& r) const;
};

struct SortrRenderer final : GameRenderer
{
	SortrFolderRing             ring;
	Vector<ImageViewerResource> images;
	TimePoint                   lastButtonInput;
	TimePoint                   lastImageRefresh;
	UInt                        cpuLoadedCount     = 0;
	UInt                        gpuLoadedCount     = 0;
	UInt                        cpuMaxPreloadCount = 2;
	UInt                        gpuMaxPreloadCount = 1;
	Int                         currentImageId     = 0;
	Float                       holdDelay          = 1.0f;
	Float                       holdingDelay       = 0.5f;
	Float                       frameRate          = 60.0f;
	Matrix                      actualMatrix       = Matrix(1.0f);
	InputEvent                  click              = InputEvent(InputType::Mouse, Lmb, false);
	InputEvent                  shapeToggler       = InputEvent(InputType::KeyBoard, O, true);
	InputEvent                  deleteImage        = InputEvent(InputType::KeyBoard, Del, false);
	InputEvent                  left               = InputEvent(InputType::KeyBoard, Left, false);
	InputEvent                  right              = InputEvent(InputType::KeyBoard, Right, false);

	void showNextImage();
	void showPrevImage();
	void calculatePriorities();
	void addSortrFolderElement();
	void update(Window& window) override;
	void create(Window& window) override;
	void destroy(Window& window) override;
	void preloadImages(Renderer& renderer);
	void loadCpuImageWithHighestPriority();
	void unloadCpuImageWithLowestPriority();
	void connect(GameSimulation& simulation) override;
	void loadGpuImageWithHighestPriority(Renderer& renderer);
	void unloadGpuImageWithLowestPriority(Renderer& renderer);
	void renderScene(Window& window) override;
	void inputEvent(Window& window, InputEvent input) override;
	void renderInteractive(Window& window) override;
	void preloadImage(Renderer& renderer, const Path& path, GPUTexture& texture) const;

	[[nodiscard]] Vector<Index> indicesOfImagesSortedByPriority() const;
	[[nodiscard]] Index         indexOfCpuImageWithLowestPriority() const;
	[[nodiscard]] Index         indexOfGpuImageWithLowestPriority() const;
	[[nodiscard]] Index         indexOfCpuImageWithHighestPriority() const;
	[[nodiscard]] Index         indexOfGpuImageWithHighestPriority() const;
};

struct Sortr
{
	SortrRenderer renderer;
	Vector<Path>  filePaths;
	Path          currentPath = getCurrentPath();

	void run(Int argc, Char* argv[]);
};
