#include "Sortr.hpp"
#include "FileSystem.hpp"
#include "FileTypes.hpp"
#include "Platform.hpp"
#include "Windows.hpp"

void SortrFolderElement::setPath(const Path& path)
{
	this->path       = path;
	this->folderName = getFolderName(path);
}

void SortrFolderRing::add()
{
	const SortrFolderElement tmp;
	elements.push_back(tmp);
	updatePosition();
}

void SortrFolderRing::create()
{
	add();
}

void SortrRenderer::showNextImage()
{
	const auto currentTime = now();
	if (lastImageRefresh + Milliseconds(static_cast<Int>(1000 / frameRate)) < currentTime)
	{
		lastImageRefresh = currentTime;
		currentImageId++;
		if (currentImageId >= static_cast<Int>(images.size())) currentImageId = 0;
	}
}

void SortrRenderer::showPrevImage()
{
	const auto currentTime = now();
	if (lastImageRefresh + Milliseconds(static_cast<Int>(1000 / frameRate)) < currentTime)
	{
		lastImageRefresh = currentTime;
		currentImageId--;
		if (currentImageId < 0) currentImageId = static_cast<Int>(images.size() - 1);
	}
}

void SortrRenderer::preloadImages(Renderer& renderer)
{
	//load resources
	calculatePriorities();

	//if there are images
	if (!images.empty())
	{
		//get vector of indices sorted by priority
		Vector<Index> sortedIndices = indicesOfImagesSortedByPriority();

		//make sure were not loading more than we can
		if (sortedIndices.size() > cpuMaxPreloadCount) sortedIndices.resize(cpuMaxPreloadCount);

		//load images
		for (UInt order = 0; order < sortedIndices.size(); order++)
		{
			//get the image index by sorted order
			const Index currentImageIndex = sortedIndices[order];

			//load all images within the preload range but unload the rest
			if (order <= cpuMaxPreloadCount)
			{
				//load cpu image
				if (!images[currentImageIndex].inRam())
				{
					images[currentImageIndex].loadIntoRam();
					cpuLoadedCount++;
					//logDebug(cpuLoadedCount);
				}

				if (order < gpuMaxPreloadCount)
				{
					//load gpu image
					if (!images[currentImageIndex].onGpu())
					{
						images[currentImageIndex].loadOntoGpu(renderer);
						gpuLoadedCount++;
						//logDebug(gpuLoadedCount);
					}
				}
				else
				{
					//unload gpu image
					if (images[currentImageIndex].onGpu())
					{
						images[currentImageIndex].unloadFromGpu(renderer);
						gpuLoadedCount--;
						//logDebug(gpuLoadedCount);
					}
				}
			}
			else
			{
				//unload cpu image
				if (images[currentImageIndex].inRam())
				{
					images[currentImageIndex].unloadFromRam();
					cpuLoadedCount--;
					//logDebug(cpuLoadedCount);
				}

				//unload gpu image just to be safe
				if (images[currentImageIndex].onGpu())
				{
					images[currentImageIndex].unloadFromGpu(renderer);
					gpuLoadedCount--;
					//logDebug(gpuLoadedCount);
				}
			}
		}
	}

	if (cpuLoadedCount >= cpuMaxPreloadCount - 1)
	{
		if (cpuMaxPreloadCount < images.size())
		{
			if (availablePhysicalMemoryInBytes() > static_cast<ULL>(512 * 1024 * 1024))
			{
				cpuMaxPreloadCount++;
				gpuMaxPreloadCount = cpuMaxPreloadCount;
			}
		}
	}
}

void SortrRenderer::destroy(Window& window) {}

void SortrFolderRing::updatePosition()
{
	for (int i = 0; i < elements.size(); i++)
	{
		const Float angle    = 2 * PI * Float(i) / Float(elements.size());
		const Float radius   = 0.8f;
		Vec2        position = Vec2(sin(angle), cos(angle));
		const Float maxDist  = max(abs(position.x), abs(position.y));
		if (shapeToggle) position /= maxDist;
		position *= radius;
		elements[i].position = position;
		////////////////////////////////////////////////////////
		elements[i].size = 3.0f / Float(elements.size());
	};
}

void SortrRenderer::calculatePriorities()
{
	for (auto& image : images) image.calculateRating(currentImageId, (UInt)images.size());
}

