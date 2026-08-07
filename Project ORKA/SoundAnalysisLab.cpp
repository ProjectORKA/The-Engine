#include "SoundAnalysisLab.hpp"

#include "FileSystem.hpp"
#include "ResourceManager.hpp"
#include "UserInterface.hpp"
#include "Window.hpp"
#include "Windows.hpp"

namespace
{
	constexpr Int   spectrogramFftSize    = 4096;
	constexpr Int   defaultMelBins        = 512;
	constexpr Float dbFloor               = -100.0f;
	constexpr Float dbCeiling             = 0.0f;
	constexpr Int   maxSpectrogramColumns = 32768;
	constexpr Int   displayTexWidth       = 4096;
	constexpr SizeT waveformPoints        = 1024;

	struct Complex
	{
		Float re = 0.0f;
		Float im = 0.0f;
	};

	UInt reverseBits(UInt value, const UInt bits)
	{
		UInt reversed = 0;
		for(UInt bit = 0; bit < bits; ++bit)
		{
			reversed = (reversed << 1) | (value & 1u);
			value >>= 1;
		}
		return reversed;
	}

	void fftInPlace(Vector<Complex>& data)
	{
		const SizeT n = data.size();
		if(n < 2) return;

		UInt levels = 0;
		for(UInt temp = static_cast<UInt>(n); temp > 1u; temp >>= 1) ++levels;

		for(UInt i = 0; i < static_cast<UInt>(n); ++i)
		{
			const UInt j = reverseBits(i, levels);
			if(j > i) std::swap(data[i], data[j]);
		}

		for(UInt size = 2; size <= static_cast<UInt>(n); size <<= 1)
		{
			const Float angleStep = -6.28318530718f / static_cast<Float>(size);
			for(UInt i = 0; i < static_cast<UInt>(n); i += size)
			{
				Float angle = 0.0f;
				for(UInt j = 0; j < size / 2; ++j)
				{
					const Complex even = data[i + j];
					const Complex odd  = data[i + j + size / 2];
					const Float   cosA = cos(angle);
					const Float   sinA = sin(angle);
					const Complex t{odd.re * cosA - odd.im * sinA, odd.re * sinA + odd.im * cosA};
					data[i + j].re            = even.re + t.re;
					data[i + j].im            = even.im + t.im;
					data[i + j + size / 2].re = even.re - t.re;
					data[i + j + size / 2].im = even.im - t.im;
					angle += angleStep;
				}
			}
		}
	}

	Float hzToMel(const Float hz) { return 2595.0f * log10(1.0f + hz / 700.0f); }
	Float melToHz(const Float mel) { return 700.0f * (pow(10.0f, mel / 2595.0f) - 1.0f); }

	Bool isAudioExtension(const Path& path)
	{
		const String ext = toLowerCase(path.extension().string());
		return ext == ".wav" || ext == ".ogg" || ext == ".flac" || ext == ".mp3";
	}

	void spectrogramColor(const Float t, Byte& rOut, Byte& gOut, Byte& bOut)
	{
		struct Stop
		{
			Float pos, r, g, b;
		};

		constexpr Stop stops[] = {
			{0.00f, 0.001f, 0.000f, 0.014f},
			{0.07f, 0.024f, 0.015f, 0.095f},
			{0.15f, 0.075f, 0.028f, 0.240f},
			{0.25f, 0.190f, 0.050f, 0.410f},
			{0.35f, 0.320f, 0.060f, 0.450f},
			{0.45f, 0.460f, 0.100f, 0.400f},
			{0.55f, 0.590f, 0.175f, 0.310f},
			{0.65f, 0.720f, 0.270f, 0.200f},
			{0.75f, 0.840f, 0.395f, 0.100f},
			{0.85f, 0.935f, 0.555f, 0.020f},
			{0.93f, 0.975f, 0.760f, 0.155f},
			{1.00f, 0.988f, 0.998f, 0.645f},
		};
		constexpr Int numStops = sizeof(stops) / sizeof(stops[0]);

		const Float clamped = clamp(t, 0.0f, 1.0f);

		Int upper = 1;
		while(upper < numStops - 1 && stops[upper].pos < clamped) ++upper;
		const Int lower = upper - 1;

		const Float range = stops[upper].pos - stops[lower].pos;
		const Float local = range > 0.0001f ? (clamped - stops[lower].pos) / range : 0.0f;

		rOut = static_cast<Byte>(clamp((stops[lower].r + (stops[upper].r - stops[lower].r) * local) * 255.0f, 0.0f, 255.0f));
		gOut = static_cast<Byte>(clamp((stops[lower].g + (stops[upper].g - stops[lower].g) * local) * 255.0f, 0.0f, 255.0f));
		bOut = static_cast<Byte>(clamp((stops[lower].b + (stops[upper].b - stops[lower].b) * local) * 255.0f, 0.0f, 255.0f));
	}

