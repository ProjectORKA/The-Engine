#pragma once

#include "../Core/UiTypes.hpp"

#include <optional>
#include <span>
#include <variant>

namespace Ui
{
	using UiResourceId = U64;
	constexpr UInt invalidUiResourceIndex = maxUInt;

	struct UiLinearColor
	{
		Vec4 premultiplied = Vec4(0);

		[[nodiscard]] static UiLinearColor transparent();
		[[nodiscard]] static UiLinearColor fromStraightLinear(Vec4 color);
		[[nodiscard]] static UiLinearColor fromPremultipliedLinear(Vec4 color);
		[[nodiscard]] static UiLinearColor fromStraightSrgb(Vec4 color);
	};

	struct UiRenderBudgets
	{
		SizeT maximumCommands = 65536;
		SizeT maximumInstances = 262144;
		SizeT maximumResources = 4096;
		SizeT maximumMeshVertices = 1048576;
		SizeT maximumMeshIndices = 3145728;
		UInt maximumStackDepth = 256;
	};

	enum class UiBlendMode : Byte
	{
		PremultipliedAlpha,
		Additive,
		Opaque,
	};

	enum class UiPipeline : Byte
	{
		SolidQuad,
		GradientQuad,
		ImageQuad,
		GrayscaleGlyph,
		MsdfGlyph,
		ColorGlyph,
		PathMesh,
	};

	enum class UiTexturePool : Byte
	{
		None,
		Images,
		GrayscaleGlyphs,
		DistanceFieldGlyphs,
		ColorGlyphs,
	};

	struct UiResourceRef
	{
		UiResourceId id = 0;
		UiTexturePool pool = UiTexturePool::None;
		UInt generation = 0;

		constexpr auto operator<=>(const UiResourceRef&) const = default;
	};

	// Renderer-owned resource reserved for the dependency-free built-in font.
	constexpr UiResourceRef builtinFallbackFontResource()
	{
		return { 0x4f524b4155494646ULL, UiTexturePool::GrayscaleGlyphs, 1 };
	}

	struct UiQuadGeometry
	{
		Vec2 position = Vec2(0);
		Vec2 size = Vec2(0);
		Vec4 cornerRadii = Vec4(0);
	};

	struct UiGradient
	{
		Vec2 start = Vec2(0);
		Vec2 end = Vec2(1, 0);
		UiLinearColor startColor;
		UiLinearColor endColor;
	};

	struct UiImagePaint
	{
		UInt resourceIndex = invalidUiResourceIndex;
		Vec4 uvRect = Vec4(0, 0, 1, 1);
		UiLinearColor tint = UiLinearColor::fromStraightLinear(Vec4(1));
	};

	struct UiGlyphPaint
	{
		UInt resourceIndex = invalidUiResourceIndex;
		Vec4 uvRect = Vec4(0, 0, 1, 1);
		UiLinearColor color = UiLinearColor::fromStraightLinear(Vec4(1));
		Float distanceRange = 0;
	};

	struct UiQuadInstance
	{
		UiQuadGeometry geometry;
		UiLinearColor colors[4];
		Vec4 uvRect = Vec4(0, 0, 1, 1);
		Vec2 gradientStart = Vec2(0);
		Vec2 gradientEnd = Vec2(1, 0);
		UInt resourceIndex = invalidUiResourceIndex;
		Float distanceRange = 0;
	};

	struct UiMeshVertex
	{
		Vec2 position = Vec2(0);
		Vec2 uv = Vec2(0);
		UiLinearColor color;
	};

	struct UiInstanceRange
	{
		UInt first = 0;
		UInt count = 0;
	};

	struct UiMeshRange
	{
		UInt firstVertex = 0;
		UInt vertexCount = 0;
		UInt firstIndex = 0;
		UInt indexCount = 0;
	};

	struct UiDrawCommand
	{
		UiPipeline pipeline = UiPipeline::SolidQuad;
		UiBlendMode blend = UiBlendMode::PremultipliedAlpha;
		UiTexturePool texturePool = UiTexturePool::None;
		UInt resourceIndex = invalidUiResourceIndex;
		UiInstanceRange instances;
		UiMeshRange mesh;
		U64 paintOrder = 0;
	};

