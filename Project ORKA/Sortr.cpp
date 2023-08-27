#include "Sortr.hpp"
#include "Device.hpp"
#include "FileTypes.hpp"

void SortrFolderRing::add()
{
	const SortrFolderElement tmp;
	elements.push_back(tmp);
	updatePosition();
}

void SortrResource::destroy()
{
	cpuTexture.unload();
	gpuTexture.unload();
}

void SortrFolderRing::create()
{
	add();
	add();
	add();
	add();
}

void SortrRenderer::sortRight() const { }

void SortrRenderer::updateZoom()
{
	const Float oldZoom = zoom;
	zoom                = powf(zoomFactor, static_cast<Float>(zoomLevel));
	offset *= zoom / oldZoom;
}

void SortrRenderer::showNextImage()
{
	const auto currentTime = now();
	if(lastImageRefresh + Milliseconds(static_cast<Int>(1000 / frameRate)) < currentTime)
	{
		lastImageRefresh = currentTime;
		currentImageId++;
		if(currentImageId >= static_cast<Int>(images.size())) currentImageId = 0;
	}
}

void SortrRenderer::showPrevImage()
{
	const auto currentTime = now();
	if(lastImageRefresh + Milliseconds(static_cast<Int>(1000 / frameRate)) < currentTime)
	{
		lastImageRefresh = currentTime;
		currentImageId--;
		if(currentImageId < 0) currentImageId = static_cast<Int>(images.size() - 1);
	}
}

void SortrRenderer::preloadImages()
{
	//load resources
	calculatePriorities();

	//if there are images
	if(!images.empty())
	{
		//get vector of indices sorted by priority
		Vector<Index> sortedIndices = indicesOfImagesSortedByPriority();

		//make sure were not loading more than we can
		if(sortedIndices.size() > cpuMaxPreloadCount) sortedIndices.resize(cpuMaxPreloadCount);

		//load images
		for(UInt order = 0; order < sortedIndices.size(); order++)
		{
			//get the image index by sorted order
			const Index currentImageIndex = sortedIndices[order];

			//load all images within the preload range but unload the rest
			if(order <= cpuMaxPreloadCount)
			{
				//load cpu image
				if(!images[currentImageIndex].cpuTexture.isLoaded())
				{
					images[currentImageIndex].cpuTexture.load(images[currentImageIndex].path, "SortrImage", Filter::Nearest, Filter::Linear, Wrapping::Clamped);
					cpuLoadedCount++;
					//logDebug(cpuLoadedCount);
				}

				if(order < gpuMaxPreloadCount)
				{
					//load gpu image
					if(!images[currentImageIndex].gpuTexture.isLoaded())
					{
						images[currentImageIndex].gpuTexture.load(images[currentImageIndex].cpuTexture);
						gpuLoadedCount++;
						//logDebug(gpuLoadedCount);
					}
				}
				else
				{
					//unload gpu image
					if(images[currentImageIndex].gpuTexture.isLoaded())
					{
						images[currentImageIndex].gpuTexture.unload();
						gpuLoadedCount--;
						//logDebug(gpuLoadedCount);
					}
				}
			}
			else
			{
				//unload cpu image
				if(images[currentImageIndex].cpuTexture.isLoaded())
				{
					images[currentImageIndex].cpuTexture.unload();
					cpuLoadedCount--;
					//logDebug(cpuLoadedCount);
				}

				//unload gpu image just to be safe
				if(images[currentImageIndex].gpuTexture.isLoaded())
				{
					images[currentImageIndex].gpuTexture.unload();
					gpuLoadedCount--;
					//logDebug(gpuLoadedCount);
				}
			}
		}
	}

	if(cpuLoadedCount >= cpuMaxPreloadCount - 1)
	{
		if(cpuMaxPreloadCount < images.size())
		{
			if(availablePhysicalMemory() > static_cast<ULL>(512 * 1024 * 1024))
			{
				cpuMaxPreloadCount++;
				gpuMaxPreloadCount = cpuMaxPreloadCount;
			}
		}
	}
}

void SortrRenderer::sortLeft() const
{
	//moveFile(images[currentImageId].path, "C:\\Users\\15739\\Downloads\\testfold\\testfold2\\");
}

void SortrRenderer::destroy(Window& window) {}

