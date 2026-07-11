#pragma once

#include "AudioEngine.hpp"
#include "Camera.hpp"
#include "Game.hpp"
#include "UIButton.hpp"
#include "UISlider.hpp"
#include "WaveformGraph.hpp"

struct Window;

struct SoundAnalysisLabRenderer final : GameRenderer
{
	WaveformGraph waveformGraph;

	Vector<Float> samples;
	Vector<Path>  soundFiles;
	String        loadedFileName;
	UInt          sampleRate = 0;

	Float zoom          = 1.0f;
	Float pan01         = 0.0f;
	Float scrub01       = 0.0f;
	Float viewStart01   = 0.0f;
	Float viewSpan01    = 1.0f;
	Int   currentFileIndex = 0;

	Vector<Float> waveformPreview;
	Vector<Float> scrubMarker;
	Vector<Float> spectrumMagnitudes;
	Vector<Float> spectrogramMagnitudes;

	Float waveformGraphX = 30.0f;
	Float waveformGraphY = 280.0f;
	Int   waveformGraphWidth  = 560;
	Int   waveformGraphHeight = 180;
	Float spectrumGraphY      = 90.0f;
	Int   spectrumGraphHeight = 120;
	Float spectrogramGraphY      = 20.0f;
	Int   spectrogramGraphHeight = 120;
	Float spectrogramGraphLeft   = 30.0f;
	Float spectrogramGraphWidth  = 560.0f;

	Camera spectralCamera;
	Vec3   spectrogramOrigin     = Vec3(0.5f, 0.5f, 0.0f);
	Float  spectralMoveSpeed     = 0.75f;
	Double spectralMouseSensitivity = 0.0035;
	Bool  pendingImport          = false;

	InputEvent importAudioKey     = InputEvent(InputType::KeyBoard, O, true);
	InputEvent prevFileKey        = InputEvent(InputType::KeyBoard, LeftBracket, true);
	InputEvent nextFileKey        = InputEvent(InputType::KeyBoard, RightBracket, true);
	InputEvent zoomInKey          = InputEvent(InputType::Scroll, 1, true);
	InputEvent zoomOutKey         = InputEvent(InputType::Scroll, 1, false);
	InputEvent captureSpectralView = InputEvent(InputType::Mouse, Lmb, true);
	InputEvent releaseSpectralView = InputEvent(InputType::Mouse, Rmb, false);
	InputId    moveForward        = InputId(InputType::KeyBoard, W);
	InputId    moveBackward       = InputId(InputType::KeyBoard, S);
	InputId    moveRight          = InputId(InputType::KeyBoard, D);
	InputId    moveLeft           = InputId(InputType::KeyBoard, A);
	InputId    moveUp             = InputId(InputType::KeyBoard, E);
	InputId    moveDown           = InputId(InputType::KeyBoard, Q);

	Bool isMouseOverWaveform(const Window& window) const;
	Bool isMouseOverSpectrogram3D(const Window& window) const;
	Bool ownsSpectralCapture() const;

	void applyWaveformZoom(const Float factor, const Window& window);
	void updateSpectralCamera(Window& window);
	void renderSpectrogram3D(Renderer& renderer);
	void refreshSoundFileList();
	void loadSoundFile(const Path& path);
	void loadSoundFileByIndex(Int index);
	void syncViewFromControls();
	void rebuildAnalysis();
	void update(Window& window) override;
	void create(Window& window) override;
	void destroy(Window& window) override;
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
	UISlider fileIndexSlider;
	UISlider zoomSlider;
	UISlider panSlider;
	UISlider scrubSlider;

	explicit SoundAnalysisLabControls(SoundAnalysisLabRenderer& renderer);
};

struct SoundAnalysisLab
{
	void run() const;
};
