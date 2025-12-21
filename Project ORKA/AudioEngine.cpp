#include "AudioEngine.hpp"

#include "Debug.hpp"

#define MINIAUDIO_IMPLEMENTATION
#define MA_NO_ENCODING
#include <miniaudio.h>

struct AudioTrack::Impl
{
	ma_sound sound{};
	Path     source;
	Bool     loaded  = false;
	Bool     looping = false;
};

struct AudioEngine::Impl
{
	ma_engine             engine{};
	Bool                  initialized = false;
	Map<String, AudioTrack> tracks;
};

AudioEngine audioEngine;

AudioTrack::AudioTrack() = default;
AudioTrack::AudioTrack(AudioTrack&&) noexcept = default;
AudioTrack& AudioTrack::operator=(AudioTrack&&) noexcept = default;

AudioTrack::~AudioTrack()
{
	if(impl && impl->loaded)
	{
		ma_sound_uninit(&impl->sound);
		impl->loaded = false;
	}
}

void AudioTrack::play()
{
	if(!impl || !impl->loaded) return;

	ma_sound_stop(&impl->sound);
	ma_sound_seek_to_pcm_frame(&impl->sound, 0);
	ma_sound_start(&impl->sound);
}

void AudioTrack::stop()
{
	if(!impl || !impl->loaded) return;
	ma_sound_stop(&impl->sound);
}

Bool AudioTrack::isLoaded() const
{
	return impl && impl->loaded;
}

AudioEngine::AudioEngine() : impl(std::make_unique<Impl>()) {}

AudioEngine::~AudioEngine()
{
	destroy();
}

void AudioEngine::create()
{
	if(!impl) impl = std::make_unique<Impl>();
	if(impl->initialized) return;

	ma_engine_config config = ma_engine_config_init();
	config.listenerCount    = 1;

	if(const ma_result result = ma_engine_init(&config, &impl->engine); result != MA_SUCCESS)
	{
		logError("Failed to initialize audio engine (miniaudio), error code: " + toString(result));
		return;
	}

	impl->initialized = true;
}

void AudioEngine::destroy()
{
	if(!impl) return;

	impl->tracks.clear();

	if(impl->initialized)
	{
		ma_engine_uninit(&impl->engine);
		impl->initialized = false;
	}
}

Bool AudioEngine::isInitialized() const
{
	return impl && impl->initialized;
}

Bool AudioEngine::has(const String& id) const
{
	return impl && impl->tracks.contains(id);
}

AudioTrack* AudioEngine::load(const String& id, const Path& path, const Bool loop)
{
	if(!impl) impl = std::make_unique<Impl>();
	if(!impl->initialized) create();
	if(!impl->initialized) return nullptr;

	if(const auto it = impl->tracks.find(id); it != impl->tracks.end()) return &it->second;

	AudioTrack track;
	track.impl            = std::make_unique<AudioTrack::Impl>();
	track.impl->source    = path;
	track.impl->looping   = loop;

	ma_uint32 flags = MA_SOUND_FLAG_DECODE;

	const String filePath = path.string();
	if(const ma_result result = ma_sound_init_from_file(&impl->engine, filePath.c_str(), flags, nullptr, nullptr, &track.impl->sound); result != MA_SUCCESS)
	{
		logWarning("Failed to load audio track (" + filePath + ")");
		return nullptr;
	}

	ma_sound_set_looping(&track.impl->sound, loop ? MA_TRUE : MA_FALSE);

	track.impl->loaded = true;

	const auto [iterator, inserted] = impl->tracks.emplace(id, std::move(track));
	if(!inserted) return nullptr;

	return &iterator->second;
}

void AudioEngine::play(const String& id)
{
	if(!impl || !impl->initialized) return;
	if(const auto it = impl->tracks.find(id); it != impl->tracks.end())
	{
		it->second.play();
		return;
	}

	logWarning("Tried to play missing audio track (" + id + ")");
}

