#include "AudioEngine.hpp"

#include "Debug.hpp"

#include <atomic>

#define MINIAUDIO_IMPLEMENTATION
#define MA_NO_ENCODING
#include <miniaudio.h>

namespace
{
	constexpr UInt  proceduralSampleRate     = 48000;
	constexpr UInt  proceduralChannels       = 1;
	constexpr UInt  proceduralLoopFrames     = proceduralSampleRate * 8;
	constexpr SizeT proceduralDebugSamples   = 512;
	constexpr Float twoPi                    = 6.28318530718f;
	constexpr Float proceduralHumAmHz        = 47.0f;
	constexpr Float proceduralRestSpeedThreshold = 0.015f;
	constexpr Float proceduralMotionSpeedThreshold = 0.04f;
	// Baked loop runs at this pulse rate; runtime pitch scales it with movement speed.
	constexpr Float proceduralHeartbeatBaseHz = 1.15f;
	constexpr Float proceduralHeartbeatRestHz = 0.9f;
	// Keep top-end pulse rate musical and avoid chipmunk-style pitch at max speed.
	constexpr Float proceduralHeartbeatMaxHz  = 2.2f;

	Float heartbeatRateHz(const Float speed01)
	{
		const Float speed = std::clamp(speed01, 0.0f, 1.0f);
		return proceduralHeartbeatRestHz + speed * (proceduralHeartbeatMaxHz - proceduralHeartbeatRestHz);
	}

	Float heartbeatPitchFromSpeed(const Float speed01)
	{
		return heartbeatRateHz(speed01) / proceduralHeartbeatBaseHz;
	}

	Float smoothstep(const Float edge0, const Float edge1, const Float x)
	{
		const Float t = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
		return t * t * (3.0f - 2.0f * t);
	}

	Float gaussianPulse(const Float x, const Float center, const Float width)
	{
		const Float sigma = max(width, 0.0001f);
		const Float n     = (x - center) / sigma;
		return exp(-0.5f * n * n);
	}

	Float sampleLoop(const Vector<Float>& samples, const ma_uint64 frameIndex)
	{
		if(samples.empty()) return 0.0f;
		return samples[static_cast<SizeT>(frameIndex % static_cast<ma_uint64>(samples.size()))];
	}

	void normalizeWave(Vector<Float>& samples)
	{
		Float peak = 0.0f;
		for(const Float sample : samples) peak = max(peak, abs(sample));
		if(peak <= 0.0001f) return;
		for(Float& sample : samples) sample /= peak;
	}

	void exportRingBuffer(const Vector<Float>& ring, const SizeT cursor, Vector<Float>& out)
	{
		out.clear();
		if(ring.empty()) return;
		out.reserve(ring.size());
		for(SizeT i = cursor; i < ring.size(); ++i) out.push_back(ring[i]);
		for(SizeT i = 0; i < cursor; ++i) out.push_back(ring[i]);
	}

	Float restHumPitch(const ProceduralMotionTuning& tuning)
	{
		return std::clamp(tuning.humPitchScale, 0.1f, 0.5f);
	}

	Float restWindPitch(const ProceduralMotionTuning& tuning)
	{
		return tuning.windPitchScale;
	}

	Float windLpfCutoffHz(const Float speed01)
	{
		const Float speed = std::clamp(speed01, 0.0f, 1.0f);
		constexpr Float minCutoff = 320.0f;
		constexpr Float maxCutoff = 14000.0f;
		return minCutoff + pow(speed, 0.62f) * (maxCutoff - minCutoff);
	}

	constexpr Float windLpfCutoffSmoothHz   = 7.5f;
	constexpr Float windReleaseSmoothHz     = 9.0f;
	constexpr Float heartbeatReleaseSmoothHz = 3.2f;

	struct ProceduralMotion;

	struct WindFilteredSource
	{
		ma_data_source_base base{};
		ProceduralMotion*   owner = nullptr;
	};

	struct ProceduralMotion
	{
		Vector<Float> humSamples;
		Vector<Float> windSamples;
		Vector<Float> breathSamples;
		ma_audio_buffer humBuffer{};
		ma_audio_buffer windBuffer{};
		ma_audio_buffer breathBuffer{};
		WindFilteredSource windFilteredSource{};
		Vector<Float> windReadScratch = Vector<Float>(4096, 0.0f);
		std::atomic<Float> windLpfTargetCutoff{320.0f};
		Float windLpfCutoffSmooth     = 320.0f;
		Float windDebugLpfCutoffSmooth = 320.0f;
		Float windLpfState            = 0.0f;
		Float windDebugLpfState       = 0.0f;
		Float windSpeedForTone        = 0.0f;
		Float heartbeatSpeedForPitch  = 0.0f;
		ma_sound humSound{};
		ma_sound windSound{};
		ma_sound breathSound{};

		Bool initialized = false;
		Bool enabled     = false;

