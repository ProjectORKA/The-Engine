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
	Vector<Float> leftSamples;
	Vector<Float> rightSamples;
	UInt          sampleRate = 0;
	UInt          channels   = 0;
};

struct AudioSpatialSettings
{
	Bool  enabled            = false;
	Bool  relativeToListener = false;
	Float minDistance        = 1.0f;
	Float maxDistance        = 120.0f;
	Float rolloff            = 1.0f;
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
	void setVolume(Float volume);
	void setPan(Float pan);
	void setPitch(Float pitch);
	void setSpatialPosition(const Vec3& position);
	void setSpatialVelocity(const Vec3& velocity);
	void setSpatialSettings(const AudioSpatialSettings& settings);

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
	void        playAt(const String& id, const Vec3& position, const Vec3& velocity = Vec3(0));
	[[nodiscard]] Bool setTrackVolume(const String& id, Float volume);
	[[nodiscard]] Bool setTrackPan(const String& id, Float pan);
	[[nodiscard]] Bool setTrackPitch(const String& id, Float pitch);
	[[nodiscard]] Bool setTrackSpatialPosition(const String& id, const Vec3& position);
	[[nodiscard]] Bool setTrackSpatialVelocity(const String& id, const Vec3& velocity);
	[[nodiscard]] Bool setTrackSpatialSettings(const String& id, const AudioSpatialSettings& settings);
	void               setListenerTransform(const Vec3& position, const Vec3& forward = Vec3(0, 1, 0), const Vec3& up = Vec3(0, 0, 1));
	void               setListenerVelocity(const Vec3& velocity);
	[[nodiscard]] Bool decodeAudioFile(const Path& path, DecodedAudioClip& out);
	[[nodiscard]] Bool decodeStereoAudioFile(const Path& path, DecodedAudioClip& out);
	void               startTrack(const String& id);
	void               seekTrack(const String& id, UInt framePosition);
	void               pauseTrack(const String& id);
	void               unloadTrack(const String& id);
	[[nodiscard]] UInt  getTrackPositionInFrames(const String& id);
	[[nodiscard]] UInt  getTrackLengthInFrames(const String& id);
	[[nodiscard]] Bool  isTrackPlaying(const String& id);
	AudioTrack*         loadFromSamples(const String& id, Vector<Float> samples, UInt sampleRate, Bool loop = false);
	[[nodiscard]] Bool  updateTrackSamples(const String& id, const Vector<Float>& samples);
	[[nodiscard]] UInt  getSampleRate() const;

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

