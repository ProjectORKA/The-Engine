#include "SoundAnalysisLab.hpp"

#include "AudioEngine.hpp"
#include "FileSystem.hpp"
#include "ResourceManager.hpp"
#include "UserInterface.hpp"
#include "Window.hpp"
#include "Windows.hpp"

namespace
{
	constexpr SizeT waveformPreviewPoints = 512;
	constexpr SizeT spectrumFftSize         = 1024;
	constexpr SizeT spectrogramColumnCount  = 96;
	constexpr SizeT spectrogramBinCount     = 72;
	constexpr SizeT spectrogramFftSize      = 512;

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
					data[i + j].re             = even.re + t.re;
					data[i + j].im             = even.im + t.im;
					data[i + j + size / 2].re  = even.re - t.re;
					data[i + j + size / 2].im  = even.im - t.im;
					angle += angleStep;
				}
			}
		}
	}

	void computeMagnitudeSpectrum(const Vector<Float>& samples, const SizeT centerSample, const SizeT fftSize, Vector<Float>& magnitudesOut)
	{
		magnitudesOut.clear();
		if(samples.empty() || fftSize < 2) return;

		const SizeT halfSize = fftSize / 2;
		magnitudesOut.assign(halfSize, 0.0f);

		Vector<Complex> bins(fftSize);
		const Int startSample = static_cast<Int>(centerSample) - static_cast<Int>(halfSize);
		for(SizeT i = 0; i < fftSize; ++i)
		{
			const Int sampleIndex = startSample + static_cast<Int>(i);
			Float sample          = 0.0f;
			if(sampleIndex >= 0 && sampleIndex < static_cast<Int>(samples.size())) sample = samples[static_cast<SizeT>(sampleIndex)];

			const Float window = 0.5f * (1.0f - cos(6.28318530718f * static_cast<Float>(i) / static_cast<Float>(fftSize - 1)));
			bins[i].re         = sample * window;
			bins[i].im         = 0.0f;
		}

		fftInPlace(bins);

		Float peak = 0.0001f;
		for(SizeT bin = 0; bin < halfSize; ++bin)
		{
			const Float magnitude      = sqrt(bins[bin].re * bins[bin].re + bins[bin].im * bins[bin].im);
			magnitudesOut[bin]         = magnitude;
			peak                       = max(peak, magnitude);
		}

		for(Float& magnitude : magnitudesOut) magnitude /= peak;
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
			Float peak              = 0.0f;
			for(SizeT sampleIndex = bucketStart; sampleIndex < bucketEnd && sampleIndex < samples.size(); ++sampleIndex)
			{
				const Float sample = samples[sampleIndex];
				if(abs(sample) > abs(peak)) peak = sample;
			}
			out[point] = peak;
		}
	}

	Bool isAudioExtension(const Path& path)
	{
		const String extension = toLowerCase(path.extension().string());
		return extension == ".wav" || extension == ".ogg" || extension == ".flac" || extension == ".mp3";
	}

	Color spectrogramHeatmapColor(const Float magnitude)
	{
		const Float t = clamp(magnitude, 0.0f, 1.0f);
		if(t < 0.2f)
		{
			const Float local = t / 0.2f;
			return Color(0.04f + local * 0.04f, 0.05f + local * 0.15f, 0.18f + local * 0.72f, 0.55f + local * 0.35f);
		}
		if(t < 0.45f)
		{
			const Float local = (t - 0.2f) / 0.25f;
			return Color(0.08f + local * 0.02f, 0.20f + local * 0.65f, 0.90f - local * 0.15f, 0.90f);
		}
		if(t < 0.7f)
		{
			const Float local = (t - 0.45f) / 0.25f;
			return Color(0.10f + local * 0.85f, 0.85f + local * 0.10f, 0.75f - local * 0.55f, 0.95f);
		}
		const Float local = (t - 0.7f) / 0.3f;
		return Color(0.95f, 0.95f - local * 0.55f, 0.20f - local * 0.15f, 1.0f);
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
	samples.clear();
	sampleRate     = 0;
	loadedFileName = path.filename().string();

	DecodedAudioClip decoded;
	if(!audioEngine.decodeAudioFile(path, decoded))
	{
		loadedFileName = "Failed to load: " + loadedFileName;
		rebuildAnalysis();
		return;
	}

	samples    = std::move(decoded.monoSamples);
	sampleRate = decoded.sampleRate;
	scrub01    = 0.0f;
	pan01      = 0.0f;
	zoom       = 1.0f;
	syncViewFromControls();
	rebuildAnalysis();
}

