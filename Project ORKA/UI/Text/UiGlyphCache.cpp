#include "UiGlyphCache.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstring>
#include <limits>

#ifdef ORKA_UI_ENABLE_ADVANCED_TEXT
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_COLOR_H
#include FT_MULTIPLE_MASTERS_H
#include FT_OUTLINE_H
#include <msdfgen/msdfgen.h>
#include <msdfgen/msdfgen-ext.h>
#endif

namespace Ui
{
	namespace
	{
		template <typename T>
		void hashCombine(SizeT& seed, const T& value)
		{
			seed ^= std::hash<T>{}(value) + static_cast<SizeT>(0x9e3779b9) +
				(seed << 6) + (seed >> 2);
		}

		UInt defaultPadding(const UiGlyphCacheConfig& config,
			const UiGlyphRenderMode mode)
		{
			if (mode == UiGlyphRenderMode::Msdf || mode == UiGlyphRenderMode::Mtsdf)
				return config.distanceFieldPadding;
			if (mode == UiGlyphRenderMode::ColorRgba) return config.colorPadding;
			return config.grayscalePadding;
		}

		Bool makeTofu(const UiGlyphRasterRequest& request,
			UiGlyphRasterResult& result)
		{
			if (request.key.glyphId != 0) return false;
			const UInt pixelSize = std::max(1u, request.key.pixelSize26_6 / 64);
			result.key = request.key;
			result.format = uiGlyphPixelFormat(request.key.mode);
			result.width = std::max(4u, pixelSize / 2);
			result.height = std::max(6u, (pixelSize * 3) / 4);
			result.advanceX26_6 = static_cast<Int>((result.width + 1) * 64);
			result.bearingY26_6 = static_cast<Int>(result.height * 64);
			const UInt channels = uiGlyphBytesPerPixel(result.format);
			result.pixels.assign(static_cast<SizeT>(result.width) *
				result.height * channels, 0);
			for (UInt y = 0; y < result.height; ++y)
				for (UInt x = 0; x < result.width; ++x)
				{
					const Bool edge = x == 0 || y == 0 ||
						x + 1 == result.width || y + 1 == result.height;
					Byte* pixel = result.pixels.data() +
						(static_cast<SizeT>(y) * result.width + x) * channels;
					for (UInt channel = 0; channel < channels; ++channel)
						pixel[channel] = edge ? 255 : 0;
					if (channels == 4) pixel[3] = edge ? 255 : 0;
				}
			result.succeeded = true;
			return true;
		}

#ifdef ORKA_UI_ENABLE_ADVANCED_TEXT
		constexpr UInt maximumRasterDimension = 4096;
		constexpr UInt maximumVariationAxes = 64;
		constexpr Float defaultDistanceFieldPixelRange = 4;

		Bool applyVariationAxes(FT_Library library, FT_Face face,
			const Vector<UiVariableAxis>& requested,
			UiGlyphRasterFailure& failure)
		{
			if (requested.empty()) return true;
			if (requested.size() > maximumVariationAxes ||
				!FT_HAS_MULTIPLE_MASTERS(face))
			{
				failure = UiGlyphRasterFailure::VariationUnsupported;
				return false;
			}
			FT_MM_Var* variables = nullptr;
			if (FT_Get_MM_Var(face, &variables) != 0 || !variables)
			{
				failure = UiGlyphRasterFailure::VariationUnsupported;
				return false;
			}
			Vector<FT_Fixed> coordinates(variables->num_axis);
			Vector<Bool> matched(requested.size(), false);
			for (FT_UInt index = 0; index < variables->num_axis; ++index)
			{
				coordinates[index] = variables->axis[index].def;
				for (SizeT requestedIndex = 0;
					requestedIndex < requested.size(); ++requestedIndex)
				{
					const UiVariableAxis& axis = requested[requestedIndex];
					if (axis.tag.size() != 4) continue;
					const FT_ULong tag = FT_MAKE_TAG(
						axis.tag[0], axis.tag[1], axis.tag[2], axis.tag[3]);
					if (tag != variables->axis[index].tag) continue;
					const double fixed = static_cast<double>(axis.value) * 65536.0;
					if (!std::isfinite(fixed) ||
						fixed < variables->axis[index].minimum ||
						fixed > variables->axis[index].maximum)
						continue;
					coordinates[index] = static_cast<FT_Fixed>(std::llround(fixed));
					matched[requestedIndex] = true;
				}
			}
			const Bool allMatched =
				std::find(matched.begin(), matched.end(), false) == matched.end();
			const FT_Error applied = allMatched ? FT_Set_Var_Design_Coordinates(
				face, variables->num_axis, coordinates.data()) : 1;
			FT_Done_MM_Var(library, variables);
			if (!allMatched)
			{
				failure = UiGlyphRasterFailure::VariationAxisUnknown;
				return false;
			}
			if (applied != 0)
			{
				failure = UiGlyphRasterFailure::VariationApplicationFailed;
				return false;
			}
			return true;
		}