	void computeMelSpectrogram(const Vector<Float>& samples, const UInt sampleRate, Vector<Float>& spectrogramOut, Int& columnsOut, Int& binsOut)
	{
		spectrogramOut.clear();
		columnsOut = 0;
		binsOut    = defaultMelBins;

		if(samples.empty() || sampleRate == 0) return;

		const SizeT totalSamples = samples.size();
		if(totalSamples < 2) return;

		Int hopSize = max(spectrogramFftSize / 32, static_cast<Int>(totalSamples / maxSpectrogramColumns));
		Int hop     = 1;
		while(hop < hopSize) hop <<= 1;
		hopSize = min(hop, spectrogramFftSize / 2);

		Int numColumns = max(1, static_cast<Int>(totalSamples / hopSize) + 1);
		numColumns     = min(numColumns, maxSpectrogramColumns);
		columnsOut     = numColumns;

		const Float nyquist = static_cast<Float>(sampleRate) * 0.5f;
		const Float melMax  = hzToMel(nyquist);
		const Float melMin  = hzToMel(20.0f);

		Vector<Int> melBinEdges(defaultMelBins + 1);
		for(Int i = 0; i <= defaultMelBins; ++i)
		{
			const Float mel = melMin + static_cast<Float>(i) * (melMax - melMin) / static_cast<Float>(defaultMelBins);
			melBinEdges[i]  = clamp(static_cast<Int>(melToHz(mel) * static_cast<Float>(spectrogramFftSize) / static_cast<Float>(sampleRate)), 0, spectrogramFftSize / 2 - 1);
		}

		Vector<Float> window(spectrogramFftSize);
		for(Int i = 0; i < spectrogramFftSize; ++i)
			window[i] = 0.5f * (1.0f - cos(6.28318530718f * static_cast<Float>(i) / static_cast<Float>(spectrogramFftSize - 1)));

		spectrogramOut.resize(static_cast<SizeT>(numColumns) * defaultMelBins, dbFloor);

		Vector<Complex> fftBuf(spectrogramFftSize);

		const Float halfN     = static_cast<Float>(spectrogramFftSize) * 0.5f;
		const Float powerNorm = 1.0f / (halfN * halfN);

		for(Int col = 0; col < numColumns; ++col)
		{
			const Float centerF     = static_cast<Float>(col) * static_cast<Float>(totalSamples - 1) / static_cast<Float>(max(numColumns - 1, 1));
			const Int   windowStart = static_cast<Int>(centerF) - spectrogramFftSize / 2;

			for(Int i = 0; i < spectrogramFftSize; ++i)
			{
				const Int sampleIdx = windowStart + i;
				Float     sample    = 0.0f;
				if(sampleIdx >= 0 && static_cast<SizeT>(sampleIdx) < totalSamples)
					sample = samples[static_cast<SizeT>(sampleIdx)];
				fftBuf[i].re = sample * window[i];
				fftBuf[i].im = 0.0f;
			}

			fftInPlace(fftBuf);

			const Int halfFft = spectrogramFftSize / 2;
			for(Int bin = 0; bin < defaultMelBins; ++bin)
			{
				const Int startBin = melBinEdges[bin];
				const Int endBin   = melBinEdges[bin + 1];

				Float power = 0.0f;
				Int   count = 0;
				for(Int k = startBin; k <= endBin && k < halfFft; ++k)
				{
					power += (fftBuf[k].re * fftBuf[k].re + fftBuf[k].im * fftBuf[k].im) * powerNorm;
					++count;
				}
				if(count > 0) power /= static_cast<Float>(count);

				Float db = dbFloor;
				if(power > 1.0e-20f) db = 10.0f * log10(power);
				db = clamp(db, dbFloor, dbCeiling);

				spectrogramOut[static_cast<SizeT>(col) * defaultMelBins + bin] = db;
			}
		}
	}

	void ifftInPlace(Vector<Complex>& data)
	{
		const SizeT n = data.size();
		if(n < 2) return;

		for(auto& c : data) c.im = -c.im;
		fftInPlace(data);
		const Float scale = 1.0f / static_cast<Float>(n);
		for(auto& c : data)
		{
			c.re *= scale;
			c.im = -c.im * scale;
		}
	}

	constexpr Int maxStftColumns = 4096;

	void computeFullSTFT(
		const Vector<Float>& samples,
		const UInt           sampleRate,
		Vector<Float>&       realOut,
		Vector<Float>&       imagOut,
		Int&                 columnsOut,
		Int&                 halfBinsOut)
	{
		realOut.clear();
		imagOut.clear();
		columnsOut  = 0;
		halfBinsOut = spectrogramFftSize / 2 + 1;

		if(samples.empty() || sampleRate == 0) return;

		const SizeT totalSamples = samples.size();
		if(totalSamples < 2) return;

		Int stftHop = max(spectrogramFftSize / 4, static_cast<Int>(totalSamples / maxStftColumns));
		Int hop     = 1;
		while(hop < stftHop) hop <<= 1;
		stftHop = min(hop, spectrogramFftSize);

		Int numColumns = max(1, static_cast<Int>(totalSamples / stftHop) + 1);
		numColumns     = min(numColumns, maxStftColumns);
		columnsOut     = numColumns;

		const Int halfBins = spectrogramFftSize / 2 + 1;
		realOut.resize(static_cast<SizeT>(numColumns) * halfBins, 0.0f);
		imagOut.resize(static_cast<SizeT>(numColumns) * halfBins, 0.0f);

		Vector<Float> window(spectrogramFftSize);
		for(Int i = 0; i < spectrogramFftSize; ++i)
			window[i] = 0.5f * (1.0f - cos(6.28318530718f * static_cast<Float>(i) / static_cast<Float>(spectrogramFftSize - 1)));

		Vector<Complex> fftBuf(spectrogramFftSize);

		for(Int col = 0; col < numColumns; ++col)
		{
			const Float centerF     = static_cast<Float>(col) * static_cast<Float>(totalSamples - 1) / static_cast<Float>(max(numColumns - 1, 1));
			const Int   windowStart = static_cast<Int>(centerF) - spectrogramFftSize / 2;

			for(Int i = 0; i < spectrogramFftSize; ++i)
			{
				const Int sampleIdx = windowStart + i;
				Float     sample    = 0.0f;
				if(sampleIdx >= 0 && static_cast<SizeT>(sampleIdx) < totalSamples)
					sample = samples[static_cast<SizeT>(sampleIdx)];
				fftBuf[i].re = sample * window[i];
				fftBuf[i].im = 0.0f;
			}

			fftInPlace(fftBuf);

			const SizeT colOffset = static_cast<SizeT>(col) * halfBins;
			for(Int k = 0; k < halfBins; ++k)
			{
				realOut[colOffset + k] = fftBuf[k].re;
				imagOut[colOffset + k] = fftBuf[k].im;
			}
		}
	}