	struct UiClipRect
	{
		Vec2 position = Vec2(0);
		Vec2 size = Vec2(0);
	};

	struct UiClipRoundedRect
	{
		UiClipRect rect;
		Vec4 cornerRadii = Vec4(0);
	};

	struct UiClipTransformed
	{
		UiClipRoundedRect shape;
		glm::mat3 transform = glm::mat3(1);
	};

	using UiClipShape = std::variant<UiClipRect, UiClipRoundedRect, UiClipTransformed>;

	struct UiPushClipCommand { UiClipShape shape; };
	struct UiPopClipCommand {};
	struct UiPushTransformCommand { glm::mat3 transform = glm::mat3(1); };
	struct UiPopTransformCommand {};

	struct UiBeginLayerCommand
	{
		Float opacity = 1;
		UiBlendMode blend = UiBlendMode::PremultipliedAlpha;
		Bool isolated = false;
	};

	struct UiEndLayerCommand {};

	using UiDisplayCommand = std::variant<
		UiDrawCommand,
		UiPushClipCommand,
		UiPopClipCommand,
		UiPushTransformCommand,
		UiPopTransformCommand,
		UiBeginLayerCommand,
		UiEndLayerCommand>;

	struct UiWorldSurfaceMetadata
	{
		Matrix localToWorld = Matrix(1);
		Vec2 logicalSize = Vec2(0);
		Float pixelsPerWorldUnit = 1;
		Bool depthTest = true;
		Bool doubleSided = false;
		U64 surfaceId = 0;
	};

	enum class UiRenderDiagnosticCode : Byte
	{
		AlreadyFinalized,
		BudgetExceeded,
		UnbalancedClipStack,
		UnbalancedTransformStack,
		UnbalancedLayerStack,
		InvalidInstanceRange,
		InvalidMeshRange,
		InvalidMeshIndex,
		InvalidResource,
		InvalidCommand,
	};

	struct UiRenderDiagnostic
	{
		UiRenderDiagnosticCode code = UiRenderDiagnosticCode::InvalidCommand;
		SizeT commandIndex = 0;
		String message;
	};

	class UiDisplayList
	{
	public:
		explicit UiDisplayList(UiRenderBudgets budgets = {});

		[[nodiscard]] Bool addResource(UiResourceRef resource, UInt& resourceIndex);
		[[nodiscard]] Bool addSolidQuad(const UiQuadGeometry& geometry, UiLinearColor color,
			UiBlendMode blend = UiBlendMode::PremultipliedAlpha);
		// Border strips drawn inside geometry bounds — does not expand layout or paint area.
		[[nodiscard]] Bool addInsetBorder(const UiQuadGeometry& geometry, Float width,
			UiLinearColor color, UiBlendMode blend = UiBlendMode::PremultipliedAlpha);
		[[nodiscard]] Bool addGradientQuad(const UiQuadGeometry& geometry, const UiGradient& gradient,
			UiBlendMode blend = UiBlendMode::PremultipliedAlpha);
		[[nodiscard]] Bool addImageQuad(const UiQuadGeometry& geometry, const UiImagePaint& paint,
			UiBlendMode blend = UiBlendMode::PremultipliedAlpha);
		[[nodiscard]] Bool addGlyph(UiPipeline glyphPipeline, const UiQuadGeometry& geometry,
			const UiGlyphPaint& paint, UiBlendMode blend = UiBlendMode::PremultipliedAlpha);
		[[nodiscard]] Bool addGrayscaleGlyph(const UiQuadGeometry& geometry,
			const UiGlyphPaint& paint, UiBlendMode blend = UiBlendMode::PremultipliedAlpha);
		[[nodiscard]] Bool addMsdfGlyph(const UiQuadGeometry& geometry,
			const UiGlyphPaint& paint, UiBlendMode blend = UiBlendMode::PremultipliedAlpha);
		[[nodiscard]] Bool addColorGlyph(const UiQuadGeometry& geometry,
			const UiGlyphPaint& paint, UiBlendMode blend = UiBlendMode::PremultipliedAlpha);
		[[nodiscard]] Bool addPathMesh(std::span<const UiMeshVertex> vertices,
			std::span<const UInt> indices, UiBlendMode blend = UiBlendMode::PremultipliedAlpha);
		[[nodiscard]] Bool pushClip(UiClipShape shape);
		[[nodiscard]] Bool popClip();
		[[nodiscard]] Bool pushTransform(glm::mat3 transform);
		[[nodiscard]] Bool popTransform();
		[[nodiscard]] Bool beginLayer(UiBeginLayerCommand layer);
		[[nodiscard]] Bool endLayer();
		[[nodiscard]] Bool setWorldSurface(UiWorldSurfaceMetadata metadata);