		template <int Channels>
		void convertDistanceBitmap(const msdfgen::Bitmap<float, Channels>& bitmap,
			Vector<Byte>& pixels)
		{
			pixels.resize(static_cast<SizeT>(bitmap.width()) *
				bitmap.height() * Channels);
			for (Int y = 0; y < bitmap.height(); ++y)
				for (Int x = 0; x < bitmap.width(); ++x)
				{
					const float* source = bitmap(x, y);
					Byte* destination = pixels.data() +
						(static_cast<SizeT>(y) * bitmap.width() + x) * Channels;
					for (Int channel = 0; channel < Channels; ++channel)
						destination[channel] =
							static_cast<Byte>(msdfgen::pixelFloatToByte(source[channel]));
				}
		}

		Bool rasterDistanceField(FT_Face face,
			const UiGlyphRasterRequest& request, UiGlyphRasterResult& result)
		{
			const UInt pixelSize = std::max(1u,
				request.key.pixelSize26_6 / 64 +
				(request.key.pixelSize26_6 % 64 != 0));
			if (pixelSize > maximumRasterDimension || !face->units_per_EM)
			{
				result.failure = UiGlyphRasterFailure::DeterministicLimitExceeded;
				return false;
			}
			msdfgen::FontHandle* font = msdfgen::adoptFreetypeFont(face);
			if (!font)
			{
				result.failure = UiGlyphRasterFailure::BackendUnavailable;
				return false;
			}
			msdfgen::Shape shape;
			double advance = 0;
			Bool loaded = false;
			if (request.key.outline26_6 <= 0)
				loaded = msdfgen::loadGlyph(shape, font,
					msdfgen::GlyphIndex(request.key.glyphId),
					msdfgen::FONT_SCALING_EM_NORMALIZED, &advance);
			else if (FT_Load_Glyph(face, request.key.glyphId,
				FT_LOAD_NO_SCALE | FT_LOAD_NO_HINTING | FT_LOAD_NO_BITMAP) == 0 &&
				face->glyph->format == FT_GLYPH_FORMAT_OUTLINE)
			{
				const double strength = static_cast<double>(request.key.outline26_6) /
					request.key.pixelSize26_6 * face->units_per_EM;
				if (std::isfinite(strength) &&
					FT_Outline_Embolden(&face->glyph->outline,
						static_cast<FT_Pos>(std::llround(strength))) == 0)
				{
					loaded = msdfgen::readFreetypeOutline(shape,
						&face->glyph->outline, 1.0 / face->units_per_EM) == 0;
					advance = static_cast<double>(face->glyph->advance.x) /
						face->units_per_EM;
				}
			}
			msdfgen::destroyFont(font);
			if (!loaded || !shape.validate() || shape.edgeCount() <= 0)
			{
				result.failure = UiGlyphRasterFailure::GlyphLoadFailed;
				return false;
			}
			shape.normalize();
			msdfgen::edgeColoringSimple(shape, 3.0, request.key.glyphId);
			const msdfgen::Shape::Bounds bounds = shape.getBounds();
			const double scale = pixelSize;
			const double pixelRange = request.padding ?
				request.padding : defaultDistanceFieldPixelRange;
			const double left = std::floor(bounds.l * scale - pixelRange);
			const double bottom = std::floor(bounds.b * scale - pixelRange);
			const double right = std::ceil(bounds.r * scale + pixelRange);
			const double top = std::ceil(bounds.t * scale + pixelRange);
			const double widthValue = right - left;
			const double heightValue = top - bottom;
			if (!std::isfinite(widthValue) || !std::isfinite(heightValue) ||
				widthValue < 1 || heightValue < 1 ||
				widthValue > maximumRasterDimension ||
				heightValue > maximumRasterDimension)
			{
				result.failure = UiGlyphRasterFailure::DeterministicLimitExceeded;
				return false;
			}
			const UInt width = static_cast<UInt>(widthValue);
			const UInt height = static_cast<UInt>(heightValue);
			const msdfgen::Vector2 translation(-left / scale, -bottom / scale);
			const msdfgen::SDFTransformation transformation(
				msdfgen::Projection(msdfgen::Vector2(scale, scale), translation),
				msdfgen::Range(pixelRange / scale));
			try
			{
				if (request.key.mode == UiGlyphRenderMode::Mtsdf)
				{
					msdfgen::Bitmap<float, 4> bitmap(width, height);
					msdfgen::generateMTSDF(bitmap, shape, transformation);
					convertDistanceBitmap(bitmap, result.pixels);
				}
				else
				{
					msdfgen::Bitmap<float, 3> bitmap(width, height);
					msdfgen::generateMSDF(bitmap, shape, transformation);
					convertDistanceBitmap(bitmap, result.pixels);
				}
			}
			catch (...)
			{
				result.pixels.clear();
				result.failure = UiGlyphRasterFailure::RasterizationFailed;
				return false;
			}
			result.width = width;
			result.height = height;
			result.bearingX26_6 = static_cast<Int>(left * 64);
			result.bearingY26_6 = static_cast<Int>(top * 64);
			result.advanceX26_6 = static_cast<Int>(std::llround(advance * scale * 64));
			result.distanceField = uiGlyphDistanceFieldMetadata(
				request.key.mode, static_cast<Float>(pixelRange));
			result.distanceField.emScale = static_cast<Float>(scale);
			result.distanceField.translation = Vec2(
				static_cast<Float>(translation.x),
				static_cast<Float>(translation.y));
			result.succeeded = true;
			return true;
		}
#endif
	}

