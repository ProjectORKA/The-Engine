#pragma once

#include <AL/al.h>
#include <AL/alc.h>
#include "Math.hpp"
#include "SoundGenerators.hpp"

struct Sound
{

	~Sound()
	{
		delete[] samples;
	}

	Int getSampleRate() const
	{
		return sampleRate;
	}

	void generate(Double frequency) const
	{
		for(int i = 0; i < numSamples; ++i)
		{
			Double t   = toDouble(i) / sampleRate;
			samples[i] = static_cast<ALshort>(std::sin(2.0 * PI * frequency * t) * 32767.0);
		}
	}

	void generate2(Double frequency) const
	{
		for(int i = 0; i < numSamples; ++i)
		{
			Double t   = toDouble(i) / sampleRate;
			samples[i] = static_cast<ALshort>(std::sin(2.0 * PI * frequency * mod(t, 0.5) * 2 - 1) * 32767.0);
		}
	}

	[[nodiscard]] ALsizei byteSize() const
	{
		return numSamples * sizeof(ALshort);
	}

	[[nodiscard]] const ALvoid* data() const
	{
		return samples;
	}

	explicit Sound(const Double duration, const Int sampleRate = 48000)
	{
		this->numSamples = duration * sampleRate;
		samples          = new ALshort[numSamples];
	}

private:
	Int      numSamples = 0;
	Double   duration   = 0.0;
	Int      sampleRate = 48000;
	ALshort* samples    = nullptr;
};


struct AudioBuffer
{
	Bool   initialized = false;
	ALuint bufferID = 0;

	void setData(const Sound& sound) const
	{
		alBufferData(bufferID, AL_FORMAT_MONO16, sound.data(), sound.byteSize(), sound.getSampleRate());
	}

	void destroy()
	{
		if (!initialized)
		{
			logError("Buffer destroyed without being created!");
			return;
		}
		alDeleteBuffers(1, &bufferID);
		initialized = false;
	}

	~AudioBuffer()
	{
		if (initialized) destroy();
	}

	void create()
	{
		if (initialized)
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
	Bool   initialized = false;
	ALuint sourceID = 0;

	void setBuffer(const AudioBuffer& buffer) const
	{
		if (initialized) alSourcei(sourceID, AL_BUFFER, buffer.bufferID);
		else logError("Audio Source not initialized!");
	}

	void play() const
	{
		if (initialized) alSourcePlay(sourceID);
		else logError("Audio Source not initialized!");
	}

	void pause() const
	{
		if (initialized) alSourcePause(sourceID);
		else logError("Audio Source not initialized!");
	}

	void stop() const
	{
		if (initialized) alSourceStop(sourceID);
		else logError("Audio Source not initialized!");
	}

	Bool isPlaying() const
	{
		ALint state;
		alGetSourcei(sourceID, AL_SOURCE_STATE, &state);
		return state == AL_PLAYING;
	}

	~AudioSource()
	{
		if (initialized) logError("Audio Source was not properly destroyed!");
	}

	void create(const Bool looping)
	{
		if (initialized)
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
		if (!initialized)
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
	ALCdevice* device = nullptr;
	ALCcontext* context = nullptr;
	Int         sampleCount = 44100;

	AudioSystem()
	{
		if (debugAudioSystem) logDebug("Creating Audio System!");

		if (initialized)
		{
			logError("Audio System already initialized!");
			return;
		}

		device = alcOpenDevice(nullptr); //get default sound device
		if (!device) logError("Failed to get sound device!");
		context = alcCreateContext(device, nullptr);
		if (!context) logError("Failed to create audio context!");
		if (!alcMakeContextCurrent(context)) logError("Failed to make context current!");
	}

	void destroy()
	{
		if (debugAudioSystem) logDebug("Destroying Audio System!");
		if (initialized)
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
		const Sound sound1(1);
		sound1.generate(100);

		const Sound sound2(1);
		sound2.generate2(100);

		AudioBuffer buffer1;
		buffer1.create();
		buffer1.setData(sound1);

		AudioBuffer buffer2;
		buffer2.create();
		buffer2.setData(sound1);

		AudioSource source;
		source.create(false);
		source.setBuffer(buffer1);

		source.play();

		sleep(3000);

		source.stop();
		source.setBuffer(buffer2);
		source.play();

		sleep(3000);

		source.destroy();
		buffer1.destroy();
		buffer2.destroy();
	}
};