void SortrRenderer::loadGPUResource()
{
	if(!images.empty())
	{
		//if max loaded image count is reached
		if(gpuLoadedCount >= gpuMaxPreloadCount) unloadGPUImageWithLowestPriority();
		//if there are images available to load into ram and we still have space
		if(gpuLoadedCount <= min(gpuMaxPreloadCount, images.size())) loadGPUImageWithHighestPriority();
	}
}

void SortrRenderer::loadCPUResource()
{
	if(!images.empty())
	{
		//if max loaded image count is reached
		if(cpuLoadedCount >= cpuMaxPreloadCount) unloadCPUImageWithLowestPriority();
		//if there are images available to load into ram and we still have space
		if(cpuLoadedCount <= min(cpuMaxPreloadCount, images.size())) loadCPUImageWithHighestPriority();
	}
}

void SortrFolderRing::updatePosition()
{
	for(int i = 0; i < elements.size(); i++)
	{
		const Float angle    = 2 * PI * Float(i) / Float(elements.size());
		const Float radius   = 0.8f;
		Vec2        position = Vec2(sin(angle), cos(angle));
		const Float maxDist  = max(abs(position.x), abs(position.y));
		if(shapeToggle) position /= maxDist;
		position *= radius;
		elements[i].position = position;
		////////////////////////////////////////////////////////
		elements[i].size = 3.0f / Float(elements.size());
	};
}

void SortrRenderer::calculatePriorities()
{
	for(auto& image : images) image.calculateRating(currentImageId, (UInt)images.size());
}

void SortrRenderer::update(Window& window)
{
	//load new images if dropped
	if(!window.droppedFilePaths.empty())
	{
		//image path to load folder from
		Path imagePath = "";

		//check for new paths
		for(auto p : window.droppedFilePaths)
		{
			//get last path that is actually an image
			if(isImageFile(p)) imagePath = p;
		}

		//clear queue
		window.droppedFilePaths.clear();

		//only if there is a valid image should we reload resources
		if(imagePath != "")
		{
			//delete currently loaded resource
			for(auto i : images) i.destroy();
			images.clear();

			//get folder
			Path folderPath = getDirectory(imagePath);

			//get all images in folder
			Vector<Path> paths = getFilesInDirectory(folderPath, imageExtensions);

			//set current index to the originally loaded image
			for(Int i = 0; i < static_cast<Int>(paths.size()); i++)
			{
				if(paths[i] == imagePath)
				{
					currentImageId = i;
					break;
				}
			}

			//set images to new paths
			for(Index i = 0; i < paths.size(); i++)
			{
				SortrResource image;
				image.path  = paths[i];
				image.index = i;
				images.push_back(image);
			}
		}
	}

	//input
	if(window.pressed(mouseDown)) offset += window.mouseDelta;
}

void SortrRenderer::addSortrFolderElement()
{
	ring.add();
}

void Sortr::run(const Int argc, Char* argv[])
{
	//file management
	Path         currentPath = getCurrentPath();
	Vector<Path> filePaths;

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

	resourceManager.create();
	ui.create();
	Vector<Path>& windowFilePaths = ui.window("Sortr", Area(1920, 1080), true, true, WindowState::Fullscreen, renderer, resourceManager).droppedFilePaths;
	for(auto& path : filePaths) if(isImageFile(path)) windowFilePaths.push_back(path);
	ui.run();
}

void SortrRenderer::loadGPUImageWithHighestPriority()
{
	if(!images.empty())
	{
		const Index imageId = indexOfGPUImageWithHighestPriority();
		images[imageId].gpuTexture.load(images[imageId].cpuTexture);
		if(images[imageId].gpuTexture.isLoaded()) gpuLoadedCount++;
		else logError("Image could not be loaded!");
	}
}

void SortrRenderer::loadCPUImageWithHighestPriority()
{
	if(!images.empty())
	{
		const Index imageId = indexOfCPUImageWithHighestPriority();
		images[imageId].cpuTexture.load(images[imageId].path, "SortrImage", Filter::Nearest, Filter::Linear, Wrapping::Clamped);
		if(images[imageId].cpuTexture.isLoaded()) cpuLoadedCount++;
		else logError("Image could not be loaded!");
	}
}

void SortrRenderer::unloadGPUImageWithLowestPriority()
{
	if(!images.empty())
	{
		const Index imageId = indexOfGPUImageWithLowestPriority();
		images[imageId].gpuTexture.unload();
		if(!images[imageId].gpuTexture.isLoaded()) gpuLoadedCount--;
		else logError("Image could not be unloaded!");
	}
}