	UiGlyphRenderMode selectUiGlyphRenderMode(
		const UiGlyphRenderContext& context)
	{
		if (context.colorGlyph) return UiGlyphRenderMode::ColorRgba;
		if (context.screenSpace && context.pixelAligned && !context.transformed &&
			!context.worldSpace && context.pixelSize <= context.grayscaleMaximumSize)
			return UiGlyphRenderMode::HintedGrayscale;
		return context.worldSpace || context.transformed ?
			UiGlyphRenderMode::Mtsdf : UiGlyphRenderMode::Msdf;
	}

	UiGlyphPixelFormat uiGlyphPixelFormat(const UiGlyphRenderMode mode)
	{
		if (mode == UiGlyphRenderMode::Msdf) return UiGlyphPixelFormat::RGB8;
		if (mode == UiGlyphRenderMode::Mtsdf)
			return UiGlyphPixelFormat::MtsdfRGBA8;
		if (mode == UiGlyphRenderMode::ColorRgba)
			return UiGlyphPixelFormat::RGBA8;
		return UiGlyphPixelFormat::R8;
	}

	UInt uiGlyphBytesPerPixel(const UiGlyphPixelFormat format)
	{
		if (format == UiGlyphPixelFormat::RGB8) return 3;
		if (format == UiGlyphPixelFormat::RGBA8 ||
			format == UiGlyphPixelFormat::MtsdfRGBA8) return 4;
		return 1;
	}

	UiGlyphDistanceFieldMetadata uiGlyphDistanceFieldMetadata(
		const UiGlyphRenderMode mode, const Float pixelRange)
	{
		UiGlyphDistanceFieldMetadata metadata;
		metadata.enabled = mode == UiGlyphRenderMode::Msdf ||
			mode == UiGlyphRenderMode::Mtsdf;
		metadata.trueDistanceAlpha = mode == UiGlyphRenderMode::Mtsdf;
		metadata.pixelRange = metadata.enabled ? std::max(0.0f, pixelRange) : 0;
		return metadata;
	}

	SizeT UiGlyphKeyHash::operator()(const UiGlyphKey& key) const noexcept
	{
		SizeT result = 0;
		hashCombine(result, key.faceId);
		hashCombine(result, key.glyphId);
		hashCombine(result, key.pixelSize26_6);
		hashCombine(result, static_cast<Byte>(key.mode));
		hashCombine(result, key.variation);
		hashCombine(result, key.outline26_6);
		return result;
	}

	UiGlyphSkylineAllocator::UiGlyphSkylineAllocator(
		const UInt width, const UInt height)
	{
		reset(width, height);
	}

