#pragma once

#include "Basics.hpp"

// Lightweight audio engine facade used to load and play short sound effects.
// The heavy audio backend (miniaudio) is hidden behind an internal
// implementation so that only this header needs to be included elsewhere.

struct AudioEngine;

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

private:
	struct Impl;
	UniquePointer<Impl> impl;
};

extern AudioEngine audioEngine;

