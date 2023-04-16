
#include "RedDotHack.hpp"
#include <Windows.h>
#include "Basics.hpp"
#include "Math.hpp"
#include "Debug.hpp"

#pragma comment(lib, "Gdi32.lib")

void simulateLeftClick() {
	INPUT inputEvent = { 0 };
	inputEvent.type = INPUT_MOUSE;
	inputEvent.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	SendInput(1, &inputEvent, sizeof(inputEvent));

	Sleep(20);

	inputEvent.mi.dwFlags = MOUSEEVENTF_LEFTUP;
	SendInput(1, &inputEvent, sizeof(inputEvent));

	Sleep(20);
}

void simulateRightClick() {
	INPUT inputEvent = { 0 };
	inputEvent.type = INPUT_MOUSE;
	inputEvent.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
	SendInput(1, &inputEvent, sizeof(inputEvent));

	Sleep(10);

	inputEvent.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
	SendInput(1, &inputEvent, sizeof(inputEvent));

	Sleep(10);
}

void cheatTest() {
	//this is a hack that looks at the center pixel to figure out weather you are looking at an enemy or not
	//if the crosshair goes red it spams right and left click
	//this can work for multiple games as long as you have pixels that change when looking at an enemy

	//LPCSTR gameName = "Grand Theft Auto V";
	//LPCSTR gameName = "Dishonored";
	LPCSTR gameName = "Red Dead Redemption 2";

	HWND gameWindow = FindWindow(NULL, gameName);

	while (gameWindow == NULL) {
		logDebug("Start Game!");
		Sleep(1000);
		gameWindow = FindWindow(NULL, gameName);
	}

	HDC device = GetDC(0);              //use for fullscreen
	//HDC device = GetDC(gameWindow);   //use for windowed

	while (true) {
		RECT screenRectangle;
		if (GetClientRect(gameWindow, &screenRectangle)) {
			long x = screenRectangle.right - screenRectangle.left;
			long y = screenRectangle.bottom - screenRectangle.top;

			COLORREF colorRef = GetPixel(device, x / 2, y / 2);
			Vec3 color = Vec3(GetRValue(colorRef), GetGValue(colorRef), GetBValue(colorRef));
			//Vec3 target = Vec3(193, 79, 79);      //grand theft auto crosshair color
			//Vec3 target = Vec3(203, 24, 41);        //dishonored crosshair color
			Vec3 target = Vec3(255, 0, 0);        //red dead redemption 2 color

			if (isNear(color, target, 90)) {
				//simulateRightClick();
				simulateLeftClick();
				//beep();
				Sleep(50);
			}
		}
		else break;
	}

	ReleaseDC(gameWindow, device);
}