	void UiGlyphSkylineAllocator::reset(const UInt width, const UInt height)
	{
		atlasWidth = width;
		atlasHeight = height;
		skyline.clear();
		if (width && height) skyline.push_back({ 0, 0, width });
	}

	Bool UiGlyphSkylineAllocator::allocate(
		const UInt width, const UInt height, UiGlyphRect& output)
	{
		if (!width || !height || width > atlasWidth || height > atlasHeight)
			return false;
		UInt bestIndex = maxUInt;
		UInt bestX = 0;
		UInt bestY = maxUInt;
		for (UInt index = 0; index < skyline.size(); ++index)
		{
			const UInt x = skyline[index].x;
			if (x + width > atlasWidth) continue;
			UInt y = skyline[index].y;
			UInt remaining = width;
			for (UInt scan = index; remaining && scan < skyline.size(); ++scan)
			{
				y = std::max(y, skyline[scan].y);
				if (y + height > atlasHeight) { remaining = width; break; }
				remaining = remaining > skyline[scan].width ?
					remaining - skyline[scan].width : 0;
			}
			if (remaining) continue;
			if (y < bestY || (y == bestY && x < bestX))
			{
				bestIndex = index;
				bestX = x;
				bestY = y;
			}
		}
		if (bestIndex == maxUInt) return false;

		skyline.insert(skyline.begin() + bestIndex,
			{ bestX, bestY + height, width });
		for (UInt index = bestIndex + 1; index < skyline.size();)
		{
			Node& previous = skyline[index - 1];
			Node& node = skyline[index];
			const UInt previousEnd = previous.x + previous.width;
			if (node.x >= previousEnd) break;
			const UInt overlap = previousEnd - node.x;
			if (overlap >= node.width) skyline.erase(skyline.begin() + index);
			else
			{
				node.x += overlap;
				node.width -= overlap;
				break;
			}
		}
		for (UInt index = 0; index + 1 < skyline.size();)
		{
			if (skyline[index].y == skyline[index + 1].y)
			{
				skyline[index].width += skyline[index + 1].width;
				skyline.erase(skyline.begin() + index + 1);
			}
			else ++index;
		}
		output = { bestX, bestY, width, height };
		return true;
	}

	struct UiGlyphCache::Impl
	{
		enum class State : Byte { Pending, Resident, Failed };

		struct Entry
		{
			State state = State::Pending;
			UiGlyphAtlasHandle handle;
			UiGlyphRasterResult metrics;
			U64 lastUse = 0;
		};

		struct Page
		{
			UiGlyphPixelFormat format = UiGlyphPixelFormat::R8;
			UInt index = 0;
			UInt generation = 1;
			U64 lastUse = 0;
			UiGlyphSkylineAllocator allocator;
		};

		UiGlyphCacheConfig config;
		mutable Mutex mutex;
		std::condition_variable requestReady;
		std::condition_variable resultSpace;
		std::deque<UiGlyphRasterRequest> requests;
		std::deque<UiGlyphRasterResult> results;
		std::unordered_map<UiGlyphKey, Entry, UiGlyphKeyHash> entries;
		std::array<Vector<Page>, 4> pages;
		Vector<Thread> workers;
		Bool stopping = false;
		U64 clock = 0;
		U64 evictions = 0;
		std::thread::id renderThread;

		explicit Impl(UiGlyphCacheConfig supplied) : config(std::move(supplied))
		{
			if (!config.missingGlyph.pixelSize26_6)
				config.missingGlyph.pixelSize26_6 = 16 * 64;
		}

		UInt maximumPages(const UiGlyphPixelFormat format) const
		{
			if (format == UiGlyphPixelFormat::RGB8 ||
				format == UiGlyphPixelFormat::MtsdfRGBA8)
				return config.maximumRGB8Pages;
			if (format == UiGlyphPixelFormat::RGBA8) return config.maximumRGBA8Pages;
			return config.maximumR8Pages;
		}

		static UInt formatIndex(const UiGlyphPixelFormat format)
		{
			return static_cast<UInt>(format);
		}

		Bool bindRenderThread()
		{
			const std::thread::id current = std::this_thread::get_id();
			if (renderThread == std::thread::id{}) renderThread = current;
			assert(renderThread == current && "glyph uploads are render-thread-only");
			return renderThread == current;
		}