void SortrRenderer::update(Window& window)
{
	// If the user dropped new paths (files/folders)
	if (!window.droppedFilePaths.empty())
	{
		// We'll keep track of a single "targetPath"
		// which could be an image file or a folder
		Path targetPath = "";

		for (Path& path : window.droppedFilePaths)
		{
			// If it's an image or a directory, make it our target
			if (isDirectory(path) || isImageFile(path)) targetPath = path;
		}

		// Clear the dropped paths so we don't reprocess them next frame
		window.droppedFilePaths.clear();

		// Only proceed if we found a valid path (either image or folder)
		if (!targetPath.empty())
		{
			// Destroy currently loaded images
			for (auto& img : images)
			{
				img.destroy(window.renderer);
			}
			images.clear();

			// Determine folder path:
			// - If user dropped a folder, use it directly
			// - If user dropped an image, use the image's parent folder
			Path folderPath = isDirectory(targetPath) ? targetPath : getDirectory(targetPath);

			// Get all image paths in that folder
			Vector<Path> imagePaths = getFilesInDirectory(folderPath, imageExtensions);

			// If the target is an image, set the current image ID to that image.
			// Otherwise, default to the first image in the folder (if any).
			if (isImageFile(targetPath))
			{
				for (Index i = 0; i < imagePaths.size(); i++)
				{
					if (imagePaths[i] == targetPath)
					{
						currentImageId = i;
						break;
					}
				}
			}
			else
			{
				// Dropped a folder; start at the first image.
				currentImageId = 0;
			}

			// Now load the images from the folder paths
			for (Index i = 0; i < imagePaths.size(); i++)
			{
				images.emplace_back(imagePaths[i], i);
			}
		}
	}
}

void SortrRenderer::create(Window& window)
{
	GameRenderer::create(window);
	ring.create();
}

void SortrRenderer::addSortrFolderElement()
{
	ring.add();
}

void Sortr::run(const Int argc, Char* argv[])
{
	logDebug(pickFolder());

	//file management
	// process parameters
	for (Int i = 0; i < argc; i++)
	{
		if (Path path = argv[i]; isExecutableFile(path))
		{
			executablePath = getDirectory(path);
			setCurrentPath(executablePath);
		}
		else
		{
			if (isImageFile(path)) filePaths.push_back(path);
			else logWarning(String("Can't process input: ").append(path.string()));
		}
	}

	Window&       window          = ui.window("Sortr", settings.defaultWindowWidth, settings.defaultWindowHeight, true, true, WindowState::Windowed, renderer);
	Vector<Path>& windowFilePaths = window.droppedFilePaths;
	for (auto& path : filePaths) if (isImageFile(path)) windowFilePaths.push_back(path);
	ui.run();
}

void SortrRenderer::connect(GameSimulation& simulation) {}

void SortrRenderer::loadGpuImageWithHighestPriority(Renderer& renderer)
{
	if (!images.empty())
	{
		const Index imageId = indexOfGpuImageWithHighestPriority();
		images[imageId].loadOntoGpu(renderer);
		if (images[imageId].onGpu()) gpuLoadedCount++;
		else logError("Image could not be loaded!");
	}
}

void SortrRenderer::loadCpuImageWithHighestPriority()
{
	if (!images.empty())
	{
		const Index imageId = indexOfCpuImageWithHighestPriority();
		images[imageId].loadIntoRam();
		if (images[imageId].inRam()) cpuLoadedCount++;
		else logError("Image could not be loaded!");
	}
}

void SortrRenderer::unloadGpuImageWithLowestPriority(Renderer& renderer)
{
	if (!images.empty())
	{
		const Index imageId = indexOfGpuImageWithLowestPriority();
		images[imageId].unloadFromGpu(renderer);
		if (!images[imageId].onGpu()) gpuLoadedCount--;
		else logError("Image could not be unloaded!");
	}
}

void SortrRenderer::unloadCpuImageWithLowestPriority()
{
	if (!images.empty())
	{
		const Index imageId = indexOfCpuImageWithLowestPriority();
		images[imageId].unloadFromRam();
		if (!images[imageId].inRam()) cpuLoadedCount--;
		else logError("Image could not be unloaded!");
	}
}