		Vector<Float> debugWaveform = Vector<Float>(proceduralDebugSamples, 0.0f);
		Vector<Float> debugControl  = Vector<Float>(proceduralDebugSamples, 0.0f);
		Vector<Float> debugAudio    = Vector<Float>(proceduralDebugSamples, 0.0f);
		Vector<Float> debugHum      = Vector<Float>(proceduralDebugSamples, 0.0f);
		Vector<Float> debugWind     = Vector<Float>(proceduralDebugSamples, 0.0f);
		Vector<Float> debugBreath   = Vector<Float>(proceduralDebugSamples, 0.0f);
		SizeT         debugCursor   = 0;
		Float         debugPhase    = 0.0f;
		Float         currentHumVolume    = 0.0f;
		Float         currentWindVolume   = 0.0f;
		Float         currentBreathVolume = 0.0f;
		Float         currentBreathPitch  = 1.0f;
		Float         currentHumPitch   = 0.3f;
		Float         currentWindPitch  = 1.0f;
		Float         smoothedDopplerDirection = 0.0f;
		ProceduralMotionTuning tuning;

		void resetRuntimeState()
		{
			const Float humPitch  = restHumPitch(tuning);
			const Float windPitch = restWindPitch(tuning);

			currentHumVolume         = 0.0f;
			currentWindVolume        = 0.0f;
			currentBreathVolume      = 0.0f;
			currentBreathPitch       = heartbeatPitchFromSpeed(0.0f);
			currentHumPitch          = humPitch;
			currentWindPitch         = windPitch;
			smoothedDopplerDirection = 0.0f;
			const Float initialWindCutoff = windLpfCutoffHz(0.0f);
			windLpfTargetCutoff.store(initialWindCutoff, std::memory_order_relaxed);
			windLpfCutoffSmooth     = initialWindCutoff;
			windDebugLpfCutoffSmooth = initialWindCutoff;
			windLpfState             = 0.0f;
			windDebugLpfState        = 0.0f;
			windSpeedForTone         = 0.0f;
			heartbeatSpeedForPitch   = 0.0f;
			debugPhase               = 0.0f;
			debugCursor              = 0;
			std::fill(debugWaveform.begin(), debugWaveform.end(), 0.0f);
			std::fill(debugControl.begin(), debugControl.end(), 0.0f);
			std::fill(debugAudio.begin(), debugAudio.end(), 0.0f);
			std::fill(debugHum.begin(), debugHum.end(), 0.0f);
			std::fill(debugWind.begin(), debugWind.end(), 0.0f);
			std::fill(debugBreath.begin(), debugBreath.end(), 0.0f);

			if(!initialized) return;

			ma_sound_set_volume(&humSound, 0.0f);
			ma_sound_set_volume(&windSound, 0.0f);
			ma_sound_set_volume(&breathSound, 0.0f);
			ma_sound_set_pitch(&humSound, humPitch);
			ma_sound_set_pitch(&windSound, windPitch);
			ma_sound_set_pitch(&breathSound, currentBreathPitch);
			ma_sound_stop(&humSound);
			ma_sound_stop(&windSound);
			ma_sound_stop(&breathSound);
		}
	};

	Float windLpfAlpha(const Float cutoffHz)
	{
		return exp(-twoPi * std::clamp(cutoffHz, 20.0f, 20000.0f) / static_cast<Float>(proceduralSampleRate));
	}

	Float windLpfCutoffFollowPerSample()
	{
		return 1.0f - exp(-windLpfCutoffSmoothHz / static_cast<Float>(proceduralSampleRate));
	}

	void flushDenormal(Float& value)
	{
		if(abs(value) < 1.0e-20f) value = 0.0f;
	}

	void applyWindLowPass(ProceduralMotion& procedural, const Float* input, Float* output, const ma_uint64 frameCount)
	{
		const Float targetCutoff = procedural.windLpfTargetCutoff.load(std::memory_order_relaxed);
		const Float cutoffFollow = windLpfCutoffFollowPerSample();
		for(ma_uint64 i = 0; i < frameCount; ++i)
		{
			procedural.windLpfCutoffSmooth += (targetCutoff - procedural.windLpfCutoffSmooth) * cutoffFollow;
			const Float alpha = windLpfAlpha(procedural.windLpfCutoffSmooth);
			procedural.windLpfState = procedural.windLpfState * alpha + input[i] * (1.0f - alpha);
			flushDenormal(procedural.windLpfState);
			output[i] = procedural.windLpfState;
		}
	}