void SoundAnalysisLabRenderer::loadSoundFileByIndex(const Int index)
{
	if(soundFiles.empty()) return;
	currentFileIndex = clamp(index, 0, static_cast<Int>(soundFiles.size()) - 1);
	loadSoundFile(soundFiles[static_cast<SizeT>(currentFileIndex)]);
}

void SoundAnalysisLabRenderer::syncViewFromControls()
{
	viewSpan01  = clamp(1.0f / max(zoom, 1.0f), 0.001f, 1.0f);
	viewStart01 = clamp(pan01, 0.0f, 1.0f - viewSpan01);
	pan01       = viewStart01;
}

Bool SoundAnalysisLabRenderer::isMouseOverWaveform(const Window& window) const
{
	const Vec2 mouse = Vec2(window.mousePosBotLeft);
	return mouse.x >= waveformGraphX && mouse.x <= waveformGraphX + static_cast<Float>(waveformGraphWidth)
		&& mouse.y >= waveformGraphY && mouse.y <= waveformGraphY + static_cast<Float>(waveformGraphHeight);
}

Bool SoundAnalysisLabRenderer::isMouseOverSpectrogram3D(const Window& window) const
{
	const Vec2 mouse = Vec2(window.mousePosBotLeft);
	return mouse.x >= spectrogramGraphLeft && mouse.x <= spectrogramGraphLeft + spectrogramGraphWidth
		&& mouse.y >= spectrogramGraphY && mouse.y <= spectrogramGraphY + static_cast<Float>(spectrogramGraphHeight);
}

Bool SoundAnalysisLabRenderer::ownsSpectralCapture() const
{
	return ui.capturing && ui.focusedElement == this;
}

void SoundAnalysisLabRenderer::updateSpectralCamera(Window& window)
{
	const Bool spectralActive = isMouseOverSpectrogram3D(window) || ownsSpectralCapture();
	if(!spectralActive) return;

	const Float delta = max(window.renderer.deltaTime(), 0.0001f);
	if(ownsSpectralCapture()) spectralCamera.rotate(window.mouseDelta * DVec2(spectralMouseSensitivity));

	Vec3 movement(0.0f);
	if(window.pressed(moveForward)) movement += spectralCamera.getForwardVector();
	if(window.pressed(moveBackward)) movement -= spectralCamera.getForwardVector();
	if(window.pressed(moveRight)) movement += spectralCamera.getRightVector();
	if(window.pressed(moveLeft)) movement -= spectralCamera.getRightVector();
	if(window.pressed(moveUp)) movement += spectralCamera.getUpVector();
	if(window.pressed(moveDown)) movement -= spectralCamera.getUpVector();
	if(length(movement) > 0.0f)
	{
		movement = normalize(movement) * spectralMoveSpeed * delta;
		spectralCamera.setPosition(spectralCamera.getPosition() + movement);
	}
}

void SoundAnalysisLabRenderer::applyWaveformZoom(const Float factor, const Window& window)
{
	const Vec2 mouse   = Vec2(window.mousePosBotLeft);
	const Float localX = clamp((mouse.x - waveformGraphX) / static_cast<Float>(max(waveformGraphWidth, 1)), 0.0f, 1.0f);
	const Float focus01 = clamp(viewStart01 + localX * viewSpan01, 0.0f, 1.0f);

	zoom        = clamp(zoom * factor, 1.0f, 256.0f);
	viewSpan01  = clamp(1.0f / max(zoom, 1.0f), 0.001f, 1.0f);
	viewStart01 = clamp(focus01 - localX * viewSpan01, 0.0f, 1.0f - viewSpan01);
	pan01       = viewStart01;
	rebuildAnalysis();
}