		void clear();
		[[nodiscard]] Bool isFinalized() const { return finalized; }
		[[nodiscard]] const UiRenderBudgets& budgets() const { return limits; }
		[[nodiscard]] const Vector<UiDisplayCommand>& commands() const { return commandData; }
		[[nodiscard]] const Vector<UiQuadInstance>& instances() const { return instanceData; }
		[[nodiscard]] const Vector<UiMeshVertex>& meshVertices() const { return vertexData; }
		[[nodiscard]] const Vector<UInt>& meshIndices() const { return indexData; }
		[[nodiscard]] const Vector<UiResourceRef>& resources() const { return resourceData; }
		[[nodiscard]] const std::optional<UiWorldSurfaceMetadata>& worldSurface() const { return surface; }

	private:
		friend class UiDisplayListFinalizer;
		friend class UiDirtyPaintBuilder;

		[[nodiscard]] Bool appendCommand(UiDisplayCommand command);
		[[nodiscard]] Bool appendInstanceDraw(UiPipeline pipeline, UiTexturePool pool,
			UInt resourceIndex, UiQuadInstance instance, UiBlendMode blend);
		[[nodiscard]] Bool append(const UiDisplayList& other);

		UiRenderBudgets limits;
		Vector<UiDisplayCommand> commandData;
		Vector<UiQuadInstance> instanceData;
		Vector<UiMeshVertex> vertexData;
		Vector<UInt> indexData;
		Vector<UiResourceRef> resourceData;
		std::optional<UiWorldSurfaceMetadata> surface;
		U64 nextPaintOrder = 0;
		Bool finalized = false;
	};

	struct UiEffectiveClip
	{
		UiClipShape shape;
		UInt parentClipState = 0;
		Bool axisAlignedIntersection = false;
	};

	struct UiRenderBatch
	{
		UiPipeline pipeline = UiPipeline::SolidQuad;
		UiBlendMode blend = UiBlendMode::PremultipliedAlpha;
		UiTexturePool texturePool = UiTexturePool::None;
		UInt resourceIndex = invalidUiResourceIndex;
		UInt clipState = 0;
		UInt layerState = 0;
		UiInstanceRange instances;
		UiMeshRange mesh;
		U64 firstPaintOrder = 0;
		UInt firstCommand = 0;
		UInt drawCommandCount = 0;
	};

	class UiRenderPacket
	{
	public:
		[[nodiscard]] const Vector<UiDisplayCommand>& commands() const { return commandData; }
		[[nodiscard]] const Vector<UiRenderBatch>& batches() const { return batchData; }
		[[nodiscard]] const Vector<UiEffectiveClip>& clips() const { return clipData; }
		[[nodiscard]] const Vector<UiQuadInstance>& instances() const { return instanceData; }
		[[nodiscard]] const Vector<UiMeshVertex>& meshVertices() const { return vertexData; }
		[[nodiscard]] const Vector<UInt>& meshIndices() const { return indexData; }
		[[nodiscard]] const Vector<UiResourceRef>& resources() const { return resourceData; }
		[[nodiscard]] const std::optional<UiWorldSurfaceMetadata>& worldSurface() const { return surface; }

	private:
		friend class UiDisplayListFinalizer;
		Vector<UiDisplayCommand> commandData;
		Vector<UiRenderBatch> batchData;
		Vector<UiEffectiveClip> clipData;
		Vector<UiQuadInstance> instanceData;
		Vector<UiMeshVertex> vertexData;
		Vector<UInt> indexData;
		Vector<UiResourceRef> resourceData;
		std::optional<UiWorldSurfaceMetadata> surface;
	};

	class UiDisplayListFinalizer
	{
	public:
		[[nodiscard]] static Bool finalize(UiDisplayList& list, UiRenderPacket& packet,
			Vector<UiRenderDiagnostic>& diagnostics);
	};
}