void SortrRenderer::unloadCPUImageWithLowestPriority()
{
	if(!images.empty())
	{
		const Index imageId = indexOfCPUImageWithLowestPriority();
		images[imageId].cpuTexture.unload();
		if(!images[imageId].cpuTexture.isLoaded()) cpuLoadedCount--;
		else logError("Image could not be unloaded!");
	}
}

void SortrRenderer::connect(GameSimulation& simulation) {}

Index SortrRenderer::indexOfGPUImageWithLowestPriority() const
{
	if(images.empty()) logError("No images to get index from!");

	Float lowestPriority      = 0;
	Index lowestPriorityIndex = 0;
	for(Int i = 0; i < static_cast<Int>(images.size()); i++)
	{
		if(images[i].gpuTexture.isLoaded() && images[i].priority < lowestPriority)
		{
			lowestPriority      = images[i].priority;
			lowestPriorityIndex = i;
		}
	}
	return lowestPriorityIndex;
}

Index SortrRenderer::indexOfCPUImageWithLowestPriority() const
{
	if(images.empty()) logError("No images to get index from!");

	Float lowestPriority      = 0;
	Index lowestPriorityIndex = 0;

	for(Int i = 0; i < static_cast<Int>(images.size()); i++)
	{
		if(images[i].cpuTexture.isLoaded() && images[i].gpuTexture.isLoaded() != true && images[i].priority < lowestPriority)
		{
			lowestPriority      = images[i].priority;
			lowestPriorityIndex = i;
		}
	}

	return lowestPriorityIndex;
}

Index SortrRenderer::indexOfCPUImageWithHighestPriority() const
{
	if(images.empty()) logError("No images to get index from!");

	Float highestPriority      = 0;
	Index highestPriorityIndex = 0;

	//find the first image that is not yet loaded
	for(Int i = 0; i < static_cast<Int>(images.size()); i++)
	{
		if(images[i].cpuTexture.isLoaded() != true)
		{
			highestPriority      = images[i].priority;
			highestPriorityIndex = i;
			break;
		}
	}

	//get the image with the highest priority
	for(Index i = highestPriorityIndex; i < images.size(); i++)
	{
		if(images[i].cpuTexture.isLoaded() != true)
		{
			if(images[i].priority > highestPriority)
			{
				highestPriority      = images[i].priority;
				highestPriorityIndex = i;
			}
		}
	}

	return highestPriorityIndex;
}

Index SortrRenderer::indexOfGPUImageWithHighestPriority() const
{
	if(images.empty()) logError("No images to get index from!");

	Float highestPriority      = 0;
	Index highestPriorityIndex = 0;
	//find the first image that is not yet loaded
	for(Int i = 0; i < static_cast<Int>(images.size()); i++)
	{
		if(images[i].cpuTexture.isLoaded() && !images[i].gpuTexture.isLoaded())
		{
			highestPriority      = images[i].priority;
			highestPriorityIndex = i;
			break;
		}
	}

	//get the image with the highest priority
	for(Index i = highestPriorityIndex; i < images.size(); i++)
	{
		if(images[i].cpuTexture.isLoaded() && !images[i].gpuTexture.isLoaded())
		{
			if(images[i].priority > highestPriority)
			{
				highestPriority      = images[i].priority;
				highestPriorityIndex = i;
			}
		}
	}

	return highestPriorityIndex;
}

Vector<Index> SortrRenderer::indicesOfImagesSortedByPriority() const
{
	Vector<Index> indices;
	for(Index i = 0; i < images.size(); i++) indices.push_back(i);
	std::ranges::sort(indices, [&](const Index a, const Index b)
	{
		return images[a].priority > images[b].priority;
	});
	return indices;
}

void SortrRenderer::inputEvent(Window& window, const InputEvent input)
{
	lastButtonInput = now();

	if(input == zoomIn)
	{
		zoomLevel++;
		updateZoom();
	}

	if(input == zoomOut)
	{
		zoomLevel--;
		updateZoom();
	}

	if(input == left) sortLeft();

	if(input == shapeToggler)
	{
		ring.shapeToggle = !ring.shapeToggle;
		ring.updatePosition();
	}

	if(input == deleteImage && !images.empty())
	{
		deleteFile(images[currentImageId].path);
		images[currentImageId].destroy();
		images.erase(images.begin() + currentImageId);
		if(currentImageId >= static_cast<Int>(images.size())) currentImageId--;
	}

	if(input == resetView)
	{
		offset    = Vec2(0);
		zoomLevel = 0;
		updateZoom();
	}

	if(input == click)
	{
		addSortrFolderElement();
	}
}