void SoundAnalysisLabRenderer::rebuildAnalysis()
{
	syncViewFromControls();

	waveformPreview.clear();
	scrubMarker.clear();
	spectrumMagnitudes.clear();
	spectrogramMagnitudes.clear();

	if(samples.empty())
	{
		waveformGraph.setSamples(waveformPreview, scrubMarker, waveformPreview);
		return;
	}

	const SizeT totalSamples    = samples.size();
	const SizeT viewStartSample = static_cast<SizeT>(viewStart01 * static_cast<Float>(totalSamples - 1));
	const SizeT viewEndSample   = static_cast<SizeT>(min(static_cast<Float>(totalSamples), (viewStart01 + viewSpan01) * static_cast<Float>(totalSamples - 1) + 1.0f));
	const SizeT visibleSamples  = max(viewEndSample, viewStartSample + 1) - viewStartSample;

	downsamplePeakEnvelope(samples, viewStartSample, max(viewEndSample, viewStartSample + 1), waveformPreviewPoints, waveformPreview);

	scrubMarker.assign(waveformPreviewPoints, 0.0f);
	const SizeT scrubSample = static_cast<SizeT>(scrub01 * static_cast<Float>(totalSamples - 1));
	if(scrubSample >= viewStartSample && scrubSample < viewStartSample + visibleSamples && !waveformPreview.empty())
	{
		const SizeT markerIndex = ((scrubSample - viewStartSample) * (waveformPreviewPoints - 1)) / max(visibleSamples, static_cast<SizeT>(1));
		scrubMarker[min(markerIndex, waveformPreviewPoints - 1)] = 1.0f;
		if(markerIndex > 0) scrubMarker[markerIndex - 1] = 0.35f;
		if(markerIndex + 1 < waveformPreviewPoints) scrubMarker[markerIndex + 1] = 0.35f;
	}

	computeMagnitudeSpectrum(samples, scrubSample, spectrumFftSize, spectrumMagnitudes);

	spectrogramMagnitudes.assign(spectrogramColumnCount * spectrogramBinCount, 0.0f);
	if(visibleSamples > spectrogramFftSize)
	{
		for(SizeT column = 0; column < spectrogramColumnCount; ++column)
		{
			const SizeT columnCenter = viewStartSample + ((column + 1) * visibleSamples) / (spectrogramColumnCount + 1);
			Vector<Float> columnSpectrum;
			computeMagnitudeSpectrum(samples, columnCenter, spectrogramFftSize, columnSpectrum);
			if(columnSpectrum.empty()) continue;

			for(SizeT bin = 0; bin < spectrogramBinCount; ++bin)
			{
				const SizeT sourceBin = (bin * columnSpectrum.size()) / spectrogramBinCount;
				spectrogramMagnitudes[column * spectrogramBinCount + bin] = columnSpectrum[min(sourceBin, columnSpectrum.size() - 1)];
			}
		}
	}

	waveformGraph.setSamples(waveformPreview, scrubMarker, waveformPreview);
}

void SoundAnalysisLabRenderer::destroy(Window& window)
{
	GameRenderer::destroy(window);
}

void SoundAnalysisLabRenderer::connect(GameSimulation& simulation) {}

void SoundAnalysisLabRenderer::create(Window& window)
{
	GameRenderer::create(window);
	audioEngine.create();
	spectralCamera.setFieldOfView(55.0f);
	spectralCamera.setNearClipValue(0.01f);
	spectralCamera.setFarClipValue(50.0f);
	spectralCamera.setPosition(0.5f, -2.2f, 1.8f);
	spectralCamera.setRotation(0.42, 0.0, 0.0);
	refreshSoundFileList();
	if(!soundFiles.empty()) loadSoundFileByIndex(0);
	else rebuildAnalysis();
}