		Bool runRasterizer(const UiGlyphRasterRequest& request,
			UiGlyphRasterResult& result) const
		{
			result = {};
			result.key = request.key;
			result.padding = request.padding ?
				request.padding : defaultPadding(config, request.key.mode);
			const Bool success = config.rasterizer ?
				config.rasterizer(request, result) : rasterUiGlyph(request, result);
			result.key = request.key;
			result.padding = request.padding ?
				request.padding : (result.padding ? result.padding :
					defaultPadding(config, request.key.mode));
			result.succeeded = success && result.succeeded &&
				result.width > 0 && result.height > 0 &&
				result.pixels.size() == static_cast<SizeT>(result.width) *
					result.height * uiGlyphBytesPerPixel(result.format);
			return result.succeeded;
		}

		void workerMain()
		{
			for (;;)
			{
				UiGlyphRasterRequest request;
				{
					std::unique_lock lock(mutex);
					requestReady.wait(lock, [&] { return stopping || !requests.empty(); });
					if (stopping && requests.empty()) return;
					request = std::move(requests.front());
					requests.pop_front();
				}
				UiGlyphRasterResult result;
				runRasterizer(request, result);
				{
					std::unique_lock lock(mutex);
					resultSpace.wait(lock, [&] {
						return stopping || results.size() < config.resultQueueCapacity;
					});
					if (stopping) return;
					results.push_back(std::move(result));
				}
			}
		}

		void erasePageEntries(const Page& page)
		{
			for (auto iterator = entries.begin(); iterator != entries.end();)
			{
				const UiGlyphAtlasHandle& handle = iterator->second.handle;
				if (iterator->second.state == State::Resident &&
					handle.format == page.format && handle.page == page.index &&
					handle.pageGeneration == page.generation)
					iterator = entries.erase(iterator);
				else ++iterator;
			}
		}

		Page* allocatePageRect(const UiGlyphPixelFormat format,
			const UInt width, const UInt height, UiGlyphRect& allocation)
		{
			Vector<Page>& pool = pages[formatIndex(format)];
			for (Page& page : pool)
				if (page.allocator.allocate(width, height, allocation))
					return &page;
			if (pool.size() < maximumPages(format))
			{
				Page page;
				page.format = format;
				page.index = static_cast<UInt>(pool.size());
				page.allocator.reset(config.pageWidth, config.pageHeight);
				pool.push_back(std::move(page));
				if (pool.back().allocator.allocate(width, height, allocation))
					return &pool.back();
				return nullptr;
			}
			if (pool.empty()) return nullptr;
			Page* victim = &pool.front();
			for (Page& page : pool)
				if (page.lastUse < victim->lastUse ||
					(page.lastUse == victim->lastUse && page.index < victim->index))
					victim = &page;
			erasePageEntries(*victim);
			++victim->generation;
			if (!victim->generation) ++victim->generation;
			victim->allocator.reset(config.pageWidth, config.pageHeight);
			victim->lastUse = ++clock;
			++evictions;
			return victim->allocator.allocate(width, height, allocation) ?
				victim : nullptr;
		}

		Bool uploadResult(UiGlyphRasterResult result,
			const UiGlyphUploadCallback& upload, SizeT& uploadedBytes)
		{
			auto found = entries.find(result.key);
			if (found == entries.end()) return false;
			if (!result.succeeded)
			{
				found->second.state = State::Failed;
				return false;
			}
			if (result.padding > config.pageWidth ||
				result.padding > config.pageHeight ||
				result.width > config.pageWidth - result.padding ||
				result.width + result.padding > config.pageWidth - result.padding ||
				result.height > config.pageHeight - result.padding ||
				result.height + result.padding > config.pageHeight - result.padding)
			{
				found->second.state = State::Failed;
				return false;
			}
			const UInt channels = uiGlyphBytesPerPixel(result.format);
			const UInt paddedWidth = result.width + result.padding * 2;
			const UInt paddedHeight = result.height + result.padding * 2;
			UiGlyphRect allocated;
			Page* page = allocatePageRect(
				result.format, paddedWidth, paddedHeight, allocated);
			if (!page)
			{
				found->second.state = State::Failed;
				return false;
			}
			Vector<Byte> padded(static_cast<SizeT>(paddedWidth) *
				paddedHeight * channels, 0);
			for (UInt row = 0; row < result.height; ++row)
				std::memcpy(padded.data() +
					(static_cast<SizeT>(row + result.padding) * paddedWidth +
						result.padding) * channels,
					result.pixels.data() +
					static_cast<SizeT>(row) * result.width * channels,
					static_cast<SizeT>(result.width) * channels);
			upload(result.format, page->index, page->generation,
				allocated, padded.data(), padded.size());
			uploadedBytes += padded.size();
			page->lastUse = ++clock;
			Entry& entry = found->second;
			entry.state = State::Resident;
			entry.metrics = std::move(result);
			entry.handle = {
				page->format, page->index, page->generation,
				{ allocated.x + entry.metrics.padding,
					allocated.y + entry.metrics.padding,
					entry.metrics.width, entry.metrics.height }
			};
			entry.lastUse = page->lastUse;
			return true;
		}
	};

