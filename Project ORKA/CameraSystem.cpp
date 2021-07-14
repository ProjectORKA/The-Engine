
#include "CameraSystem.hpp"

void CameraSystem::create()
{
	addSimpleCamera();

	addPlanetCamera();
	selectPlanetCamera(0);
	currentPlanetCamera().rotate(Vec2(0));
}
void CameraSystem::destroy()
{
	planetCameras.clear();
	simpleCameras.clear();
}

//planetcameras
void CameraSystem::addPlanetCamera()
{
	planetCameras.emplace_back();
	currentPlanetCameraID = planetCameras.size() - 1;
}
void CameraSystem::selectPlanetCamera(Index cameraID)
{
	currentPlanetCameraID = cameraID;
}
void CameraSystem::renderPlanetCamera(Uniforms & uniforms, Float aspectRatio)
{
	currentPlanetCamera().render(uniforms, aspectRatio);
}
PlanetCamera& CameraSystem::currentPlanetCamera()
{
	return planetCameras[currentPlanetCameraID];
}

//simple cameras
void CameraSystem::addSimpleCamera()
{
	simpleCameras.emplace_back();
	currentSimpleCameraID = simpleCameras.size() - 1;
}