	struct ColorEntry
	{
		Byte r, g, b;
	};

	void buildColorLUT(const Float contrastExp, ColorEntry* lut, const Int lutSize)
	{
		for(Int i = 0; i < lutSize; ++i)
		{
			Float t = static_cast<Float>(i) / static_cast<Float>(lutSize - 1);
			if(contrastExp != 1.0f) t = pow(t, contrastExp);
			spectrogramColor(t, lut[i].r, lut[i].g, lut[i].b);
		}
	}

	void buildDisplayPixels(
		const Vector<Float>& spectrogram,
		const Int            srcColumns,
		const Int            srcBins,
		const Float          viewStart,
		const Float          viewSpan,
		const Float          brightnessMul,
		const Float          contrastExp,
		const Int            outWidth,
		Vector<Byte>&        pixelsOut)
	{
		pixelsOut.resize(static_cast<SizeT>(outWidth) * srcBins * 4);

		constexpr Int lutSize = 256;
		ColorEntry    lut[lutSize];
		buildColorLUT(contrastExp, lut, lutSize);

		const Float dbRange = dbCeiling - dbFloor;

		for(Int dx = 0; dx < outWidth; ++dx)
		{
			const Float t01    = static_cast<Float>(dx) / static_cast<Float>(max(outWidth - 1, 1));
			const Float srcPos = (viewStart + t01 * viewSpan) * static_cast<Float>(srcColumns - 1);
			const Int   col0   = clamp(static_cast<Int>(srcPos), 0, srcColumns - 1);
			const Int   col1   = clamp(col0 + 1, 0, srcColumns - 1);
			const Float frac   = srcPos - static_cast<Float>(col0);

			for(Int bin = 0; bin < srcBins; ++bin)
			{
				const Float db0 = spectrogram[static_cast<SizeT>(col0) * srcBins + bin];
				const Float db1 = spectrogram[static_cast<SizeT>(col1) * srcBins + bin];
				const Float db  = db0 + (db1 - db0) * frac;
				const Float normalized = clamp((db - dbFloor) / dbRange * brightnessMul, 0.0f, 1.0f);

				const Int   lutIdx     = clamp(static_cast<Int>(normalized * static_cast<Float>(lutSize - 1)), 0, lutSize - 1);
				const SizeT pixelIndex = (static_cast<SizeT>(bin) * outWidth + dx) * 4;
				pixelsOut[pixelIndex]     = lut[lutIdx].r;
				pixelsOut[pixelIndex + 1] = lut[lutIdx].g;
				pixelsOut[pixelIndex + 2] = lut[lutIdx].b;
				pixelsOut[pixelIndex + 3] = 255;
			}
		}
	}

	void downsamplePeakEnvelope(const Vector<Float>& samples, const SizeT startSample, const SizeT endSample, const SizeT pointCount, Vector<Float>& out)
	{
		out.clear();
		if(samples.empty() || pointCount == 0 || endSample <= startSample) return;

		out.resize(pointCount, 0.0f);
		const SizeT visibleCount = endSample - startSample;
		for(SizeT point = 0; point < pointCount; ++point)
		{
			const SizeT bucketStart = startSample + (point * visibleCount) / pointCount;
			const SizeT bucketEnd   = startSample + ((point + 1) * visibleCount) / pointCount;
			Float       peak        = 0.0f;
			for(SizeT i = bucketStart; i < bucketEnd && i < samples.size(); ++i)
			{
				if(abs(samples[i]) > abs(peak)) peak = samples[i];
			}
			out[point] = peak;
		}
	}
}

void SoundAnalysisLabRenderer::refreshSoundFileList()
{
	soundFiles.clear();
	const Path soundsPath = resourceManager.getOrkaDataPath() / "Sounds";
	if(!doesPathExist(soundsPath)) return;

	for(const Path& path : getAllFilesInDirectory(soundsPath))
	{
		if(isAudioExtension(path)) soundFiles.push_back(path);
	}

	std::sort(soundFiles.begin(), soundFiles.end());
}