	UiGlyphCache::UiGlyphCache(UiGlyphCacheConfig config)
		: impl(std::make_unique<Impl>(std::move(config)))
	{
		startWorkers();
		UiGlyphRasterRequest missing;
		missing.key = impl->config.missingGlyph;
		(void)request(missing);
	}

	UiGlyphCache::~UiGlyphCache()
	{
		stopWorkers();
	}

	void UiGlyphCache::startWorkers()
	{
		LockGuard lock(impl->mutex);
		if (!impl->workers.empty() || !impl->config.workerCount) return;
		impl->stopping = false;
		for (UInt index = 0; index < impl->config.workerCount; ++index)
			impl->workers.emplace_back([this] { impl->workerMain(); });
	}

	void UiGlyphCache::stopWorkers()
	{
		{
			LockGuard lock(impl->mutex);
			if (impl->workers.empty()) return;
			impl->stopping = true;
		}
		impl->requestReady.notify_all();
		impl->resultSpace.notify_all();
		for (Thread& worker : impl->workers)
			if (worker.joinable()) worker.join();
		LockGuard lock(impl->mutex);
		impl->workers.clear();
		impl->requests.clear();
		for (auto iterator = impl->entries.begin(); iterator != impl->entries.end();)
			if (iterator->second.state == Impl::State::Pending)
				iterator = impl->entries.erase(iterator);
			else ++iterator;
	}

	Bool UiGlyphCache::workersRunning() const
	{
		LockGuard lock(impl->mutex);
		return !impl->workers.empty() && !impl->stopping;
	}

	Bool UiGlyphCache::request(const UiGlyphRasterRequest& request)
	{
		{
			LockGuard lock(impl->mutex);
			if (impl->entries.contains(request.key)) return false;
			if (impl->stopping || impl->workers.empty() ||
				!impl->config.resultQueueCapacity ||
				impl->requests.size() >= impl->config.requestQueueCapacity)
				return false;
			impl->entries.emplace(request.key, Impl::Entry{});
			impl->requests.push_back(request);
		}
		impl->requestReady.notify_one();
		return true;
	}

	Bool UiGlyphCache::lookup(const UiGlyphKey& key,
		UiGlyphAtlasHandle& handle, UiGlyphRasterResult* metrics)
	{
		LockGuard lock(impl->mutex);
		const auto found = impl->entries.find(key);
		if (found == impl->entries.end() ||
			found->second.state != Impl::State::Resident)
			return false;
		Impl::Entry& entry = found->second;
		const Vector<Impl::Page>& pool =
			impl->pages[Impl::formatIndex(entry.handle.format)];
		if (entry.handle.page >= pool.size() ||
			pool[entry.handle.page].generation != entry.handle.pageGeneration)
		{
			impl->entries.erase(found);
			return false;
		}
		entry.lastUse = ++impl->clock;
		impl->pages[Impl::formatIndex(entry.handle.format)]
			[entry.handle.page].lastUse = entry.lastUse;
		handle = entry.handle;
		if (metrics) *metrics = entry.metrics;
		return true;
	}

	UiGlyphAtlasHandle UiGlyphCache::lookupOrMissing(const UiGlyphKey& key)
	{
		UiGlyphAtlasHandle handle;
		if (lookup(key, handle)) return handle;
		lookup(impl->config.missingGlyph, handle);
		return handle;
	}

