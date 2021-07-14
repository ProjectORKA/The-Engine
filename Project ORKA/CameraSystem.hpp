#pragma once

#include "PlanetCamera.hpp"
#include "SimpleCamera.hpp"

struct CameraSystem {
	//simple camera
	Index currentSimpleCameraID = 0;
	Vector<SimpleCamera> simpleCameras; //[TODO] rename to "cameras"
	void addSimpleCamera();

	//planet camera
	void addPlanetCamera();
	void selectPlanetCamera(Index cameraID);
	void renderPlanetCamera(Uniforms& uniforms, Float aspectRatio);
	PlanetCamera& currentPlanetCamera();
	Vector<PlanetCamera> planetCameras;
	Index currentPlanetCameraID = 0;

	void create();
	void destroy();
};