void SortrFolderRing::render(Renderer& renderer) const
{
	for (const auto& element : elements)
	{
		renderer.uniforms().setMMatrix(matrixFromPositionAndSize(element.position, element.size));
		renderer.fill(Color(0.2f, 0.2f, 0.65f, 1.0f));
		renderer.aspectCorrectNormalizedSpace();
		renderer.useShader("color");
		renderer.renderMesh("centeredPlane");
		renderer.fill(Color(0, 0.5, 0.5, 1));
		renderer.textRenderSystem.alignText(Alignment::middle, Alignment::top);
		renderer.fontSize(0.3f);
		renderer.textRenderSystem.setLetterSpacing(0.6f);
		renderer.textRenderSystem.render(renderer, element.folderName, Vec2(0, 0));
	};
}

void SortrFolderRing::renderInteractive(Renderer& r) const
{
	for (int i = 0; i < elements.size(); i++)
	{
		r.aspectCorrectNormalizedSpace();
		r.uniforms().setMMatrix(matrixFromPositionAndSize(elements[i].position, elements[i].size));
		r.uniforms().setObjectId(i);
		r.renderMesh("centeredPlane");
	}
}

Index SortrRenderer::indexOfGpuImageWithLowestPriority() const
{
	if (images.empty()) logError("No images to get index from!");

	Float lowestPriority      = 0;
	Index lowestPriorityIndex = 0;
	for (Int i = 0; i < static_cast<Int>(images.size()); i++)
	{
		if (images[i].onGpu() && images[i].getPriority() < lowestPriority)
		{
			lowestPriority      = images[i].getPriority();
			lowestPriorityIndex = i;
		}
	}
	return lowestPriorityIndex;
}

Index SortrRenderer::indexOfCpuImageWithLowestPriority() const
{
	if (images.empty()) logError("No images to get index from!");

	Float lowestPriority      = 0;
	Index lowestPriorityIndex = 0;

	for (Int i = 0; i < static_cast<Int>(images.size()); i++)
	{
		if (images[i].inRam() && images[i].onGpu() != true && images[i].getPriority() < lowestPriority)
		{
			lowestPriority      = images[i].getPriority();
			lowestPriorityIndex = i;
		}
	}

	return lowestPriorityIndex;
}

Index SortrRenderer::indexOfCpuImageWithHighestPriority() const
{
	if (images.empty()) logError("No images to get index from!");

	Float highestPriority      = 0;
	Index highestPriorityIndex = 0;

	//find the first image that is not yet loaded
	for (Int i = 0; i < static_cast<Int>(images.size()); i++)
	{
		if (images[i].inRam() != true)
		{
			highestPriority      = images[i].getPriority();
			highestPriorityIndex = i;
			break;
		}
	}

	//get the image with the highest priority
	for (Index i = highestPriorityIndex; i < images.size(); i++)
	{
		if (images[i].inRam() != true)
		{
			if (images[i].getPriority() > highestPriority)
			{
				highestPriority      = images[i].getPriority();
				highestPriorityIndex = i;
			}
		}
	}

	return highestPriorityIndex;
}

Index SortrRenderer::indexOfGpuImageWithHighestPriority() const
{
	if (images.empty()) logError("No images to get index from!");

	Float highestPriority      = 0;
	Index highestPriorityIndex = 0;
	//find the first image that is not yet loaded
	for (Int i = 0; i < static_cast<Int>(images.size()); i++)
	{
		if (images[i].inRam() && !images[i].onGpu())
		{
			highestPriority      = images[i].getPriority();
			highestPriorityIndex = i;
			break;
		}
	}

	//get the image with the highest priority
	for (Index i = highestPriorityIndex; i < images.size(); i++)
	{
		if (images[i].inRam() && !images[i].onGpu())
		{
			if (images[i].getPriority() > highestPriority)
			{
				highestPriority      = images[i].getPriority();
				highestPriorityIndex = i;
			}
		}
	}

	return highestPriorityIndex;
}