	UInt UiGlyphCache::drainUploads(const UiGlyphUploadCallback& upload,
		const UiGlyphUploadBudget& budget)
	{
		if (!upload || !impl->bindRenderThread()) return 0;
		UInt count = 0;
		SizeT bytes = 0;
		for (;;)
		{
			UiGlyphRasterResult result;
			{
				LockGuard lock(impl->mutex);
				if (impl->results.empty() || count >= budget.maximumGlyphs)
					break;
				const UiGlyphRasterResult& next = impl->results.front();
				const SizeT nextBytes =
					static_cast<SizeT>(next.width + next.padding * 2) *
					(next.height + next.padding * 2) *
					uiGlyphBytesPerPixel(next.format);
				if (next.succeeded && nextBytes > budget.maximumBytes - bytes)
					break;
				result = std::move(impl->results.front());
				impl->results.pop_front();
				impl->uploadResult(std::move(result), upload, bytes);
			}
			impl->resultSpace.notify_one();
			++count;
		}
		return count;
	}

	UInt UiGlyphCache::warmUp(
		const Vector<UiGlyphRasterRequest>& requests,
		const UiGlyphUploadCallback& upload)
	{
		if (!upload || !impl->bindRenderThread()) return 0;
		UInt count = 0;
		for (const UiGlyphRasterRequest& request : requests)
		{
			{
				LockGuard lock(impl->mutex);
				if (impl->entries.contains(request.key)) continue;
				impl->entries.emplace(request.key, Impl::Entry{});
			}
			UiGlyphRasterResult result;
			impl->runRasterizer(request, result);
			SizeT ignoredBytes = 0;
			LockGuard lock(impl->mutex);
			if (impl->uploadResult(std::move(result), upload, ignoredBytes)) ++count;
		}
		return count;
	}

	void UiGlyphCache::clear()
	{
		LockGuard lock(impl->mutex);
		impl->entries.clear();
		impl->requests.clear();
		impl->results.clear();
		for (Vector<Impl::Page>& pool : impl->pages)
			for (Impl::Page& page : pool)
			{
				++page.generation;
				if (!page.generation) ++page.generation;
				page.allocator.reset(impl->config.pageWidth, impl->config.pageHeight);
				page.lastUse = 0;
			}
		impl->resultSpace.notify_all();
	}

	UiGlyphCacheStats UiGlyphCache::stats() const
	{
		LockGuard lock(impl->mutex);
		UiGlyphCacheStats result;
		for (const auto& [key, entry] : impl->entries)
			if (entry.state == Impl::State::Resident) ++result.residentGlyphs;
			else if (entry.state == Impl::State::Pending) ++result.pendingGlyphs;
		result.queuedRequests = impl->requests.size();
		result.queuedResults = impl->results.size();
		result.pageEvictions = impl->evictions;
		return result;
	}

	Bool UiGlyphCache::isHandleCurrent(const UiGlyphAtlasHandle& handle) const
	{
		if (!handle.isValid()) return false;
		LockGuard lock(impl->mutex);
		const Vector<Impl::Page>& pool =
			impl->pages[Impl::formatIndex(handle.format)];
		return handle.page < pool.size() &&
			pool[handle.page].generation == handle.pageGeneration;
	}