void SoundAnalysisLabRenderer::update(Window& window)
{
	if(pendingImport)
	{
		pendingImport = false;
		const Path selectedFile = pickAudioFile();
		if(!selectedFile.empty()) loadSoundFile(selectedFile);
	}

	updateSpectralCamera(window);
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
	if(isMouseOverSpectrogram3D(window))
	{
		if(input == captureSpectralView && !ui.capturing)
		{
			ui.capture(*this);
			input.consume();
		}
		if(input == zoomInKey)
		{
			spectralCamera.setPosition(spectralCamera.getPosition() + spectralCamera.getForwardVector() * 0.08f);
			input.consume();
		}
		if(input == zoomOutKey)
		{
			spectralCamera.setPosition(spectralCamera.getPosition() - spectralCamera.getForwardVector() * 0.08f);
			input.consume();
		}
	}
	if(ownsSpectralCapture())
	{
		if(input == zoomInKey)
		{
			spectralCamera.setPosition(spectralCamera.getPosition() + spectralCamera.getForwardVector() * 0.08f);
			input.consume();
		}
		if(input == zoomOutKey)
		{
			spectralCamera.setPosition(spectralCamera.getPosition() - spectralCamera.getForwardVector() * 0.08f);
			input.consume();
		}
		if(input == releaseSpectralView)
		{
			ui.releaseCapture();
			input.consume();
		}
	}
	if(isMouseOverWaveform(window))
	{
		if(input == zoomInKey)
		{
			applyWaveformZoom(1.1f, window);
			input.consume();
		}
		if(input == zoomOutKey)
		{
			applyWaveformZoom(0.9f, window);
			input.consume();
		}
	}
}

void SoundAnalysisLabRenderer::renderInteractive(Window& window)
{
	if(samples.empty()) return;

	const Vec2 mouse = Vec2(window.mousePosBotLeft);
	const Bool mouseInWaveform = mouse.x >= waveformGraphX && mouse.x <= waveformGraphX + static_cast<Float>(waveformGraphWidth)
		&& mouse.y >= waveformGraphY && mouse.y <= waveformGraphY + static_cast<Float>(waveformGraphHeight);

	if(mouseInWaveform && window.pressed(InputId(InputType::Mouse, Lmb)))
	{
		const Float localX = clamp((mouse.x - waveformGraphX) / static_cast<Float>(waveformGraphWidth), 0.0f, 1.0f);
		scrub01 = clamp(viewStart01 + localX * viewSpan01, 0.0f, 1.0f);
		rebuildAnalysis();
	}
}