void SoundAnalysisLabRenderer::loadSoundFile(const Path& path)
{
	if(scrubbing) stopScrubSynthesis();
	if(synthReady)
	{
		audioEngine.pauseTrack(synthTrackId);
		audioEngine.unloadTrack(synthTrackId);
		synthReady = false;
	}
	if(playing)
	{
		audioEngine.pauseTrack(playbackTrackId);
		playing = false;
	}
	audioEngine.unloadTrack(playbackTrackId);

	samplesLeft.clear();
	samplesRight.clear();
	samplesMono.clear();
	sampleRate     = 0;
	nativeChannels = 0;
	loadedFileName = path.filename().string();

	DecodedAudioClip decoded;
	if(!audioEngine.decodeStereoAudioFile(path, decoded))
	{
		loadedFileName = "Failed: " + loadedFileName;
		spectrogramDirty = false;
		return;
	}

	samplesLeft    = std::move(decoded.leftSamples);
	samplesRight   = std::move(decoded.rightSamples);
	samplesMono    = std::move(decoded.monoSamples);
	sampleRate     = decoded.sampleRate;
	nativeChannels = decoded.channels;

	zoom        = 1.0f;
	viewStart01 = 0.0f;
	viewSpan01  = 1.0f;

	computeSpectrograms();
	buildSpectrogramPixels();
	rebuildWaveformPreview();
	spectrogramDirty = true;

	audioEngine.load(playbackTrackId, path, false);
	audioEngine.setTrackVolume(playbackTrackId, volume);
}

void SoundAnalysisLabRenderer::loadSoundFileByIndex(const Int index)
{
	if(soundFiles.empty()) return;
	currentFileIndex = clamp(index, 0, static_cast<Int>(soundFiles.size()) - 1);
	loadSoundFile(soundFiles[static_cast<SizeT>(currentFileIndex)]);
}

void SoundAnalysisLabRenderer::computeSpectrograms()
{
	computeMelSpectrogram(samplesLeft, sampleRate, spectrogramL, spectrogramColumns, spectrogramMelBins);
	Int colsR = 0, binsR = 0;
	computeMelSpectrogram(samplesRight, sampleRate, spectrogramR, colsR, binsR);
	computeSTFT();
}

void SoundAnalysisLabRenderer::computeSTFT()
{
	computeFullSTFT(samplesLeft, sampleRate, stftRealL, stftImagL, stftColumns, stftHalfBins);
	Int colsR = 0, binsR = 0;
	computeFullSTFT(samplesRight, sampleRate, stftRealR, stftImagR, colsR, binsR);
}

void SoundAnalysisLabRenderer::buildSpectrogramPixels()
{
	if(spectrogramColumns <= 0 || spectrogramMelBins <= 0)
	{
		pixelsL.clear();
		pixelsR.clear();
		return;
	}
	buildDisplayPixels(spectrogramL, spectrogramColumns, spectrogramMelBins, viewStart01, viewSpan01, brightness, contrast, displayTexWidth, pixelsL);
	buildDisplayPixels(spectrogramR, spectrogramColumns, spectrogramMelBins, viewStart01, viewSpan01, brightness, contrast, displayTexWidth, pixelsR);
}

void SoundAnalysisLabRenderer::uploadTextures(Renderer& renderer)
{
	if(spectrogramColumns <= 0 || spectrogramMelBins <= 0) return;

	if(texturesReady && (textureWidth != displayTexWidth || textureHeight != spectrogramMelBins))
	{
		textureL.unload(renderer);
		textureR.unload(renderer);
		texturesReady = false;
	}

	if(!texturesReady)
	{
		const IVec2 texSize(displayTexWidth, spectrogramMelBins);
		textureL.load(renderer, texSize, WritePixelsFormat::RGBA, DataType::Byte, Wrapping::Clamped);
		textureR.load(renderer, texSize, WritePixelsFormat::RGBA, DataType::Byte, Wrapping::Clamped);
		textureL.setFilters(renderer, Filter::Linear, Filter::Linear);
		textureR.setFilters(renderer, Filter::Linear, Filter::Linear);
		textureWidth  = displayTexWidth;
		textureHeight = spectrogramMelBins;
		texturesReady = true;
	}

	if(!pixelsL.empty())
	{
		OpenGL::Textures::apiTextureSubImage2D(
			renderer.openGlContext, textureL.getOpenGLID(),
			0, 0, 0, displayTexWidth, spectrogramMelBins,
			WritePixelsFormat::RGBA, DataType::Byte, pixelsL.data());
	}
	if(!pixelsR.empty())
	{
		OpenGL::Textures::apiTextureSubImage2D(
			renderer.openGlContext, textureR.getOpenGLID(),
			0, 0, 0, displayTexWidth, spectrogramMelBins,
			WritePixelsFormat::RGBA, DataType::Byte, pixelsR.data());
	}

	spectrogramDirty = false;
}

void SoundAnalysisLabRenderer::rebuildWaveformPreview()
{
	waveformPreview.clear();
	if(samplesMono.empty()) return;
	const SizeT totalSamples = samplesMono.size();
	const SizeT start = static_cast<SizeT>(viewStart01 * static_cast<Float>(totalSamples));
	const SizeT end   = min(static_cast<SizeT>((viewStart01 + viewSpan01) * static_cast<Float>(totalSamples)), totalSamples);
	downsamplePeakEnvelope(samplesMono, start, end, waveformPoints, waveformPreview);
	Vector<Float> empty(waveformPreview.size(), 0.0f);
	waveformGraph.setSamples(waveformPreview, empty, waveformPreview);
}

void SoundAnalysisLabRenderer::syncViewFromZoom()
{
	viewSpan01  = clamp(1.0f / max(zoom, 1.0f), 0.001f, 1.0f);
	viewStart01 = clamp(viewStart01, 0.0f, 1.0f - viewSpan01);
}

