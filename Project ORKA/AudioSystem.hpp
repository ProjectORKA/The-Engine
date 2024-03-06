#pragma once

#include <AL/al.h>
#include <AL/alc.h>
#include "Math.hpp"

constexpr int    SAMPLE_RATE = 44100;
constexpr double FREQUENCY   = 1000.0; // Frequency of the sine wave in Hz
constexpr double AMPLITUDE   = 1.0;   // Amplitude of the sine wave

struct Sound
{
	Int      numSamples = 0;
	ALshort* samples    = nullptr;

	explicit Sound(const Int numSamples)
	{
		this->numSamples = numSamples;
		samples          = new ALshort[numSamples];
	}

	~Sound()
	{
		delete[] samples;
	}

	void generate() const
	{
		for(int i = 0; i < numSamples; ++i)
		{
			Double t   = static_cast<Double>(i) / SAMPLE_RATE;
			samples[i] = static_cast<ALshort>(AMPLITUDE * std::sin(2.0 * PI * FREQUENCY * t) * 32767.0);
		}
	}

	const ALvoid* data() const
	{
		return samples;
	}

	ALsizei byteSize() const
	{
		return numSamples * sizeof(ALshort);
	}
};

struct AudioBuffer
{
	Bool initialized = false;
	ALuint bufferID = 0;

	void setData(const Sound& sound) const
	{
		alBufferData(bufferID, AL_FORMAT_MONO16, sound.data(), sound.byteSize(), SAMPLE_RATE);
	}

	void destroy()
	{
		if(!initialized){
			logError("Buffer destroyed without being created!");
			return;
		}
		alDeleteBuffers(1, &bufferID);
		initialized = false;
	}

	~AudioBuffer()
	{
		if(initialized) logError("Buffer not destroyed properly!");
	}

	void create()
	{
		if(initialized)
		{
			logError("Buffer already initialized!");
			return;
		}
		alGenBuffers(1, &bufferID);
		initialized = true;
	}
};

struct AudioSource
{
	Bool initialized = false;
	ALuint sourceID = 0;


	void setBuffer(AudioBuffer& buffer)
	{
		if(initialized) alSourcei(sourceID, AL_BUFFER, buffer.bufferID);
		else logError("Audio Source not initialized!");
	}

	void play()
	{
		if(initialized) alSourcePlay(sourceID);
		else logError("Audio Source not initialized!");
	}

	~AudioSource()
	{
		if(initialized) logError("Audio Source was not properly destroyed!");
	}

	void create(Bool looping)
	{
		if(initialized)
		{
			logError("Audio Source already created!");
			return;
		}
		alGenSources(1, &sourceID);
		alSourcei(sourceID, AL_LOOPING, looping);
		initialized = true;
	}

	void destroy()
	{
		if(!initialized)
		{
			logError("Audio Source was not initialized!");
			return;
		}
		alDeleteSources(1, &sourceID);
		initialized = false;
	}
};

struct AudioSystem
{
	Bool        initialized = false;
	ALCdevice*  device      = nullptr;
	ALCcontext* context     = nullptr;
	Int         sampleCount = 44100;

	AudioSystem()
	{
		if(debugAudioSystem) logDebug("Creating Audio System!");

		if(initialized)
		{
			logError("Audio System already initialized!");
			return;
		}

		device = alcOpenDevice(nullptr); //get default sound device
		if(!device) logError("Failed to get sound device!");
		context = alcCreateContext(device, nullptr);
		if(!context) logError("Failed to create audio context!");
		if(!alcMakeContextCurrent(context)) logError("Failed to make context current!");
	}

	void destroy()
	{
		if(debugAudioSystem) logDebug("Destroying Audio System!");
		if(initialized)
		{
			alcMakeContextCurrent(nullptr);
			alcDestroyContext(context);
			alcCloseDevice(device);
			initialized = false;
		}
	}

	~AudioSystem()
	{
		destroy();
	}

	void audioTest() const
	{
		Sound beep(10000);
		beep.generate();

		AudioBuffer buffer;
		buffer.create();
		buffer.setData(beep);

		AudioSource source;
		source.create(true);
		source.setBuffer(buffer);

		source.play();

		sleep(3000);

		source.destroy();
		buffer.destroy();
	}
};