void SoundAnalysisLabRenderer::renderSpectrogram3D(Renderer& r)
{
	if(spectrogramMagnitudes.empty()) return;

	const Float aspect = spectrogramGraphWidth / max(static_cast<Float>(spectrogramGraphHeight), 1.0f);
	const IVec2 viewportPos(static_cast<Int>(spectrogramGraphLeft), static_cast<Int>(spectrogramGraphY));
	const IVec2 viewportSize(static_cast<Int>(spectrogramGraphWidth), spectrogramGraphHeight);

	r.setRenderRegion(viewportPos, viewportSize);
	r.setDepthTest(true);
	r.setCulling(false);
	r.setWireframeMode(false);
	r.setAlphaBlending(true);

	r.useShader("color");
	const Vec3 target = spectrogramOrigin;
	r.uniforms().setVMatrix(spectralCamera.viewMatrix());
	r.uniforms().setPMatrix(spectralCamera.projectionMatrix(max(aspect, 0.01f)));
	r.uniforms().setCameraVec(Vec4(spectralCamera.getForwardVector(), 1.0f));
	r.uniforms().setCameraPos(Vec4(spectralCamera.getPosition(), 1.0f));
	r.uniforms().setMMatrix(Matrix(1));

	auto drawAxis = [&](const Vec3& start, const Vec3& end, const Color& color)
	{
		Vec3Vector axisLine{start, end};
		r.fill(color);
		r.line(axisLine);
	};

	drawAxis(target, target + Vec3(1.08f, 0.0f, 0.0f), Color(1.0f, 0.25f, 0.25f, 1.0f));
	drawAxis(target, target + Vec3(0.0f, 1.08f, 0.0f), Color(0.25f, 1.0f, 0.35f, 1.0f));
	drawAxis(target, target + Vec3(0.0f, 0.0f, 0.75f), Color(0.35f, 0.55f, 1.0f, 1.0f));

	const Float cellX = 1.0f / static_cast<Float>(spectrogramColumnCount);
	const Float cellY = 1.0f / static_cast<Float>(spectrogramBinCount);
	const Float heightScale = 0.62f;

	auto sampleHeight = [&](const SizeT column, const SizeT bin) -> Float
	{
		if(column >= spectrogramColumnCount || bin >= spectrogramBinCount) return 0.0f;
		return clamp(spectrogramMagnitudes[column * spectrogramBinCount + bin], 0.0f, 1.0f) * heightScale;
	};

	auto sampleMagnitude = [&](const SizeT column, const SizeT bin) -> Float
	{
		if(column >= spectrogramColumnCount || bin >= spectrogramBinCount) return 0.0f;
		return clamp(spectrogramMagnitudes[column * spectrogramBinCount + bin], 0.0f, 1.0f);
	};

	auto terrainPoint = [&](const SizeT column, const SizeT bin) -> Vec3
	{
		const Float x = static_cast<Float>(column) * cellX + cellX * 0.5f;
		const Float y = static_cast<Float>(bin) * cellY + cellY * 0.5f;
		return target + Vec3(x, y, sampleHeight(column, bin));
	};

	for(SizeT column = 0; column < spectrogramColumnCount; ++column)
	{
		Vec3Vector ridgeLine;
		ridgeLine.reserve(spectrogramBinCount);
		for(SizeT bin = 0; bin < spectrogramBinCount; ++bin)
		{
			ridgeLine.emplace_back(terrainPoint(column, bin));
		}
		const Float ridgeMagnitude = sampleMagnitude(column, spectrogramBinCount / 2);
		const Color ridgeColor = spectrogramHeatmapColor(ridgeMagnitude);
		r.fill(ridgeColor.r, ridgeColor.g, ridgeColor.b, 0.85f);
		r.line(ridgeLine);
	}

	for(SizeT bin = 0; bin < spectrogramBinCount; ++bin)
	{
		Vec3Vector crossLine;
		crossLine.reserve(spectrogramColumnCount);
		for(SizeT column = 0; column < spectrogramColumnCount; ++column)
		{
			crossLine.emplace_back(terrainPoint(column, bin));
		}
		const Float crossMagnitude = sampleMagnitude(spectrogramColumnCount / 2, bin);
		const Color crossColor = spectrogramHeatmapColor(crossMagnitude * 0.85f);
		r.fill(crossColor.r, crossColor.g, crossColor.b, 0.45f);
		r.line(crossLine);
	}

	for(SizeT column = 0; column < spectrogramColumnCount; ++column)
	{
		for(SizeT bin = 0; bin < spectrogramBinCount; ++bin)
		{
			const Float magnitude = sampleMagnitude(column, bin);
			if(magnitude <= 0.02f) continue;

			const Vec3 base = target + Vec3(static_cast<Float>(column) * cellX + cellX * 0.5f, static_cast<Float>(bin) * cellY + cellY * 0.5f, 0.0f);
			const Float columnHeight = sampleHeight(column, bin);
			const Color heatColor = spectrogramHeatmapColor(magnitude);
			r.fill(heatColor.r, heatColor.g, heatColor.b, heatColor.a);
			r.uniforms().setMMatrix(scale(translate(Matrix(1), base + Vec3(0.0f, 0.0f, columnHeight * 0.5f)), Vec3(cellX * 0.92f, cellY * 0.92f, max(columnHeight, 0.01f))));
			r.plane();
		}
	}

	r.setRenderRegion(Vec2(0), getSize());
	r.setDepthTest(false);
	r.screenSpaceFromBottomLeft();
}