void SoundAnalysisLabRenderer::applyZoom(const Float factor, const Float focusX01)
{
	const Float focus01 = viewStart01 + focusX01 * viewSpan01;
	zoom = clamp(zoom * factor, 1.0f, 512.0f);
	syncViewFromZoom();
	viewStart01 = clamp(focus01 - focusX01 * viewSpan01, 0.0f, 1.0f - viewSpan01);
	rebuildWaveformPreview();
	buildSpectrogramPixels();
	spectrogramDirty = true;
}

void SoundAnalysisLabRenderer::updateScrubSynthesis(const Float pos01)
{
	const SizeT totalSamples = samplesMono.size();
	if(totalSamples == 0) return;

	if(scrubUseFFT && stftColumns > 0 && stftHalfBins > 0)
	{
		constexpr Int fftN = spectrogramFftSize;
		constexpr Int hop  = fftN / 4;
		const Int     hb   = stftHalfBins;

		const Float colF = clamp(pos01, 0.0f, 1.0f) * static_cast<Float>(stftColumns - 1);
		const Int   col0 = clamp(static_cast<Int>(colF), 0, stftColumns - 1);
		const Int   col1 = clamp(col0 + 1, 0, stftColumns - 1);
		const Float frac = colF - static_cast<Float>(col0);

		Vector<Complex> spectrum(fftN, {0.0f, 0.0f});
		for(Int k = 0; k < hb; ++k)
		{
			const SizeT i0 = static_cast<SizeT>(col0) * hb + k;
			const SizeT i1 = static_cast<SizeT>(col1) * hb + k;

			const Float reL = stftRealL[i0] + (stftRealL[i1] - stftRealL[i0]) * frac;
			const Float imL = stftImagL[i0] + (stftImagL[i1] - stftImagL[i0]) * frac;
			const Float reR = stftRealR[i0] + (stftRealR[i1] - stftRealR[i0]) * frac;
			const Float imR = stftImagR[i0] + (stftImagR[i1] - stftImagR[i0]) * frac;

			spectrum[k].re = (reL + reR) * 0.5f;
			spectrum[k].im = (imL + imR) * 0.5f;
		}

		for(Int k = 1; k < fftN / 2; ++k)
		{
			spectrum[fftN - k].re =  spectrum[k].re;
			spectrum[fftN - k].im = -spectrum[k].im;
		}

		ifftInPlace(spectrum);

		synthBuffer.assign(fftN, 0.0f);
		for(Int copy = 0; copy < 4; ++copy)
		{
			const Int offset = copy * hop;
			for(Int i = 0; i < fftN; ++i)
				synthBuffer[(offset + i) % fftN] += spectrum[i].re;
		}

		const Float scale = volume * 0.5f;
		for(Float& s : synthBuffer) s *= scale;
	}
	else
	{
		constexpr SizeT chunkLen = 16384;
		constexpr SizeT fadeLen  = 512;

		const auto center  = static_cast<long long>(clamp(pos01, 0.0f, 1.0f) * static_cast<Float>(totalSamples - 1));
		const auto halfLen = static_cast<long long>(chunkLen / 2);
		const auto startIdx = center - halfLen;
		const auto maxIdx  = static_cast<long long>(totalSamples) - 1;

		auto readSample = [&](const long long idx) -> Float
		{
			return samplesMono[static_cast<SizeT>(clamp(idx, static_cast<long long>(0), maxIdx))];
		};

		synthBuffer.resize(chunkLen);
		for(SizeT i = 0; i < chunkLen; ++i)
			synthBuffer[i] = readSample(startIdx + static_cast<long long>(i)) * volume;

		for(SizeT i = 0; i < fadeLen; ++i)
		{
			const Float tail = readSample(startIdx + static_cast<long long>(chunkLen + i)) * volume;
			const Float t    = static_cast<Float>(i) / static_cast<Float>(fadeLen);
			synthBuffer[i]   = synthBuffer[i] * t + tail * (1.0f - t);
		}
	}
}

void SoundAnalysisLabRenderer::startScrubSynthesis(const Float pos01)
{
	scrubbing       = true;
	scrubPosition01 = pos01;

	if(playing)
	{
		audioEngine.pauseTrack(playbackTrackId);
		playing = false;
	}

	updateScrubSynthesis(pos01);

	if(synthReady)
	{
		audioEngine.pauseTrack(synthTrackId);
		audioEngine.unloadTrack(synthTrackId);
		synthReady = false;
	}
	audioEngine.loadFromSamples(synthTrackId, synthBuffer, sampleRate, true);
	synthReady = true;
	audioEngine.setTrackVolume(synthTrackId, 1.0f);
	audioEngine.startTrack(synthTrackId);
}

void SoundAnalysisLabRenderer::stopScrubSynthesis()
{
	if(!scrubbing) return;
	scrubbing = false;
	audioEngine.pauseTrack(synthTrackId);

	if(!samplesMono.empty())
	{
		const UInt seekFrame = static_cast<UInt>(clamp(scrubPosition01, 0.0f, 1.0f) * static_cast<Float>(samplesMono.size()));
		audioEngine.seekTrack(playbackTrackId, seekFrame);
	}
}

Float SoundAnalysisLabRenderer::getPlaybackPosition01()
{
	if(scrubbing) return scrubPosition01;
	if(samplesMono.empty()) return 0.0f;
	const UInt framePos = audioEngine.getTrackPositionInFrames(playbackTrackId);
	return clamp(static_cast<Float>(framePos) / static_cast<Float>(samplesMono.size()), 0.0f, 1.0f);
}

