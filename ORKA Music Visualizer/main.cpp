#include <iostream>
#include "raylib.h"

bool          fakeData         = false;
constexpr int sampleRate       = 48000;
constexpr int lowestFrequency  = 20;
constexpr int highestFrequency = 20000;
constexpr int bandCount        = 512;

unsigned long long currentSample = 0;

constexpr int frameBufferSize = 1024;
int           bufferOffset    = 0;
float         bufferA[frameBufferSize];
float         bufferB[frameBufferSize];
float*        frontBufferPtr = nullptr;
float*        backBufferPtr  = nullptr;

void swapBuffers()
{
	float* tmp     = frontBufferPtr;
	frontBufferPtr = backBufferPtr;
	backBufferPtr  = tmp;
}

void useBuffers(const unsigned int frameCount, const float* data)
{
	for (int i = 0; i < frameCount; i++)
	{
		backBufferPtr[bufferOffset] = data[i];
		bufferOffset++;
		if (bufferOffset >= frameBufferSize)
		{
			swapBuffers();
			bufferOffset = 0;
		}
	}
}

void callback(void* bufferData, const unsigned int unsignedFrameCount)
{
	const unsigned int frameCount = unsignedFrameCount;
	const float*       data       = static_cast<float*>(bufferData);

	useBuffers(frameCount, data);
}

int main()
{
	InitWindow(2000, 1024, "ORKA Music Visualizer");
	SetTargetFPS(144);
	InitAudioDevice();
	const Music audio = LoadMusicStream("sound.mp3");

    SetConfigFlags(FLAG_VSYNC_HINT);
    SetTargetFPS(144);

	//set buffer pointers
	frontBufferPtr = &bufferA[0];
	backBufferPtr  = &bufferB[0];

	if (!fakeData) AttachAudioStreamProcessor(audio.stream, callback);

	PlayMusicStream(audio);
	while (!WindowShouldClose())
	{
		const float width  = GetRenderWidth();
		const float height = GetRenderHeight();

		if (!fakeData) UpdateMusicStream(audio);

		if (fakeData)
		{
			float data[frameBufferSize];
			for (float& i : data) i = sin(PI * (static_cast<float>(currentSample++) / static_cast<float>(sampleRate) * 20.0f));
			callback(data, frameBufferSize);
		}

		BeginDrawing();
		ClearBackground({255, 0, 0, 255});

		int i = 0;
		while (i < frameBufferSize)
		{
			const float sampleA = frontBufferPtr[i] * 512;
			const float sampleB = frontBufferPtr[i + 2] * 512;
			DrawLine(i, 512 - (sampleA), i + 2, 512 - (sampleB), {255, 255, 255, 255});
			i += 2;
		}

		EndDrawing();
	}

	return 0;
}