	ma_result windFilteredOnRead(ma_data_source* pDataSource, void* pFramesOut, const ma_uint64 frameCount, ma_uint64* pFramesRead)
	{
		auto* source = reinterpret_cast<WindFilteredSource*>(pDataSource);
		ProceduralMotion* motion = source ? source->owner : nullptr;
		if(!motion || !pFramesOut) return MA_INVALID_ARGS;

		auto* outSamples = static_cast<Float*>(pFramesOut);
		ma_uint64       totalRead = 0;
		while(totalRead < frameCount)
		{
			const ma_uint64 chunkFrames = min(frameCount - totalRead, static_cast<ma_uint64>(motion->windReadScratch.size()));
			ma_uint64       chunkRead   = 0;
			const ma_result readResult  = ma_data_source_read_pcm_frames(reinterpret_cast<ma_data_source*>(&motion->windBuffer), motion->windReadScratch.data(), chunkFrames, &chunkRead);
			if(readResult != MA_SUCCESS && readResult != MA_AT_END) return readResult;
			if(chunkRead == 0) break;

			applyWindLowPass(*motion, motion->windReadScratch.data(), outSamples + totalRead, chunkRead);
			totalRead += chunkRead;
			if(chunkRead < chunkFrames) break;
		}

		if(pFramesRead) *pFramesRead = totalRead;
		if(totalRead == 0) return MA_AT_END;
		return totalRead < frameCount ? MA_AT_END : MA_SUCCESS;
	}

	ma_result windFilteredOnSeek(ma_data_source* pDataSource, const ma_uint64 frameIndex)
	{
		auto* source = reinterpret_cast<WindFilteredSource*>(pDataSource);
		if(!source || !source->owner) return MA_INVALID_ARGS;
		return ma_data_source_seek_to_pcm_frame(reinterpret_cast<ma_data_source*>(&source->owner->windBuffer), frameIndex);
	}

	ma_result windFilteredOnGetDataFormat(ma_data_source* pDataSource, ma_format* pFormat, ma_uint32* pChannels, ma_uint32* pSampleRate, ma_channel* pChannelMap, const size_t channelMapCap)
	{
		auto* source = reinterpret_cast<WindFilteredSource*>(pDataSource);
		if(!source || !source->owner) return MA_INVALID_ARGS;
		return ma_data_source_get_data_format(reinterpret_cast<ma_data_source*>(&source->owner->windBuffer), pFormat, pChannels, pSampleRate, pChannelMap, channelMapCap);
	}

	ma_result windFilteredOnGetCursor(ma_data_source* pDataSource, ma_uint64* pCursor)
	{
		auto* source = reinterpret_cast<WindFilteredSource*>(pDataSource);
		if(!source || !source->owner || !pCursor) return MA_INVALID_ARGS;
		return ma_data_source_get_cursor_in_pcm_frames(reinterpret_cast<ma_data_source*>(&source->owner->windBuffer), pCursor);
	}

	ma_result windFilteredOnGetLength(ma_data_source* pDataSource, ma_uint64* pLength)
	{
		auto* source = reinterpret_cast<WindFilteredSource*>(pDataSource);
		if(!source || !source->owner || !pLength) return MA_INVALID_ARGS;
		return ma_data_source_get_length_in_pcm_frames(reinterpret_cast<ma_data_source*>(&source->owner->windBuffer), pLength);
	}

	ma_result windFilteredOnSetLooping(ma_data_source* pDataSource, const ma_bool32 isLooping)
	{
		auto* source = reinterpret_cast<WindFilteredSource*>(pDataSource);
		if(!source || !source->owner) return MA_INVALID_ARGS;
		return ma_data_source_set_looping(reinterpret_cast<ma_data_source*>(&source->owner->windBuffer), isLooping);
	}

	const ma_data_source_vtable g_windFilteredDataSourceVtable =
	{
		windFilteredOnRead,
		windFilteredOnSeek,
		windFilteredOnGetDataFormat,
		windFilteredOnGetCursor,
		windFilteredOnGetLength,
		windFilteredOnSetLooping,
		0
	};

	void setWindLpfTarget(ProceduralMotion& procedural, const Float speed01)
	{
		procedural.windLpfTargetCutoff.store(windLpfCutoffHz(speed01), std::memory_order_relaxed);
	}

	Float filterWindSample(ProceduralMotion& procedural, const Float sample, const Float deltaTime)
	{
		const Float targetCutoff = procedural.windLpfTargetCutoff.load(std::memory_order_relaxed);
		const Float cutoffFollow = 1.0f - exp(-max(deltaTime, 0.0001f) * windLpfCutoffSmoothHz);
		procedural.windDebugLpfCutoffSmooth += (targetCutoff - procedural.windDebugLpfCutoffSmooth) * cutoffFollow;
		const Float alpha = windLpfAlpha(procedural.windDebugLpfCutoffSmooth);
		procedural.windDebugLpfState = procedural.windDebugLpfState * alpha + sample * (1.0f - alpha);
		flushDenormal(procedural.windDebugLpfState);
		return procedural.windDebugLpfState;
	}

	Float smoothWindSpeedWithRelease(ProceduralMotion& procedural, const Float speed01, const Float deltaTime)
	{
		const Float speed = std::clamp(speed01, 0.0f, 1.0f);
		if(speed >= procedural.windSpeedForTone)
		{
			// Fast acceleration: immediately match requested speed so wind reaches full intensity right away.
			procedural.windSpeedForTone = speed;
		}
		else
		{
			// Deceleration: short, smooth release to avoid abrupt wind cutoffs.
			const Float releaseFollow = 1.0f - exp(-max(deltaTime, 0.0001f) * windReleaseSmoothHz);
			procedural.windSpeedForTone += (speed - procedural.windSpeedForTone) * releaseFollow;
		}
		return procedural.windSpeedForTone;
	}