Float SoundAnalysisLabRenderer::getTotalDurationSeconds() const
{
	if(sampleRate == 0 || samplesMono.empty()) return 0.0f;
	return static_cast<Float>(samplesMono.size()) / static_cast<Float>(sampleRate);
}

void SoundAnalysisLabRenderer::create(Window& window)
{
	GameRenderer::create(window);
	audioEngine.create();
	refreshSoundFileList();
	if(!soundFiles.empty()) loadSoundFileByIndex(0);
}

void SoundAnalysisLabRenderer::destroy(Window& window)
{
	if(scrubbing) stopScrubSynthesis();
	if(synthReady)
	{
		audioEngine.pauseTrack(synthTrackId);
		audioEngine.unloadTrack(synthTrackId);
	}
	if(playing) audioEngine.pauseTrack(playbackTrackId);
	audioEngine.unloadTrack(playbackTrackId);
	if(texturesReady)
	{
		textureL.unload(window.renderer);
		textureR.unload(window.renderer);
		texturesReady = false;
	}
	GameRenderer::destroy(window);
}

void SoundAnalysisLabRenderer::connect(GameSimulation& simulation) {}

void SoundAnalysisLabRenderer::update(Window& window)
{
	if(pendingImport)
	{
		pendingImport = false;
		const Path selectedFile = pickAudioFile();
		if(!selectedFile.empty()) loadSoundFile(selectedFile);
	}

	if(playing && !audioEngine.isTrackPlaying(playbackTrackId))
		playing = false;
}

void SoundAnalysisLabRenderer::inputEvent(Window& window, InputEvent& input)
{
	if(input == importAudioKey)
	{
		pendingImport = true;
		input.consume();
	}
	if(input == prevFileKey && !soundFiles.empty())
	{
		loadSoundFileByIndex(currentFileIndex - 1);
		input.consume();
	}
	if(input == nextFileKey && !soundFiles.empty())
	{
		loadSoundFileByIndex(currentFileIndex + 1);
		input.consume();
	}
	if(input == togglePlayKey && !samplesMono.empty())
	{
		if(playing)
		{
			audioEngine.pauseTrack(playbackTrackId);
			playing = false;
		}
		else
		{
			audioEngine.startTrack(playbackTrackId);
			playing = true;
		}
		input.consume();
	}

	if(!samplesMono.empty())
	{
		const IVec2 canvas = getSize();
		const Vec2  mouse  = Vec2(window.mousePosBotLeft);
		constexpr Float margin = 20.0f;
		const Float graphW = max(static_cast<Float>(canvas.x) - margin * 2.0f, 100.0f);

		const Bool inGraphX = mouse.x >= margin && mouse.x <= margin + graphW;
		if(inGraphX)
		{
			const Float focusX01 = clamp((mouse.x - margin) / graphW, 0.0f, 1.0f);
			if(input == zoomInKey)
			{
				applyZoom(1.15f, focusX01);
				input.consume();
			}
			if(input == zoomOutKey)
			{
				applyZoom(1.0f / 1.15f, focusX01);
				input.consume();
			}
		}
	}
}

void SoundAnalysisLabRenderer::renderInteractive(Window& window)
{
	if(samplesMono.empty()) return;

	const IVec2 canvas = getSize();
	const Vec2  mouse  = Vec2(window.mousePosBotLeft);

	constexpr Float headerH   = 80.0f;
	constexpr Float waveformH = 60.0f;
	constexpr Float gap       = 6.0f;
	constexpr Float margin    = 20.0f;
	const Float     graphW    = max(static_cast<Float>(canvas.x) - margin * 2.0f, 100.0f);
	const Float     availH    = max(static_cast<Float>(canvas.y) - headerH - waveformH - gap * 2.0f - margin, 120.0f);
	const Float     specH     = max(availH * 0.5f, 60.0f);

	const Float waveY  = margin;
	const Float specRY = waveY + waveformH + gap;
	const Float specLY = specRY + specH + gap;

	const Bool inGraphArea =
		mouse.x >= margin && mouse.x <= margin + graphW &&
		mouse.y >= waveY && mouse.y <= specLY + specH;

	const Bool mouseDown = window.pressed(InputId(InputType::Mouse, Lmb));

	if(scrubbing)
	{
		if(mouseDown)
		{
			const Float localX  = clamp((mouse.x - margin) / graphW, 0.0f, 1.0f);
			const Float filePos = viewStart01 + localX * viewSpan01;
			scrubPosition01 = filePos;
			updateScrubSynthesis(filePos);
			audioEngine.updateTrackSamples(synthTrackId, synthBuffer);
		}
		else
		{
			stopScrubSynthesis();
		}
	}
	else if(inGraphArea && mouseDown)
	{
		const Float localX  = clamp((mouse.x - margin) / graphW, 0.0f, 1.0f);
		const Float filePos = viewStart01 + localX * viewSpan01;
		startScrubSynthesis(filePos);
	}
}

