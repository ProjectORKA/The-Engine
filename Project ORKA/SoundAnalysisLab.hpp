#pragma once

#include "AudioEngine.hpp"
#include "Game.hpp"
#include "GPUTexture.hpp"
#include "UIButton.hpp"
#include "UISlider.hpp"
#include "WaveformGraph.hpp"

struct Window;

struct SoundAnalysisLabRenderer final : GameRenderer
{
	Vector<Float> samplesLeft;
	Vector<Float> samplesRight;
	Vector<Float> samplesMono;
	Vector<Path>  soundFiles;
	String        loadedFileName;
	UInt          sampleRate     = 0;
	UInt          nativeChannels = 0;
	Int           currentFileIndex = 0;

	Vector<Float> spectrogramL;
	Vector<Float> spectrogramR;
	Int           spectrogramColumns = 0;
	Int           spectrogramMelBins = 0;

	Vector<Byte> pixelsL;
	Vector<Byte> pixelsR;
	GPUTexture   textureL;
	GPUTexture   textureR;
	Bool         texturesReady    = false;
	Bool         spectrogramDirty = false;

	WaveformGraph waveformGraph;
	Vector<Float> waveformPreview;

	String playbackTrackId = "salPlayback";
	Bool   playing         = false;
	Float  volume          = 0.8f;
	Float  brightness      = 1.0f;
	Float  contrast        = 1.0f;

	Float zoom        = 1.0f;
	Float viewStart01 = 0.0f;
	Float viewSpan01  = 1.0f;
	Int   textureWidth  = 0;
	Int   textureHeight = 0;

	Vector<Float> stftRealL, stftImagL;
	Vector<Float> stftRealR, stftImagR;
	Int           stftColumns  = 0;
	Int           stftHalfBins = 0;

	String        synthTrackId    = "salSynth";
	Vector<Float> synthBuffer;
	Bool          scrubbing       = false;
	Bool          synthReady      = false;
	Bool          scrubUseFFT     = true;
	Float         scrubPosition01 = 0.0f;

	Bool pendingImport = false;

	InputEvent importAudioKey = InputEvent(InputType::KeyBoard, O, true);
	InputEvent prevFileKey    = InputEvent(InputType::KeyBoard, LeftBracket, true);
	InputEvent nextFileKey    = InputEvent(InputType::KeyBoard, RightBracket, true);
	InputEvent togglePlayKey  = InputEvent(InputType::KeyBoard, Space, true);
	InputEvent zoomInKey      = InputEvent(InputType::Scroll, 1, true);
	InputEvent zoomOutKey     = InputEvent(InputType::Scroll, 1, false);

	void refreshSoundFileList();
	void loadSoundFile(const Path& path);
	void loadSoundFileByIndex(Int index);
	void computeSpectrograms();
	void buildSpectrogramPixels();
	void uploadTextures(Renderer& renderer);
	void rebuildWaveformPreview();
	void syncViewFromZoom();
	void applyZoom(Float factor, Float focusX01);
	void computeSTFT();
	void startScrubSynthesis(Float pos01);
	void updateScrubSynthesis(Float pos01);
	void stopScrubSynthesis();
	Float getPlaybackPosition01();
	Float getTotalDurationSeconds() const;

	void create(Window& window) override;
	void destroy(Window& window) override;
	void update(Window& window) override;
	void connect(GameSimulation& simulation) override;
	void renderScene(Window& window) override;
	void inputEvent(Window& window, InputEvent& input) override;
	void renderInteractive(Window& window) override;
};

struct ImportAudioButton final : UIButton
{
	SoundAnalysisLabRenderer* renderer = nullptr;

	void doThis() override;
	void inputEvent(Window& window, InputEvent& input) override;
};

struct SoundAnalysisLabControls final : UIElement
{
	ImportAudioButton importButton;
	UISlider          fileIndexSlider;
	UISlider          volumeSlider;
	UISlider          brightnessSlider;
	UISlider          contrastSlider;

	explicit SoundAnalysisLabControls(SoundAnalysisLabRenderer& renderer);
};

struct SoundAnalysisLab
{
	void run() const;
};