void SortrRenderer::create(ResourceManager& resourceManager, Window& window)
{
	ring.create();
}

void SortrRenderer::preloadImage(const Path& path, GPUTexture& texture) const
{
	if(texture.isLoaded()) texture.unload();
	CPUTexture cpuTexture;
	cpuTexture.unload();
	cpuTexture.load(path, "SortrImage", Filter::Nearest, Filter::Linear, Wrapping::Clamped);
	texture.load(cpuTexture);
}

void SortrFolderRing::render(Renderer& renderer, ResourceManager& resourceManager) const
{
	for(const auto& element : elements)
	{
		renderer.uniforms().setMMatrix(matrixFromPositionAndSize(element.position, element.size));
		renderer.fill(Color(0.2f, 0.2f, 0.65f, 1.0f));
		renderer.aspectCorrectNormalizedSpace();
		renderer.useShader(resourceManager, "color");
		renderer.renderMesh(resourceManager, "centeredPlane");
		renderer.fill(Color(0, 0.5, 0.5, 1));
		renderer.textRenderSystem.alignText(Alignment::middle, Alignment::top);
		renderer.textRenderSystem.setSize(0.3f);
		renderer.textRenderSystem.setLetterSpacing(0.6f);
		renderer.textRenderSystem.render(resourceManager, renderer, String("image"), Vec2(0, 0));
	};
}

void SortrResource::calculateRating(const Index currentImageIndex, const UInt resourceCount)
{
	const auto  b        = static_cast<Float>(resourceCount);
	const auto  a        = static_cast<Float>(currentImageIndex);
	const auto  x        = static_cast<Float>(index);
	const Float diff_a_x = abs(a - x);
	priority             = b / 2.0f - min(abs(a - x - b), min(diff_a_x, abs(a - x + b)));
}

void SortrRenderer::render(ResourceManager& resourceManager, Window& window, TiledRectangle area)
{
	Renderer& renderer = window.renderer;
	renderer.uniforms().reset();
	renderer.setDepthTest(false);
	renderer.clearBackground(Color(0.08, 0.08, 0.08, 1));
	renderer.setAlphaBlending(true, BlendFunction::One, BlendFunction::OneMinusSrcAlpha, BlendEquation::Add);
	renderer.normalizedScreenSpace();

	Float imgX = 0;
	Float imgY = 0;

	if(!images.empty())
	{
		images[currentImageId].gpuTexture.useTextureInSlot(0);
		imgX = static_cast<Float>(images[currentImageId].cpuTexture.getWidth());
		imgY = static_cast<Float>(images[currentImageId].cpuTexture.getHeight());
	}

	const auto winX = static_cast<Float>(renderer.getWindowWidth());
	const auto winY = static_cast<Float>(renderer.getWindowHeight());

	const Float size = min(winX / imgX, winY / imgY);

	auto finalMatrix = Matrix(1);
	//finalMatrix = translate(finalMatrix, Vec3(offset, 0));
	finalMatrix = scale(finalMatrix, Vec3(size * imgX, size * imgY, 0));
	finalMatrix = scale(finalMatrix, Vec3(0.5));

	//approach(actualMatrix, finalMatrix, renderer.deltaTime() * 20);
	actualMatrix = finalMatrix;

	renderer.uniforms().setMMatrix(actualMatrix);

	renderer.useShader(resourceManager, "sortr");
	renderer.renderMesh(resourceManager, "centeredPlane");
	ring.render(renderer, resourceManager);

	// on screen controls
	renderer.uniforms().setMMatrix(Matrix(1));
	renderer.fill(Color(1));
	renderer.screenSpace();
	renderer.textRenderSystem.setSize(16.0f);
	renderer.textRenderSystem.setLetterSpacing(0.6f);
	renderer.textRenderSystem.alignText(Alignment::left, Alignment::bottom);
	renderer.textRenderSystem.render(resourceManager, renderer, "Left click to add nodes", Vec2(50));

	preloadImages();
}

void SortrRenderer::renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle area) {}