void SoundAnalysisLabRenderer::renderScene(Window& window)
{
	Renderer& r = window.renderer;

	if(spectrogramDirty) uploadTextures(r);

	framebuffer.clear(r);
	r.setWireframeMode(false);
	r.setCulling(false);
	r.setDepthTest(false);
	r.setAlphaBlending(true);
	r.screenSpaceFromBottomLeft();

	const IVec2 canvas = getSize();

	r.useShader("color");
	r.fill(0.04f, 0.04f, 0.05f, 1.0f);
	r.uniforms().setMMatrix(matrixFromPositionAndSize(
		canvas.x * 0.5f, canvas.y * 0.5f,
		static_cast<Float>(canvas.x), static_cast<Float>(canvas.y)));
	r.plane();

	constexpr Float headerH   = 80.0f;
	constexpr Float waveformH = 60.0f;
	constexpr Float gap       = 6.0f;
	constexpr Float margin    = 20.0f;
	const Float     graphW    = max(static_cast<Float>(canvas.x) - margin * 2.0f, 100.0f);
	const Float     availH    = max(static_cast<Float>(canvas.y) - headerH - waveformH - gap * 2.0f - margin, 120.0f);
	const Float     specH     = max(availH * 0.5f, 60.0f);

	const Float waveY  = margin;
	const Float specRY = waveY + waveformH + gap;
	const Float specLY = specRY + specH + gap;

	if(texturesReady && spectrogramColumns > 0)
	{
		r.useShader("texture");
		r.uniforms().setCustomInt1(0);

		r.uniforms().setMMatrix(matrixFromPositionAndSize(margin + graphW * 0.5f, specLY + specH * 0.5f, graphW, specH));
		textureL.useTextureInSlot(r, 0);
		r.plane();

		r.uniforms().setMMatrix(matrixFromPositionAndSize(margin + graphW * 0.5f, specRY + specH * 0.5f, graphW, specH));
		textureR.useTextureInSlot(r, 0);
		r.plane();
	}
	else
	{
		r.useShader("color");
		r.fill(0.0f, 0.0f, 0.02f, 0.6f);
		r.uniforms().setMMatrix(matrixFromPositionAndSize(
			margin + graphW * 0.5f, specLY + specH * 0.5f, graphW, specH));
		r.plane();
		r.uniforms().setMMatrix(matrixFromPositionAndSize(
			margin + graphW * 0.5f, specRY + specH * 0.5f, graphW, specH));
		r.plane();
	}

	waveformGraph.positionFromBottomLeft = Vec2(margin, waveY);
	waveformGraph.width                  = static_cast<Int>(graphW);
	waveformGraph.height                 = static_cast<Int>(waveformH);
	waveformGraph.render(window);

	if(!samplesMono.empty())
	{
		const Float pos01      = getPlaybackPosition01();
		const Float visiblePos = (pos01 - viewStart01) / max(viewSpan01, 0.001f);

		if(visiblePos >= 0.0f && visiblePos <= 1.0f)
		{
			const Float lineX = margin + visiblePos * graphW;

			r.useShader("color");
			r.uniforms().setMMatrix(Matrix(1));
			r.fill(1.0f, 1.0f, 1.0f, 0.9f);

			Vec2Vector playheadLine;
			playheadLine.emplace_back(lineX, waveY);
			playheadLine.emplace_back(lineX, specLY + specH);
			r.line(playheadLine);

			r.fill(0.0f, 0.0f, 0.0f, 0.3f);
			Vec2Vector shadowLine;
			shadowLine.emplace_back(lineX + 1.0f, waveY);
			shadowLine.emplace_back(lineX + 1.0f, specLY + specH);
			r.line(shadowLine);
		}
	}

	r.useShader("text");
	r.uniforms().setMMatrix(Matrix(1));
	r.fill(1.0f, 1.0f, 1.0f, 1.0f);
	r.fontSize(20.0f);
	r.letterSpacing(0.55f);
	r.textRenderSystem.alignText(Alignment::left, Alignment::bottom);
	r.text("Sound Analysis Lab", Vec2(margin, static_cast<Float>(canvas.y) - 26.0f));

	r.fontSize(13.0f);
	const Float  duration     = getTotalDurationSeconds();
	const Int    durationSec  = static_cast<Int>(duration);
	const Int    durationFrac = static_cast<Int>(fmod(duration, 1.0f) * 10.0f);
	const String chLabel      = nativeChannels == 1 ? "mono" : "stereo";
	const String info = "File: " + loadedFileName + "  |  " + toString(sampleRate) + " Hz  |  " + chLabel + "  |  " + toString(durationSec) + "." + toString(durationFrac) + " s";
	r.text(info, Vec2(margin, static_cast<Float>(canvas.y) - 50.0f));

	r.fontSize(11.0f);
	r.fill(1.0f, 1.0f, 1.0f, 0.55f);
	r.text("[O] Import   [[] []] Navigate   [Space] Play/Pause   Click to scrub", Vec2(margin, static_cast<Float>(canvas.y) - 70.0f));

	r.fontSize(12.0f);
	r.fill(1.0f, 1.0f, 1.0f, 0.75f);
	r.text("L", Vec2(margin + 5.0f, specLY + specH - 16.0f));
	r.text("R", Vec2(margin + 5.0f, specRY + specH - 16.0f));

	r.fontSize(9.0f);
	r.fill(1.0f, 1.0f, 1.0f, 0.4f);

	const Float nyquist = sampleRate > 0 ? static_cast<Float>(sampleRate) * 0.5f : 22050.0f;

	auto drawFreqLabels = [&](const Float bottom, const Float height)
	{
		constexpr Float labelFreqs[] = {100.0f, 500.0f, 1000.0f, 2000.0f, 5000.0f, 10000.0f, 20000.0f};
		const Float     melMax       = hzToMel(nyquist);
		const Float     melMin       = hzToMel(20.0f);
		for(const Float hz : labelFreqs)
		{
			if(hz > nyquist) break;
			const Float mel = hzToMel(hz);
			const Float y01 = clamp((mel - melMin) / (melMax - melMin), 0.0f, 1.0f);
			const Float y   = bottom + y01 * height;

			String label;
			if(hz >= 1000.0f)
				label = toString(static_cast<Int>(hz / 1000.0f)) + "k";
			else
				label = toString(static_cast<Int>(hz));

			r.text(label, Vec2(margin + graphW + 3.0f, y - 4.0f));
		}
	};

	drawFreqLabels(specLY, specH);
	drawFreqLabels(specRY, specH);

	if(!samplesMono.empty())
	{
		const Float playPos = getPlaybackPosition01();
		const Float timeSec = playPos * duration;
		const Int   mm      = static_cast<Int>(timeSec) / 60;
		const Int   ss      = static_cast<Int>(timeSec) % 60;
		const Int   ms      = static_cast<Int>(fmod(timeSec, 1.0f) * 10.0f);
		const String timeStr = toString(mm) + ":" + (ss < 10 ? "0" : "") + toString(ss) + "." + toString(ms);
		r.fontSize(12.0f);
		r.fill(1.0f, 1.0f, 1.0f, 0.8f);
		r.textRenderSystem.alignText(Alignment::right, Alignment::bottom);
		r.text(timeStr, Vec2(margin + graphW, static_cast<Float>(canvas.y) - 50.0f));
		r.textRenderSystem.alignText(Alignment::left, Alignment::bottom);
	}
}

