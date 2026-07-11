#pragma once

#include "Basics.hpp"

// Lightweight audio engine facade used to load and play short sound effects.
// The heavy audio backend (miniaudio) is hidden behind an internal
// implementation so that only this header needs to be included elsewhere.

struct AudioEngine;

// Rough in-game speed ceiling used to map player units/sec to procedural audio speed01.
inline constexpr Float proceduralMotionMaxSpeedUnitsPerSec = 15000.0f;

struct ProceduralMotionTuning
{
	Float humNoiseMix   = 0.45f;
	Float humPitchScale = 0.3f;
	Float windPitchScale = 1.0f;
	Float humGain       = 1.0f;
	Float windGain      = 1.0f;
	Float breathGain    = 1.0f;
	Float responseCurve = 0.25f;
	Bool  enableHum     = true;
	Bool  enableWind    = true;
	Bool  enableBreath  = true;
};

struct DecodedAudioClip
{
	Vector<Float> monoSamples;
	UInt          sampleRate = 0;
	UInt          channels   = 0;
};

struct AudioTrack
{
	AudioTrack();
	AudioTrack(const AudioTrack&)            = delete;
	AudioTrack& operator=(const AudioTrack&) = delete;
	AudioTrack(AudioTrack&&) noexcept;
	AudioTrack& operator=(AudioTrack&&) noexcept;
	~AudioTrack();

	void play();
	void stop();

	[[nodiscard]] Bool isLoaded() const;

private:
	struct Impl;
	UniquePointer<Impl> impl;

	friend struct AudioEngine;
};

struct AudioEngine
{
	AudioEngine();
	AudioEngine(const AudioEngine&)            = delete;
	AudioEngine& operator=(const AudioEngine&) = delete;
	AudioEngine(AudioEngine&&)                 = delete;
	AudioEngine& operator=(AudioEngine&&)      = delete;
	~AudioEngine();

	void create();
	void destroy();

	[[nodiscard]] Bool isInitialized() const;
	[[nodiscard]] Bool has(const String& id) const;

	AudioTrack* load(const String& id, const Path& path, Bool loop = false);
	void        play(const String& id);
	[[nodiscard]] Bool decodeAudioFile(const Path& path, DecodedAudioClip& out);

	// Motion-reactive procedural layer (wind + hum) for gameplay movement feedback.
	Bool createProceduralMotion();
	void destroyProceduralMotion();

	[[nodiscard]] Bool hasProceduralMotion() const;
	void               setProceduralMotionEnabled(Bool enabled);
	void               setProceduralMotionTuning(const ProceduralMotionTuning& tuning);
	[[nodiscard]] ProceduralMotionTuning getProceduralMotionTuning() const;
	void               updateProceduralMotion(Float speed01, Float dopplerDirection, Float deltaTime);
	void               getProceduralMotionDebugWaveform(Vector<Float>& waveformOut, Vector<Float>& controlOut, Vector<Float>& audioOut, Vector<Float>* humOut = nullptr, Vector<Float>* windOut = nullptr, Vector<Float>* breathOut = nullptr) const;

private:
	struct Impl;
	UniquePointer<Impl> impl;
};

extern AudioEngine audioEngine;