void SortrRenderer::renderScene(Window& window)
{
	Renderer& r = window.renderer;
	r.uniforms().reset();
	r.setDepthTest(false);
	r.clearBackground(Color(0.08, 0.08, 0.08, 1));
	r.setAlphaBlending(true, BlendFunction::One, BlendFunction::OneMinusSrcAlpha, BlendEquation::Add);
	r.normalizedScreenSpace(window);

	Float imgX = 0;
	Float imgY = 0;

	if (!images.empty())
	{
		images[currentImageId].use(r, 0);
		imgX = static_cast<Float>(images[currentImageId].getWidth());
		imgY = static_cast<Float>(images[currentImageId].getHeight());
	}

	const auto winX = static_cast<Float>(window.getWidth());
	const auto winY = static_cast<Float>(window.getHeight());

	const Float size = min(winX / imgX, winY / imgY);

	auto finalMatrix = Matrix(1);
	//finalMatrix = translate(finalMatrix, Vec3(offset, 0));
	finalMatrix = scale(finalMatrix, Vec3(size * imgX, size * imgY, 0));
	finalMatrix = scale(finalMatrix, Vec3(0.5));

	//approach(actualMatrix, finalMatrix, renderer.deltaTime() * 20);
	actualMatrix = finalMatrix;

	r.uniforms().setMMatrix(actualMatrix);

	r.useShader("sortr");
	r.renderMesh("centeredPlane");
	ring.render(r);

	// on screen controls
	r.uniforms().setMMatrix(Matrix(1));
	r.fill(Color(1));
	r.screenSpaceFromBottomLeft();
	r.fontSize(16.0f);
	r.textRenderSystem.setLetterSpacing(0.6f);
	r.textRenderSystem.alignText(Alignment::left, Alignment::bottom);
	r.textRenderSystem.render(r, "Left click to add nodes", Vec2(50));
	r.textRenderSystem.render(r, toString(r.objectId), Vec2(50, 100));

	preloadImages(r);
}

void SortrRenderer::renderInteractive(Window& window)
{
	Renderer& r = window.renderer;

	r.idFramebuffer.resize(r, getSize());
	r.idFramebuffer.clearColor(r, IVec4(-1));
	r.idFramebuffer.clearDepth(r);
	r.idFramebuffer.bindDraw(r);
	r.setWireframeMode(false);
	r.setDepthTest(true);
	r.setCulling(false);
	r.setAlphaBlending(false);
	r.normalizedScreenSpace(window);
	r.useShader("idShader");

	ring.renderInteractive(r);

	r.idFramebuffer.bindRead(r);
	IVec4 idData;
	if (window.capturing) idData = r.idFramebuffer.readPixelsAtCenterUIntRGB(r, FramebufferMode::Color0);
	else idData                  = r.idFramebuffer.readPixelsUIntRGB(r, static_cast<Int>(window.mousePosBotLeft.x), static_cast<Int>(window.mousePosBotLeft.y), FramebufferMode::Color0);
	r.objectId    = idData.x;
	r.instanceId  = idData.y;
	r.primitiveId = idData.z;
}

Vector<Index> SortrRenderer::indicesOfImagesSortedByPriority() const
{
	Vector<Index> indices;
	for (Index i = 0; i < images.size(); i++) indices.push_back(i);
	std::ranges::sort(indices, [&](const Index a, const Index b)
	{
		return images[a].getPriority() > images[b].getPriority();
	});
	return indices;
}

void SortrRenderer::inputEvent(Window& window, const InputEvent input)
{
	lastButtonInput = now();

	if (input == shapeToggler)
	{
		ring.shapeToggle = !ring.shapeToggle;
		ring.updatePosition();
	}

	if (input == deleteImage && !images.empty())
	{
		deleteFile(images[currentImageId].getPath());
		images[currentImageId].destroy(window.renderer);
		images.erase(images.begin() + currentImageId);
		if (currentImageId >= static_cast<Int>(images.size())) currentImageId--;
	}

	UInt eId = window.renderer.objectId;

	if (input == click && eId != -1)
	{
		//clicked button
		if (eId == 0)
		{
			//add folder
			addSortrFolderElement();
		}
		else
		{
			//set folder
			ring.elements[eId].setPath(pickFolder());
		}

		beep();
	}
}

void SortrRenderer::preloadImage(Renderer& renderer, const Path& path, GPUTexture& texture) const
{
	if (texture.isLoaded()) texture.unload(renderer);
	CpuTexture cpuTexture;
	cpuTexture.unload();
	cpuTexture.load(path, "SortrImage", Filter::Nearest, Filter::Linear, Wrapping::Clamped);
	texture.load(renderer, cpuTexture);
}