void ImportAudioButton::doThis()
{
	if(renderer) renderer->pendingImport = true;
}

void ImportAudioButton::inputEvent(Window& window, InputEvent& input)
{
	UIButton::inputEvent(window, input);
}

SoundAnalysisLabControls::SoundAnalysisLabControls(SoundAnalysisLabRenderer& renderer)
{
	setName("SoundAnalysisLabControls");
	column();
	growX(false);
	growY(true);
	setWidth(300);
	gap(10);
	padding(12);
	background(Color(0.02f, 0.02f, 0.025f, 0.95f));
	hoveredBackground(Color(0.02f, 0.02f, 0.025f, 0.95f));
	activeBackground(Color(0.02f, 0.02f, 0.025f, 0.95f));
	border(1, Color(1, 1, 1, 0.12f));

	importButton.renderer = &renderer;
	importButton.setName("ImportAudioButton");
	importButton.padding(10);
	importButton.growX(false);
	importButton.setWidth(276);
	importButton.setHeight(36);
	importButton.background(Color(1.0f, 1.0f, 1.0f, 0.08f));
	importButton.hoveredBackground(Color(1.0f, 1.0f, 1.0f, 0.16f));
	importButton.activeBackground(Color(1.0f, 1.0f, 1.0f, 0.28f));
	importButton.color(Color(1.0f, 1.0f, 1.0f, 1.0f));
	importButton.setText("Import Audio File...");

	fileIndexSlider
		.setLabel("Bundled Sound")
		.setRange(0.0f, max(static_cast<Float>(renderer.soundFiles.size()) - 1.0f, 0.0f))
		.setValue(static_cast<Float>(renderer.currentFileIndex))
		.setOnValueChanged([&renderer](const Float value)
		{
			renderer.loadSoundFileByIndex(static_cast<Int>(round(value)));
		});
	fileIndexSlider.growX(false);
	fileIndexSlider.setWidth(276);

	volumeSlider
		.setLabel("Volume")
		.setRange(0.0f, 1.5f)
		.bind(renderer.volume)
		.setOnValueChanged([&renderer](const Float value)
		{
			renderer.volume = value;
			audioEngine.setTrackVolume(renderer.playbackTrackId, value);
		});
	volumeSlider.growX(false);
	volumeSlider.setWidth(276);

	brightnessSlider
		.setLabel("Brightness")
		.setRange(0.1f, 10.0f)
		.bind(renderer.brightness)
		.setOnValueChanged([&renderer](const Float value)
		{
			renderer.brightness = value;
			renderer.buildSpectrogramPixels();
			renderer.spectrogramDirty = true;
		});
	brightnessSlider.growX(false);
	brightnessSlider.setWidth(276);

	contrastSlider
		.setLabel("Contrast")
		.setRange(0.1f, 50.0f)
		.bind(renderer.contrast)
		.setOnValueChanged([&renderer](const Float value)
		{
			renderer.contrast = value;
			renderer.buildSpectrogramPixels();
			renderer.spectrogramDirty = true;
		});
	contrastSlider.growX(false);
	contrastSlider.setWidth(276);

	addElement(importButton);
	addElement(fileIndexSlider);
	addElement(volumeSlider);
	addElement(brightnessSlider);
	addElement(contrastSlider);
}

void SoundAnalysisLab::run() const
{
	SoundAnalysisLabRenderer renderer;
	SoundAnalysisLabControls controls(renderer);

	Window& window = ui.window("Sound Analysis Lab", settings.defaultWindowWidth, settings.defaultWindowHeight, true, true, WindowState::Windowed);
	window.row();
	window.gap(10);
	window.padding(10);

	renderer.growX(true);
	renderer.growY(true);
	controls.growY(true);

	window.addElement(renderer);
	window.addElement(controls);
	ui.run();
}