	Bool rasterUiGlyph(const UiGlyphRasterRequest& request,
		UiGlyphRasterResult& result)
	{
		result = {};
		result.key = request.key;
		result.format = uiGlyphPixelFormat(request.key.mode);
		if (makeTofu(request, result)) return true;
#ifdef ORKA_UI_ENABLE_ADVANCED_TEXT
		if (request.fontPath.empty() || request.faceIndex < 0 ||
			request.key.mode == UiGlyphRenderMode::Automatic)
		{
			result.failure = UiGlyphRasterFailure::InvalidRequest;
			return false;
		}
		FT_Library library = nullptr;
		FT_Face face = nullptr;
		if (FT_Init_FreeType(&library) != 0)
		{
			result.failure = UiGlyphRasterFailure::BackendUnavailable;
			return false;
		}
		const String path = request.fontPath.string();
		if (FT_New_Face(library, path.c_str(), request.faceIndex, &face) != 0)
		{
			result.failure = UiGlyphRasterFailure::FontOpenFailed;
			FT_Done_FreeType(library);
			return false;
		}
		if (!applyVariationAxes(library, face, request.axes, result.failure))
		{
			FT_Done_Face(face);
			FT_Done_FreeType(library);
			return false;
		}
		if (request.key.mode == UiGlyphRenderMode::Msdf ||
			request.key.mode == UiGlyphRenderMode::Mtsdf)
		{
			const Bool succeeded = rasterDistanceField(face, request, result);
			FT_Done_Face(face);
			FT_Done_FreeType(library);
			return succeeded;
		}
		const UInt pixelSize = std::max(1u, request.key.pixelSize26_6 / 64);
		if (pixelSize > maximumRasterDimension ||
			FT_Set_Pixel_Sizes(face, 0, pixelSize) != 0)
		{
			result.failure = UiGlyphRasterFailure::DeterministicLimitExceeded;
			FT_Done_Face(face);
			FT_Done_FreeType(library);
			return false;
		}
		FT_Int32 flags = request.key.mode == UiGlyphRenderMode::ColorRgba ?
			FT_LOAD_COLOR : FT_LOAD_DEFAULT;
		if (request.key.mode == UiGlyphRenderMode::HintedGrayscale)
			flags |= FT_LOAD_TARGET_NORMAL;
		const Bool loaded = FT_Load_Glyph(face, request.key.glyphId, flags) == 0;
		if (loaded && request.key.outline26_6 > 0 &&
			face->glyph->format == FT_GLYPH_FORMAT_OUTLINE)
			FT_Outline_Embolden(&face->glyph->outline, request.key.outline26_6);
		const Bool rendered = loaded &&
			(face->glyph->format == FT_GLYPH_FORMAT_BITMAP ||
				FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL) == 0);
		if (rendered)
		{
			const FT_Bitmap& bitmap = face->glyph->bitmap;
			if (bitmap.width > maximumRasterDimension ||
				bitmap.rows > maximumRasterDimension)
			{
				result.failure = UiGlyphRasterFailure::DeterministicLimitExceeded;
				FT_Done_Face(face);
				FT_Done_FreeType(library);
				return false;
			}
			const Bool color = bitmap.pixel_mode == FT_PIXEL_MODE_BGRA;
			result.format = color ? UiGlyphPixelFormat::RGBA8 :
				UiGlyphPixelFormat::R8;
			result.width = bitmap.width;
			result.height = bitmap.rows;
			result.bearingX26_6 = face->glyph->bitmap_left * 64;
			result.bearingY26_6 = face->glyph->bitmap_top * 64;
			result.advanceX26_6 = static_cast<Int>(face->glyph->advance.x);
			const UInt channels = uiGlyphBytesPerPixel(result.format);
			result.pixels.assign(static_cast<SizeT>(result.width) *
				result.height * channels, 0);
			for (UInt y = 0; y < result.height; ++y)
			{
				const Int sourceY = bitmap.pitch < 0 ?
					static_cast<Int>(result.height - y - 1) : static_cast<Int>(y);
				const Byte* source = bitmap.buffer +
					static_cast<SizeT>(sourceY) * std::abs(bitmap.pitch);
				Byte* destination = result.pixels.data() +
					static_cast<SizeT>(y) * result.width * channels;
				if (color)
					for (UInt x = 0; x < result.width; ++x)
					{
						destination[x * 4 + 0] = source[x * 4 + 2];
						destination[x * 4 + 1] = source[x * 4 + 1];
						destination[x * 4 + 2] = source[x * 4 + 0];
						destination[x * 4 + 3] = source[x * 4 + 3];
					}
				else if (bitmap.pixel_mode == FT_PIXEL_MODE_GRAY)
					std::memcpy(destination, source, result.width);
				else if (bitmap.pixel_mode == FT_PIXEL_MODE_MONO)
					for (UInt x = 0; x < result.width; ++x)
						destination[x] =
							(source[x / 8] & (0x80 >> (x % 8))) ? 255 : 0;
			}
			result.color.color = color;
			result.color.premultipliedAlpha = color;
			result.color.source = UiGlyphColorFormat::None;
			if (color)
			{
				FT_UInt layerGlyph = 0;
				FT_UInt colorIndex = 0;
				FT_LayerIterator iterator = {};
				result.color.source = FT_Get_Color_Glyph_Layer(face,
					request.key.glyphId, &layerGlyph, &colorIndex, &iterator) ?
					UiGlyphColorFormat::Layered : UiGlyphColorFormat::Bitmap;
			}
			result.succeeded = result.width && result.height;
		}
		if (!loaded) result.failure = UiGlyphRasterFailure::GlyphLoadFailed;
		else if (!rendered || !result.succeeded)
			result.failure = UiGlyphRasterFailure::RasterizationFailed;
		FT_Done_Face(face);
		FT_Done_FreeType(library);
		return result.succeeded;
#else
		result.failure = UiGlyphRasterFailure::BackendUnavailable;
		return false;
#endif
	}
}
