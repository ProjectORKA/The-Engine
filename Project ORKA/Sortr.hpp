#pragma once

#include "FileSystem.hpp"
#include "FileTypes.hpp"
#include "Game.hpp"

struct SortrResource
{
	CPUTexture cpuTexture;
	GPUTexture gpuTexture;
	Index      index    = 0;
	Float      priority = 0;
	Path       path     = Path("");

	void destroy();
	void calculateRating(Index currentImageIndex, UInt resourceCount);
};

struct SortrFolderElement
{
	Path       path = "";
	Float      size = 0.0;
	String     folderName;
	Vec2       position = Vec2(0, 0);
	InputEvent keyBind  = InputEvent(InputType::Unknown, UNKNOWN, true);
};

struct SortrFolderRing
{
	Vector<SortrFolderElement> elements;
	Bool                       shapeToggle = false;

	void add();
	void create();
	void updatePosition();
	void render(Renderer& renderer) const;
};

struct SortrRenderer final : GameRenderer
{
	SortrFolderRing       ring;
	Vector<SortrResource> images;
	TimePoint             lastButtonInput;
	TimePoint             lastImageRefresh;
	UInt                  cpuLoadedCount     = 0;
	UInt                  gpuLoadedCount     = 0;
	UInt                  cpuMaxPreloadCount = 2;
	UInt                  gpuMaxPreloadCount = 1;
	Int                   currentImageId     = 0;
	Int                   zoomLevel          = 0;
	Float                 holdDelay          = 1.0f;
	Float                 holdingDelay       = 0.5f;
	Float                 zoom               = 1.0f;
	Float                 zoomFactor         = 1.2f;
	Float                 smoothness         = 0.1f;
	Float                 frameRate          = 60.0f;
	Vec2                  offset             = Vec2(0.0f);
	Matrix                actualMatrix       = Matrix(1.0f);
	InputId               mouseDown          = InputId(InputType::Mouse, LMB);
	InputEvent            zoomIn             = InputEvent(InputType::Scroll, 1, true);
	InputEvent            zoomOut            = InputEvent(InputType::Scroll, 1, false);
	InputEvent            resetView          = InputEvent(InputType::Mouse, MMB, false);
	InputEvent            click              = InputEvent(InputType::Mouse, LMB, false);
	InputEvent            shapeToggler       = InputEvent(InputType::KeyBoard, O, true);
	InputEvent            deleteImage        = InputEvent(InputType::KeyBoard, DEL, false);
	InputEvent            left               = InputEvent(InputType::KeyBoard, LEFT, false);
	InputEvent            right              = InputEvent(InputType::KeyBoard, RIGHT, false);

	void updateZoom();
	void preloadImages();
	void showNextImage();
	void showPrevImage();
	void sortLeft() const;
	void loadGPUResource();
	void loadCPUResource();
	void sortRight() const;
	void calculatePriorities();
	void addSortrFolderElement();
	void update(Window& window) override;
	void destroy(Window& window) override;
	void loadCPUImageWithHighestPriority();
	void loadGPUImageWithHighestPriority();
	void unloadGPUImageWithLowestPriority();
	void unloadCPUImageWithLowestPriority();
	void connect(GameSimulation& simulation) override;
	void inputEvent(Window& window, InputEvent input) override;
	void preloadImage(const Path& path, GPUTexture& texture) const;
	void create(Window& window) override;
	void render(Window& window, TiledRectangle area) override;
	void renderInteractive(Window& window, TiledRectangle area) override;

	[[nodiscard]] Vector<Index> indicesOfImagesSortedByPriority() const;
	[[nodiscard]] Index         indexOfCPUImageWithLowestPriority() const;
	[[nodiscard]] Index         indexOfGPUImageWithLowestPriority() const;
	[[nodiscard]] Index         indexOfCPUImageWithHighestPriority() const;
	[[nodiscard]] Index         indexOfGPUImageWithHighestPriority() const;
};

struct Sortr
{
	UserInterface ui;
	Window        window;
	SortrRenderer renderer;
	Path          currentPath = getCurrentPath();
	Vector<Path>  filePaths;

	void run(const Int argc, Char* argv[])
	{
		ui.create();
		//file management
		// process parameters
		for(Int i = 0; i < argc; i++)
		{
			Path path = argv[i];
			if(isExecutableFile(path))
			{
				executablePath = getDirectory(path);
				setCurrentPath(executablePath);
			}
			else
			{
				if(isImageFile(path)) filePaths.push_back(path);
				else logWarning(String("Can't process input: ").append(path.string()));
			}
		}

		ui.window("Sortr", Area(1920, 1080), true, true, WindowState::Windowed, renderer);

		Vector<Path>& windowFilePaths = window.droppedFilePaths;
		for(auto& path : filePaths) if(isImageFile(path)) windowFilePaths.push_back(path);

		ui.run();
	}
};
