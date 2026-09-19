#include "UiBenchmarks.hpp"

#include <algorithm>
#include <chrono>
#include <exception>

namespace Ui::Testing
{
	namespace
	{
		using Clock = std::chrono::steady_clock;
		using Work = Function<SizeT(UInt, U64&)>;

		UiBenchmarkSample measure(const String& name,
			const UiBenchmarkConfig& config, const U64 operationsPerIteration,
			const Work& work)
		{
			UiBenchmarkSample sample;
			sample.name = name;
			const U64 totalIterations =
				static_cast<U64>(config.warmupIterations) + config.measuredIterations;
			if (!config.measuredIterations || operationsPerIteration > 0 &&
				totalIterations > config.maximumOperations / operationsPerIteration)
			{
				sample.status = UiBenchmarkStatus::BudgetExceeded;
				sample.note = "Configured deterministic operation budget is too small.";
				return sample;
			}
			Vector<U64> timings;
			const auto suiteStart = Clock::now();
			try
			{
				for (UInt iteration = 0; iteration < totalIterations; ++iteration)
				{
					U64 growthEvents = 0;
					const auto start = Clock::now();
					sample.outputCount = work(iteration, growthEvents);
					const U64 elapsed = static_cast<U64>(
						std::chrono::duration_cast<std::chrono::microseconds>(
							Clock::now() - start).count());
					sample.operations += operationsPerIteration;
					sample.capacityGrowthEvents += growthEvents;
					if (iteration >= config.warmupIterations) timings.push_back(elapsed);
					const U64 totalElapsed = static_cast<U64>(
						std::chrono::duration_cast<std::chrono::microseconds>(
							Clock::now() - suiteStart).count());
					if (totalElapsed > config.maximumMicroseconds)
					{
						sample.status = UiBenchmarkStatus::BudgetExceeded;
						sample.note = "Wall-clock safety budget exceeded.";
						break;
					}
				}
			}
			catch (const std::exception& error)
			{
				sample.status = UiBenchmarkStatus::Failed;
				sample.note = error.what();
			}
			catch (...)
			{
				sample.status = UiBenchmarkStatus::Failed;
				sample.note = "Unknown exception.";
			}
			sample.elapsedMicroseconds = static_cast<U64>(
				std::chrono::duration_cast<std::chrono::microseconds>(
					Clock::now() - suiteStart).count());
			if (!timings.empty())
			{
				std::sort(timings.begin(), timings.end());
				sample.minimumIterationMicroseconds = timings.front();
				sample.medianIterationMicroseconds = timings[timings.size() / 2];
				sample.maximumIterationMicroseconds = timings.back();
			}
			return sample;
		}

		UiBenchmarkSample staticNodes(const UiBenchmarkConfig& config)
		{
			UiDocument document;
			UiLayoutEngine layout(document);
			UiNodeHandle parent = document.root();
			for (UInt index = 0; index < 10000; ++index)
			{
				const UiNodeHandle node =
					document.createNode(parent, { .typeName = "cell" });
				if ((index % 100) == 99) parent = document.root();
				UiLayoutStyle style;
				style.height = UiSizeSpec::fixed(2);
				layout.setStyle(node, style);
			}
			UiLayoutStyle rootStyle;
			rootStyle.flexDirection = UiAxis::Column;
			rootStyle.flexWrap = UiWrap::Wrap;
			layout.setStyle(document.root(), rootStyle);

			return measure("static-10k-nodes", config, 10000,
				[&](UInt, U64&)
				{
					layout.layout(UiConstraints::tight(Vec2(1920, 1080)));
					return document.nodeCount();
				});
		}

		UiBenchmarkSample dirtyLeaf(const UiBenchmarkConfig& config)
		{
			UiDocument document;
			UiNodeHandle leaf = document.root();
			for (UInt index = 0; index < 10000; ++index)
				leaf = document.createNode(document.root(), { .typeName = "leaf" });
			document.processDirty();
			return measure("dirty-leaf", config, 2,
				[&](UInt, U64&)
				{
					document.markDirty(leaf, DirtyFlag::Paint);
					document.processDirty(dirty(DirtyFlag::Paint));
					return document.nodeCount();
				});
		}

