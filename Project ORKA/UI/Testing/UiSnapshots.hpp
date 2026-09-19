#pragma once

#include "../Core/UiDocument.hpp"

namespace Ui::Testing
{
	struct UiLayoutSnapshotOptions
	{
		UInt decimalPlaces = 3;
		Bool includeText = true;
		Bool includeSemantics = true;
		Bool includeDirtyFlags = true;
	};

	struct UiScreenshotGoldenMetadata
	{
		String goldenId;
		String bundleName;
		String scene;
		IVec2 logicalSize = IVec2(0);
		IVec2 framebufferSize = IVec2(0);
		Vec2 contentScale = Vec2(1);
		String renderer;
		String platform;
		String colorSpace = "sRGB";
		U64 bundleGeneration = 0;
		U64 deterministicSeed = 0;
		String expectedImageHash;
		String capturedImageHash;
		String captureArtifact;
		String note;
	};

	// Stable pre-order JSON intended for textual golden comparisons.
	[[nodiscard]] String serializeLayoutSnapshot(
		UiDocument& document, const UiLayoutSnapshotOptions& options = {});

	// Metadata-only seam. A renderer-owned capture system fills image/hash
	// fields; this utility never fabricates or reads back GPU pixels.
	[[nodiscard]] String serializeScreenshotGoldenMetadata(
		const UiScreenshotGoldenMetadata& metadata);
}