	Float smoothHeartbeatSpeedWithRelease(ProceduralMotion& procedural, const Float speed01, const Float deltaTime)
	{
		const Float speed = std::clamp(speed01, 0.0f, 1.0f);
		if(speed >= procedural.heartbeatSpeedForPitch)
		{
			// Fast spool-up when accelerating.
			procedural.heartbeatSpeedForPitch = speed;
		}
		else
		{
			// Large ease-out on slowdown for a lingering turbine decay.
			const Float releaseFollow = 1.0f - exp(-max(deltaTime, 0.0001f) * heartbeatReleaseSmoothHz);
			procedural.heartbeatSpeedForPitch += (speed - procedural.heartbeatSpeedForPitch) * releaseFollow;
		}
		return procedural.heartbeatSpeedForPitch;
	}
}

struct AudioTrack::Impl
{
	ma_sound sound{};
	Path     source;
	Bool     loaded  = false;
	Bool     looping = false;
};

struct AudioEngine::Impl
{
	ma_engine               engine{};
	Bool                    initialized = false;
	Map<String, AudioTrack> tracks;
	ProceduralMotion        procedural;
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

	destroyProceduralMotion();
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

Bool AudioEngine::decodeAudioFile(const Path& path, DecodedAudioClip& out)
{
	out = {};

	const String filePath = path.string();
	ma_decoder_config config = ma_decoder_config_init(ma_format_f32, 1, 0);
	ma_decoder decoder{};
	if(const ma_result initResult = ma_decoder_init_file(filePath.c_str(), &config, &decoder); initResult != MA_SUCCESS)
	{
		logWarning("Failed to open audio file for decode (" + filePath + "), error code: " + toString(initResult));
		return false;
	}

	ma_uint32 nativeChannels = 0;
	ma_uint32 nativeSampleRate = 0;
	ma_decoder_get_data_format(&decoder, nullptr, &nativeChannels, &nativeSampleRate, nullptr, 0);
	out.channels   = nativeChannels;
	out.sampleRate = nativeSampleRate;

	ma_uint64 totalFrames = 0;
	if(ma_decoder_get_length_in_pcm_frames(&decoder, &totalFrames) == MA_SUCCESS && totalFrames > 0)
	{
		out.monoSamples.resize(static_cast<SizeT>(totalFrames));
		ma_uint64 framesRead = 0;
		if(const ma_result readResult = ma_decoder_read_pcm_frames(&decoder, out.monoSamples.data(), totalFrames, &framesRead); readResult != MA_SUCCESS || framesRead == 0)
		{
			ma_decoder_uninit(&decoder);
			logWarning("Failed to read decoded audio frames (" + filePath + ")");
			return false;
		}
		out.monoSamples.resize(static_cast<SizeT>(framesRead));
	}
	else
	{
		constexpr ma_uint64 chunkFrames = 4096;
		Vector<Float> chunk(chunkFrames);
		for(;;)
		{
			ma_uint64 framesRead = 0;
			if(ma_decoder_read_pcm_frames(&decoder, chunk.data(), chunkFrames, &framesRead) != MA_SUCCESS || framesRead == 0) break;
			out.monoSamples.insert(out.monoSamples.end(), chunk.begin(), chunk.begin() + static_cast<SizeT>(framesRead));
		}
	}

	ma_decoder_uninit(&decoder);
	if(out.monoSamples.empty())
	{
		logWarning("Decoded audio file contained no samples (" + filePath + ")");
		return false;
	}

	return true;
}

Bool AudioEngine::createProceduralMotion()
{
	if(!impl) impl = std::make_unique<Impl>();
	if(!impl->initialized) create();
	if(!impl->initialized) return false;

	auto& procedural = impl->procedural;
	if(procedural.initialized)
	{
		procedural.resetRuntimeState();
		return true;
	}
	procedural.humSamples.resize(proceduralLoopFrames);
	procedural.windSamples.resize(proceduralLoopFrames);
	procedural.breathSamples.resize(proceduralLoopFrames);

	UInt randomState = 0xA3C59AC3u;
	auto nextRandom = [&randomState]() -> Float
	{
		randomState ^= randomState << 13;
		randomState ^= randomState >> 17;
		randomState ^= randomState << 5;
		const UInt mantissa = randomState & 0x00FFFFFFu;
		return static_cast<Float>(mantissa) / static_cast<Float>(0x007FFFFFu) - 1.0f;
	};

	Float brownNoise = 0.0f;
	Float dcTracker  = 0.0f;
	Float airNoise   = 0.0f;
	Float rumbleNoise = 0.0f;
	Float rumbleDc    = 0.0f;
	Float breathNoise = 0.0f;
	Float breathDc    = 0.0f;
	for(UInt frame = 0; frame < proceduralLoopFrames; ++frame)
	{
		const Float timeSeconds = static_cast<Float>(frame) / static_cast<Float>(proceduralSampleRate);
		constexpr Float basePitch = 110.0f;

		Float hum = sin(timeSeconds * basePitch * twoPi);
		hum += sin(timeSeconds * basePitch * 1.017f * twoPi) * 0.20f;
		hum += sin(timeSeconds * basePitch * 2.0f * twoPi) * 0.36f;
		hum += sin(timeSeconds * basePitch * 3.0f * twoPi) * 0.17f;
		hum += sin(timeSeconds * basePitch * 4.3f * twoPi) * 0.08f;
		// Fast AM keeps saber buzz lively without pitch modulation.
		hum *= 0.82f + 0.18f * (0.5f + 0.5f * sin(timeSeconds * proceduralHumAmHz * twoPi));
		hum += sin(timeSeconds * 62.0f * twoPi) * 0.025f;
		procedural.humSamples[frame] = hum;

		const Float whiteNoise = nextRandom();
		brownNoise = std::clamp(brownNoise + whiteNoise * 0.03f, -1.0f, 1.0f);
		dcTracker  = dcTracker * 0.9992f + brownNoise * 0.0008f;
		const Float brownBand = brownNoise - dcTracker;
		airNoise = airNoise * 0.78f + whiteNoise * 0.22f;
		rumbleNoise = std::clamp(rumbleNoise + whiteNoise * 0.010f, -1.0f, 1.0f);
		rumbleDc    = rumbleDc * 0.9996f + rumbleNoise * 0.0004f;
		const Float lowRumble = rumbleNoise - rumbleDc;

		// Blend broad body + airy hiss + subtle low-end rumble.
		procedural.windSamples[frame] = brownBand * 0.78f + airNoise * 0.14f + lowRumble * 0.08f;

		const Float beatCycle = std::fmod(timeSeconds * proceduralHeartbeatBaseHz, 1.0f);
		const Float firstBeat = gaussianPulse(beatCycle, 0.08f, 0.028f);
		const Float secondBeat = gaussianPulse(beatCycle, 0.23f, 0.022f);
		const Float heartbeat = firstBeat * 0.66f + secondBeat * 0.40f;

		breathNoise = std::clamp(breathNoise + nextRandom() * 0.012f, -1.0f, 1.0f);
		breathDc    = breathDc * 0.999f + breathNoise * 0.001f;
		const Float bodyNoise = (breathNoise - breathDc) * 0.06f;
		procedural.breathSamples[frame] = heartbeat + bodyNoise;
	}
	normalizeWave(procedural.humSamples);
	normalizeWave(procedural.windSamples);
	normalizeWave(procedural.breathSamples);

	ma_audio_buffer_config humConfig = ma_audio_buffer_config_init(ma_format_f32, proceduralChannels, proceduralLoopFrames, procedural.humSamples.data(), nullptr);
	if(const ma_result result = ma_audio_buffer_init(&humConfig, &procedural.humBuffer); result != MA_SUCCESS)
	{
		logWarning("Failed to initialize procedural hum buffer, error code: " + toString(result));
		return false;
	}

	ma_audio_buffer_config windConfig = ma_audio_buffer_config_init(ma_format_f32, proceduralChannels, proceduralLoopFrames, procedural.windSamples.data(), nullptr);
	if(const ma_result result = ma_audio_buffer_init(&windConfig, &procedural.windBuffer); result != MA_SUCCESS)
	{
		ma_audio_buffer_uninit(&procedural.humBuffer);
		logWarning("Failed to initialize procedural wind buffer, error code: " + toString(result));
		return false;
	}

	if(const ma_result result = ma_sound_init_from_data_source(&impl->engine, &procedural.humBuffer, 0, nullptr, &procedural.humSound); result != MA_SUCCESS)
	{
		ma_audio_buffer_uninit(&procedural.windBuffer);
		ma_audio_buffer_uninit(&procedural.humBuffer);
		logWarning("Failed to initialize procedural hum sound, error code: " + toString(result));
		return false;
	}

	const Float initialWindCutoff = windLpfCutoffHz(0.0f);
	procedural.windLpfTargetCutoff.store(initialWindCutoff, std::memory_order_relaxed);
	procedural.windLpfCutoffSmooth      = initialWindCutoff;
	procedural.windDebugLpfCutoffSmooth   = initialWindCutoff;
	procedural.windLpfState             = 0.0f;
	procedural.windDebugLpfState          = 0.0f;

	procedural.windFilteredSource.owner = &procedural;
	ma_data_source_config windSourceConfig = ma_data_source_config_init();
	windSourceConfig.vtable               = &g_windFilteredDataSourceVtable;
	if(const ma_result result = ma_data_source_init(&windSourceConfig, &procedural.windFilteredSource.base); result != MA_SUCCESS)
	{
		ma_sound_uninit(&procedural.humSound);
		ma_audio_buffer_uninit(&procedural.windBuffer);
		ma_audio_buffer_uninit(&procedural.humBuffer);
		logWarning("Failed to initialize procedural wind filtered source, error code: " + toString(result));
		return false;
	}

	if(const ma_result result = ma_sound_init_from_data_source(&impl->engine, &procedural.windFilteredSource.base, 0, nullptr, &procedural.windSound); result != MA_SUCCESS)
	{
		ma_data_source_uninit(&procedural.windFilteredSource.base);
		ma_sound_uninit(&procedural.humSound);
		ma_audio_buffer_uninit(&procedural.windBuffer);
		ma_audio_buffer_uninit(&procedural.humBuffer);
		logWarning("Failed to initialize procedural wind sound, error code: " + toString(result));
		return false;
	}

	ma_audio_buffer_config breathConfig = ma_audio_buffer_config_init(ma_format_f32, proceduralChannels, proceduralLoopFrames, procedural.breathSamples.data(), nullptr);
	if(const ma_result result = ma_audio_buffer_init(&breathConfig, &procedural.breathBuffer); result != MA_SUCCESS)
	{
		ma_sound_uninit(&procedural.windSound);
		ma_sound_uninit(&procedural.humSound);
		ma_audio_buffer_uninit(&procedural.windBuffer);
		ma_audio_buffer_uninit(&procedural.humBuffer);
		logWarning("Failed to initialize procedural breath buffer, error code: " + toString(result));
		return false;
	}

	if(const ma_result result = ma_sound_init_from_data_source(&impl->engine, &procedural.breathBuffer, 0, nullptr, &procedural.breathSound); result != MA_SUCCESS)
	{
		ma_audio_buffer_uninit(&procedural.breathBuffer);
		ma_sound_uninit(&procedural.windSound);
		ma_sound_uninit(&procedural.humSound);
		ma_audio_buffer_uninit(&procedural.windBuffer);
		ma_audio_buffer_uninit(&procedural.humBuffer);
		logWarning("Failed to initialize procedural breath sound, error code: " + toString(result));
		return false;
	}

	ma_sound_set_looping(&procedural.humSound, MA_TRUE);
	ma_sound_set_looping(&procedural.windSound, MA_TRUE);
	ma_sound_set_looping(&procedural.breathSound, MA_TRUE);
	procedural.resetRuntimeState();

	procedural.initialized = true;
	procedural.enabled     = true;
	updateProceduralMotion(0.0f, 0.0f, 1.0f / 60.0f);
	return true;
}

void AudioEngine::destroyProceduralMotion()
{
	if(!impl) return;
	auto& procedural = impl->procedural;
	if(!procedural.initialized) return;

	ma_sound_stop(&procedural.humSound);
	ma_sound_stop(&procedural.windSound);
	ma_sound_stop(&procedural.breathSound);
	ma_sound_uninit(&procedural.humSound);
	ma_sound_uninit(&procedural.windSound);
	ma_sound_uninit(&procedural.breathSound);
	ma_data_source_uninit(&procedural.windFilteredSource.base);
	ma_audio_buffer_uninit(&procedural.humBuffer);
	ma_audio_buffer_uninit(&procedural.windBuffer);
	ma_audio_buffer_uninit(&procedural.breathBuffer);

	procedural.humSamples.clear();
	procedural.windSamples.clear();
	procedural.breathSamples.clear();
	procedural.initialized = false;
	procedural.enabled     = false;
	procedural.debugPhase  = 0.0f;
	procedural.debugCursor = 0;
	procedural.currentHumVolume    = 0.0f;
	procedural.currentWindVolume   = 0.0f;
	procedural.currentBreathVolume = 0.0f;
	procedural.currentBreathPitch  = heartbeatPitchFromSpeed(0.0f);
	procedural.currentHumPitch   = restHumPitch(procedural.tuning);
	procedural.currentWindPitch  = restWindPitch(procedural.tuning);
	procedural.smoothedDopplerDirection = 0.0f;
	std::fill(procedural.debugWaveform.begin(), procedural.debugWaveform.end(), 0.0f);
	std::fill(procedural.debugControl.begin(), procedural.debugControl.end(), 0.0f);
	std::fill(procedural.debugAudio.begin(), procedural.debugAudio.end(), 0.0f);
}

Bool AudioEngine::hasProceduralMotion() const
{
	return impl && impl->procedural.initialized;
}

void AudioEngine::setProceduralMotionEnabled(const Bool enabled)
{
	if(!impl || !impl->procedural.initialized) return;
	auto& procedural  = impl->procedural;
	procedural.enabled = enabled;
	if(enabled) return;

	ma_sound_stop(&procedural.humSound);
	ma_sound_stop(&procedural.windSound);
	ma_sound_stop(&procedural.breathSound);
}

void AudioEngine::setProceduralMotionTuning(const ProceduralMotionTuning& tuning)
{
	if(!impl) return;
	auto& target = impl->procedural.tuning;
	target.humNoiseMix   = std::clamp(tuning.humNoiseMix, 0.0f, 1.0f);
	target.humPitchScale = std::clamp(tuning.humPitchScale, 0.1f, 0.5f);
	target.windPitchScale = std::clamp(tuning.windPitchScale, 0.05f, 4.0f);
	target.humGain       = std::clamp(tuning.humGain, 0.0f, 3.0f);
	target.windGain      = std::clamp(tuning.windGain, 0.0f, 3.0f);
	target.breathGain    = std::clamp(tuning.breathGain, 0.0f, 3.0f);
	target.responseCurve = std::clamp(tuning.responseCurve, 0.0f, 0.5f);
	target.enableHum     = tuning.enableHum;
	target.enableWind    = tuning.enableWind;
	target.enableBreath  = tuning.enableBreath;
}

ProceduralMotionTuning AudioEngine::getProceduralMotionTuning() const
{
	if(!impl) return {};
	return impl->procedural.tuning;
}

void AudioEngine::updateProceduralMotion(const Float speed01, const Float dopplerDirection, const Float deltaTime)
{
	if(!impl || !impl->procedural.initialized) return;
	auto& procedural = impl->procedural;
	const ProceduralMotionTuning tuning = procedural.tuning;

	const Float speed        = std::clamp(speed01, 0.0f, 1.0f);
	const Float dt           = max(deltaTime, 0.0001f);
	const Float windSpeed    = smoothWindSpeedWithRelease(procedural, speed, dt);
	const Float heartbeatSpeed = smoothHeartbeatSpeedWithRelease(procedural, speed, dt);
	const Float responseExponent = 0.5f + tuning.responseCurve;
	const Float speedTone    = pow(speed, responseExponent);
	const Float speedNoise   = pow(windSpeed, max(0.5f, responseExponent * 0.75f));

	const Float targetDopplerDirection = std::clamp(dopplerDirection, -1.0f, 1.0f);
	const Float dopplerFollow = 1.0f - exp(-dt * 20.0f);
	procedural.smoothedDopplerDirection += (targetDopplerDirection - procedural.smoothedDopplerDirection) * dopplerFollow;
	// Direction-only Doppler on wind; hum pitch stays fixed (volume conveys speed).
	const Float windDopplerPitchMultiplier = 1.0f + procedural.smoothedDopplerDirection * 0.05f;

	const Float motionBlend  = smoothstep(proceduralRestSpeedThreshold, proceduralMotionSpeedThreshold, speed);
	const Float humEnabled   = tuning.enableHum ? 1.0f : 0.0f;
	const Float windEnabled  = tuning.enableWind ? 1.0f : 0.0f;
	const Float breathEnabled = tuning.enableBreath ? 1.0f : 0.0f;

	setWindLpfTarget(procedural, windSpeed);

	const Float targetHumVolume      = motionBlend * (0.03f + speedTone * 0.26f) * tuning.humGain * humEnabled;
	const Float targetWindVolume     = motionBlend * (0.004f + speedNoise * 0.32f) * tuning.windGain * windEnabled;
	const Float targetBreathVolume   = (0.19f + speedTone * 0.26f) * tuning.breathGain * breathEnabled;
	const Float targetBreathPitch    = heartbeatPitchFromSpeed(heartbeatSpeed);
	const Float targetHumPitch       = restHumPitch(tuning);
	// Brightness comes from the speed-driven low-pass; keep playback pitch stable to avoid resampler crackle.
	const Float targetWindPitch      = tuning.windPitchScale * windDopplerPitchMultiplier;

	const Float breathVolumeFollow = 1.0f - exp(-dt * 10.0f);
	const Float breathPitchFollow  = 1.0f - exp(-dt * 30.0f);
	procedural.currentBreathVolume += (targetBreathVolume - procedural.currentBreathVolume) * breathVolumeFollow;
	procedural.currentBreathPitch  += (targetBreathPitch - procedural.currentBreathPitch) * breathPitchFollow;

	const Float humVolumeFollow  = targetHumVolume > procedural.currentHumVolume ? (1.0f - exp(-dt * 60.0f)) : (1.0f - exp(-dt * 16.0f));
	const Float windPitchFollow  = 1.0f - exp(-dt * 12.0f);
	procedural.currentHumVolume += (targetHumVolume - procedural.currentHumVolume) * humVolumeFollow;
	procedural.currentHumPitch = targetHumPitch;
	ma_sound_set_volume(&procedural.humSound, procedural.currentHumVolume);
	ma_sound_set_pitch(&procedural.humSound, procedural.currentHumPitch);
	if(procedural.enabled && tuning.enableHum)
	{
		if(ma_sound_is_playing(&procedural.humSound) == MA_FALSE) ma_sound_start(&procedural.humSound);
	}
	else
	{
		ma_sound_stop(&procedural.humSound);
	}

	if(targetWindVolume >= procedural.currentWindVolume)
	{
		procedural.currentWindVolume = targetWindVolume;
	}
	else
	{
		const Float windReleaseFollow = 1.0f - exp(-dt * 14.0f);
		procedural.currentWindVolume += (targetWindVolume - procedural.currentWindVolume) * windReleaseFollow;
	}
	procedural.currentWindPitch  += (targetWindPitch - procedural.currentWindPitch) * windPitchFollow;
	ma_sound_set_volume(&procedural.windSound, procedural.currentWindVolume);
	ma_sound_set_pitch(&procedural.windSound, procedural.currentWindPitch);
	if(procedural.enabled && tuning.enableWind)
	{
		if(ma_sound_is_playing(&procedural.windSound) == MA_FALSE) ma_sound_start(&procedural.windSound);
	}
	else
	{
		ma_sound_stop(&procedural.windSound);
	}

	ma_sound_set_volume(&procedural.breathSound, procedural.currentBreathVolume);
	ma_sound_set_pitch(&procedural.breathSound, procedural.currentBreathPitch);
	if(procedural.enabled && tuning.enableBreath)
	{
		if(ma_sound_is_playing(&procedural.breathSound) == MA_FALSE) ma_sound_start(&procedural.breathSound);
	}
	else
	{
		ma_sound_stop(&procedural.breathSound);
	}

	constexpr Int samplesPerFrame = 8;
	const Float oscillatorFrequency = 110.0f * tuning.humPitchScale;
	const Float brightness          = 0.15f + speedNoise * 0.85f;
	const Float gain                = 0.15f + speedNoise * 0.85f;
	for(Int i = 0; i < samplesPerFrame; ++i)
	{
		procedural.debugPhase += oscillatorFrequency * twoPi * (dt / static_cast<Float>(samplesPerFrame));
		while(procedural.debugPhase > twoPi) procedural.debugPhase -= twoPi;

		Float sample = sin(procedural.debugPhase);
		sample += sin(procedural.debugPhase * 2.15f + speed * 2.5f) * brightness * 0.20f;
		sample += sin(procedural.debugPhase * 4.5f + speed * 1.8f) * 0.12f * speedNoise;
		const Float noiseBiasedSample = sample * (0.5f + speedNoise * 0.8f);
		sample = sample * (1.0f - tuning.humNoiseMix) + noiseBiasedSample * tuning.humNoiseMix;
		sample *= gain;

		if(!procedural.debugWaveform.empty() && !procedural.debugControl.empty() && !procedural.debugAudio.empty())
		{
			ma_uint64 humCursor = 0;
			ma_uint64 windCursor = 0;
			ma_uint64 breathCursor = 0;
			ma_sound_get_cursor_in_pcm_frames(&procedural.humSound, &humCursor);
			ma_sound_get_cursor_in_pcm_frames(&procedural.windSound, &windCursor);
			ma_sound_get_cursor_in_pcm_frames(&procedural.breathSound, &breathCursor);

			const Float humSample    = sampleLoop(procedural.humSamples, humCursor) * procedural.currentHumVolume;
			const Float windSample   = filterWindSample(procedural, sampleLoop(procedural.windSamples, windCursor), dt / static_cast<Float>(samplesPerFrame)) * procedural.currentWindVolume;
			const Float breathSample = sampleLoop(procedural.breathSamples, breathCursor) * procedural.currentBreathVolume;
			const Float mixedAudio   = humSample + windSample + breathSample;

			const SizeT index               = procedural.debugCursor;
			procedural.debugWaveform[index] = sample;
			procedural.debugControl[index]  = speed;
			procedural.debugAudio[index]    = mixedAudio;
			procedural.debugHum[index]      = humSample;
			procedural.debugWind[index]     = windSample;
			procedural.debugBreath[index]   = breathSample;
			procedural.debugCursor          = (procedural.debugCursor + 1) % procedural.debugWaveform.size();
		}
	}
}

void AudioEngine::getProceduralMotionDebugWaveform(Vector<Float>& waveformOut, Vector<Float>& controlOut, Vector<Float>& audioOut, Vector<Float>* humOut, Vector<Float>* windOut, Vector<Float>* breathOut) const
{
	waveformOut.clear();
	controlOut.clear();
	audioOut.clear();
	if(humOut) humOut->clear();
	if(windOut) windOut->clear();
	if(breathOut) breathOut->clear();
	if(!impl || !impl->procedural.initialized) return;

	const auto& procedural = impl->procedural;
	exportRingBuffer(procedural.debugWaveform, procedural.debugCursor, waveformOut);
	exportRingBuffer(procedural.debugControl, procedural.debugCursor, controlOut);
	exportRingBuffer(procedural.debugAudio, procedural.debugCursor, audioOut);
	if(humOut) exportRingBuffer(procedural.debugHum, procedural.debugCursor, *humOut);
	if(windOut) exportRingBuffer(procedural.debugWind, procedural.debugCursor, *windOut);
	if(breathOut) exportRingBuffer(procedural.debugBreath, procedural.debugCursor, *breathOut);
}