void SoundAnalysisLabRenderer::renderScene(Window& window)
{
	Renderer& r = window.renderer;

	framebuffer.clear(r);

	r.setWireframeMode(false);
	r.setCulling(false);
	r.setDepthTest(false);
	r.setAlphaBlending(true);
	r.screenSpaceFromBottomLeft();
	r.useShader("color");

	const IVec2 canvas = getSize();
	r.fill(0.06f, 0.06f, 0.06f, 1.0f);
	r.uniforms().setMMatrix(matrixFromPositionAndSize(canvas.x * 0.5f, canvas.y * 0.5f, static_cast<Float>(canvas.x), static_cast<Float>(canvas.y)));
	r.plane();

	const Float graphX       = 30.0f;
	const Float graphGap     = 12.0f;
	const Float labelGap     = 18.0f;
	const Float stackBottom  = 30.0f;
	const Float headerHeight = 130.0f;
	const Float graphWidth   = static_cast<Float>(max(canvas.x - 60, 240));
	const Float usableHeight = max(static_cast<Float>(canvas.y) - headerHeight - stackBottom - labelGap * 3.0f - graphGap * 2.0f, 220.0f);
	const Int   spectrogramHeight = max(static_cast<Int>(usableHeight * 0.34f), 90);
	const Int   spectrumHeight    = max(static_cast<Int>(usableHeight * 0.22f), 70);
	const Int   waveformHeight    = max(static_cast<Int>(usableHeight - static_cast<Float>(spectrogramHeight + spectrumHeight)), 90);

	waveformGraphX      = graphX;
	spectrogramGraphLeft = graphX;
	spectrogramGraphWidth = graphWidth;
	spectrogramGraphHeight = spectrogramHeight;
	spectrogramGraphY = stackBottom;
	spectrumGraphHeight = spectrumHeight;
	spectrumGraphY = spectrogramGraphY + static_cast<Float>(spectrogramHeight) + graphGap + labelGap;
	waveformGraphY = spectrumGraphY + static_cast<Float>(spectrumHeight) + graphGap + labelGap;
	waveformGraphWidth = static_cast<Int>(graphWidth);
	waveformGraphHeight = waveformHeight;

	waveformGraph.positionFromBottomLeft = Vec2(waveformGraphX, waveformGraphY);
	waveformGraph.width                  = waveformGraphWidth;
	waveformGraph.height                 = waveformGraphHeight;
	waveformGraph.render(window);

	auto drawGraphBackground = [&](const Float y, const Float panelHeight)
	{
		r.useShader("color");
		r.fill(0.0f, 0.0f, 0.0f, 0.45f);
		r.uniforms().setMMatrix(matrixFromPositionAndSize(graphX + graphWidth * 0.5f, y + panelHeight * 0.5f, graphWidth, panelHeight));
		r.plane();
	};

	drawGraphBackground(spectrumGraphY, static_cast<Float>(spectrumGraphHeight));
	if(!spectrumMagnitudes.empty())
	{
		Vec2Vector spectrumLine;
		spectrumLine.reserve(spectrumMagnitudes.size());
		const Float stepX = spectrumMagnitudes.size() > 1 ? graphWidth / static_cast<Float>(spectrumMagnitudes.size() - 1) : 0.0f;
		for(SizeT i = 0; i < spectrumMagnitudes.size(); ++i)
		{
			const Float normalized = clamp(spectrumMagnitudes[i], 0.0f, 1.0f);
			spectrumLine.emplace_back(graphX + static_cast<Float>(i) * stepX, spectrumGraphY + normalized * static_cast<Float>(spectrumGraphHeight));
		}
		r.uniforms().setMMatrix(Matrix(1));
		r.fill(0.2f, 1.0f, 0.45f, 1.0f);
		r.line(spectrumLine);
	}

	drawGraphBackground(spectrogramGraphY, static_cast<Float>(spectrogramGraphHeight));
	renderSpectrogram3D(r);

	r.useShader("text");
	r.uniforms().setMMatrix(Matrix(1));
	r.fill(1.0f, 1.0f, 1.0f, 1.0f);
	r.fontSize(22.0f);
	r.letterSpacing(0.55f);
	r.textRenderSystem.alignText(Alignment::left, Alignment::bottom);
	r.text("Sound Analysis Lab", Vec2(30.0f, static_cast<Float>(canvas.y - 30.0f)));

	r.fontSize(16.0f);
	r.text("File: " + loadedFileName, Vec2(30.0f, static_cast<Float>(canvas.y - 58.0f)));
	r.text("Sample rate: " + toString(sampleRate) + " Hz | Samples: " + toString(samples.size()), Vec2(30.0f, static_cast<Float>(canvas.y - 82.0f)));
	r.text("[O] or Import button | [[ ] / ] ] prev/next bundled sound", Vec2(30.0f, static_cast<Float>(canvas.y - 106.0f)));
	r.text("Waveform (magenta=audio, orange=scrub) | Click to scrub | Scroll to zoom at cursor", Vec2(30.0f, waveformGraphY - 8.0f));
	r.text("FFT spectrum at scrub position", Vec2(30.0f, spectrumGraphY - 8.0f));
	r.text("3D spectrogram (time=X red, frequency=Y green, magnitude=Z blue) | LMB capture + drag look | WASDQE fly | RMB release | Scroll dolly", Vec2(30.0f, spectrogramGraphY - 8.0f));
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
	setWidth(360);
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
	importButton.setWidth(336);
	importButton.setHeight(36);
	importButton.background(Color(1.0f, 1.0f, 1.0f, 0.08f));
	importButton.hoveredBackground(Color(1.0f, 1.0f, 1.0f, 0.16f));
	importButton.activeBackground(Color(1.0f, 1.0f, 1.0f, 0.28f));
	importButton.color(Color(1.0f, 1.0f, 1.0f, 1.0f));
	importButton.setText("Import Audio File...");

	fileIndexSlider
		.setLabel("Bundled Sound Index")
		.setRange(0.0f, max(static_cast<Float>(renderer.soundFiles.size()) - 1.0f, 0.0f))
		.setValue(static_cast<Float>(renderer.currentFileIndex))
		.setOnValueChanged([&renderer](const Float value)
		{
			renderer.loadSoundFileByIndex(static_cast<Int>(round(value)));
		});
	fileIndexSlider.growX(false);
	fileIndexSlider.setWidth(336);

	zoomSlider
		.setLabel("Zoom")
		.setRange(1.0f, 256.0f)
		.bind(renderer.zoom)
		.setOnValueChanged([&renderer](const Float)
		{
			renderer.syncViewFromControls();
			renderer.rebuildAnalysis();
		});
	zoomSlider.growX(false);
	zoomSlider.setWidth(336);

	panSlider
		.setLabel("View Pan")
		.setRange(0.0f, 1.0f)
		.bind(renderer.pan01)
		.setOnValueChanged([&renderer](const Float)
		{
			renderer.syncViewFromControls();
			renderer.rebuildAnalysis();
		});
	panSlider.growX(false);
	panSlider.setWidth(336);

	scrubSlider
		.setLabel("Scrub Position")
		.setRange(0.0f, 1.0f)
		.bind(renderer.scrub01)
		.setOnValueChanged([&renderer](const Float)
		{
			renderer.rebuildAnalysis();
		});
	scrubSlider.growX(false);
	scrubSlider.setWidth(336);

	addElement(importButton);
	addElement(fileIndexSlider);
	addElement(zoomSlider);
	addElement(panSlider);
	addElement(scrubSlider);
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