		UiBenchmarkSample textRelayout(const UiBenchmarkConfig& config)
		{
			UiTextPrepareOptions prepare;
			prepare.glyphAdvance = [](U32 codepoint)
				{ return codepoint == ' ' ? 300.0f : 600.0f; };
			const UiPreparedText text = UiPreparedText::prepareUtf8(
				"Modern ORKA UI deterministic text relayout benchmark with UTF-8 \xe2\x9c\x93.",
				prepare);
			const UiTextLineLayout layout(text);
			return measure("text-relayout-1k", config, 1000,
				[&](UInt iteration, U64& growth)
				{
					Vector<UiTextLayoutResult> results;
					results.reserve(1000);
					const SizeT initialCapacity = results.capacity();
					for (UInt index = 0; index < 1000; ++index)
					{
						UiTextLayoutOptions options;
						options.fontSize = 12.0f + static_cast<Float>((index + iteration) % 8);
						results.push_back(layout.layout(
							80.0f + static_cast<Float>(index % 200), options));
					}
					if (results.capacity() != initialCapacity) ++growth;
					return results.size();
				});
		}

		UiBenchmarkSample virtualRows(const UiBenchmarkConfig& config)
		{
			UiVirtualList list(22);
			(void)list.initialize(100000, 22);
			return measure("virtual-100k-rows", config, 2000,
				[&](UInt iteration, U64&)
				{
					SizeT visible = 0;
					for (UInt query = 0; query < 2000; ++query)
					{
						const Float offset = static_cast<Float>(
							(query * 7919ULL + iteration * 17ULL) % 2'000'000ULL);
						const UiVirtualRange range =
							list.visibleRange(offset, 720, 120);
						visible += range.last - range.first;
					}
					return visible;
				});
		}

		UiBenchmarkSample atlasChurn(const UiBenchmarkConfig& config)
		{
			return measure("atlas-churn", config, 5000,
				[](UInt iteration, U64&)
				{
					UiGlyphSkylineAllocator allocator(512, 512);
					SizeT allocated = 0;
					for (UInt index = 0; index < 5000; ++index)
					{
						UiGlyphRect rect;
						const UInt width = 4 + ((index * 13 + iteration) % 45);
						const UInt height = 4 + ((index * 29 + iteration) % 45);
						if (!allocator.allocate(width, height, rect))
							allocator.reset(512, 512);
						else ++allocated;
					}
					return allocated;
				});
		}

		UiBenchmarkSample displayBatches(const UiBenchmarkConfig& config)
		{
			return measure("display-batch-count", config, 10000,
				[](UInt, U64& growth)
				{
					UiDisplayList list;
					const UiQuadGeometry quad{ .size = Vec2(8) };
					SizeT previousCapacity = list.commands().capacity();
					for (UInt index = 0; index < 10000; ++index)
					{
						(void)list.addSolidQuad(quad,
							UiLinearColor::fromStraightLinear(Vec4(1)));
						if (list.commands().capacity() != previousCapacity)
						{
							++growth;
							previousCapacity = list.commands().capacity();
						}
					}
					UiRenderPacket packet;
					Vector<UiRenderDiagnostic> diagnostics;
					if (!UiDisplayListFinalizer::finalize(list, packet, diagnostics))
						return SizeT(0);
					return packet.batches().size();
				});
		}

		UiBenchmarkSample steadyStateProxy(const UiBenchmarkConfig& config)
		{
			UiDisplayList list;
			const UiQuadGeometry quad{ .size = Vec2(8) };
			for (UInt index = 0; index < 10000; ++index)
				(void)list.addSolidQuad(quad,
					UiLinearColor::fromStraightLinear(Vec4(1)));
			list.clear();
			return measure("steady-state-allocation-proxy", config, 10000,
				[&](UInt, U64& growth)
				{
					const SizeT commandCapacity = list.commands().capacity();
					const SizeT instanceCapacity = list.instances().capacity();
					for (UInt index = 0; index < 10000; ++index)
						(void)list.addSolidQuad(quad,
							UiLinearColor::fromStraightLinear(Vec4(1)));
					if (list.commands().capacity() != commandCapacity) ++growth;
					if (list.instances().capacity() != instanceCapacity) ++growth;
					const SizeT count = list.commands().size();
					list.clear();
					return count;
				});
		}
	}

	Bool UiBenchmarkReport::completed() const
	{
		for (const UiBenchmarkSample& sample : samples)
			if (sample.status != UiBenchmarkStatus::Completed &&
				sample.status != UiBenchmarkStatus::Skipped) return false;
		return true;
	}

	UiBenchmarkReport runModernUiBenchmarks(const UiBenchmarkConfig& config)
	{
		UiBenchmarkReport report;
		report.seed = config.seed;
		report.samples.push_back(staticNodes(config));
		report.samples.push_back(dirtyLeaf(config));
		report.samples.push_back(textRelayout(config));
		report.samples.push_back(virtualRows(config));
		report.samples.push_back(atlasChurn(config));
		report.samples.push_back(displayBatches(config));
		report.samples.push_back(steadyStateProxy(config));
		return report;
	}
}
