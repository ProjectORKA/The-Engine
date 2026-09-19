#pragma once

#include "UiValidation.hpp"

namespace Ui::Testing
{
	enum class UiBenchmarkStatus : Byte
	{
		Completed,
		Skipped,
		BudgetExceeded,
		Failed,
	};

	struct UiBenchmarkSample
	{
		String name;
		UiBenchmarkStatus status = UiBenchmarkStatus::Completed;
		U64 operations = 0;
		U64 elapsedMicroseconds = 0;
		U64 minimumIterationMicroseconds = 0;
		U64 medianIterationMicroseconds = 0;
		U64 maximumIterationMicroseconds = 0;
		SizeT outputCount = 0;
		// Capacity growth is a deterministic, allocator-independent proxy. It
		// does not claim to be a count of global heap allocations.
		U64 capacityGrowthEvents = 0;
		String note;
	};

	struct UiBenchmarkConfig
	{
		U64 seed = 0x42454e43485549ULL;
		UInt warmupIterations = 1;
		UInt measuredIterations = 5;
		U64 maximumOperations = 5'000'000;
		U64 maximumMicroseconds = 10'000'000;
	};

	struct UiBenchmarkReport
	{
		U64 seed = 0;
		Vector<UiBenchmarkSample> samples;

		[[nodiscard]] Bool completed() const;
	};

	[[nodiscard]] UiBenchmarkReport runModernUiBenchmarks(
		const UiBenchmarkConfig& config = {});
}
