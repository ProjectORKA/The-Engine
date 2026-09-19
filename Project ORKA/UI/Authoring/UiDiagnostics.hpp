#pragma once

#include "../../Basics.hpp"

namespace Ui
{
	struct SourceSpan
	{
		String source;
		SizeT offset = 0;
		SizeT length = 0;
		UInt line = 1;
		UInt column = 1;
	};

	enum class DiagnosticSeverity : Byte
	{
		Warning,
		Error,
	};

	struct UiDiagnostic
	{
		DiagnosticSeverity severity = DiagnosticSeverity::Error;
		String message;
		SourceSpan span;
	};

	class DiagnosticSink
	{
	public:
		static constexpr SizeT maximumDiagnostics = 256;

		void add(DiagnosticSeverity severity, String message, SourceSpan span)
		{
			if (severity == DiagnosticSeverity::Error) encounteredError = true;
			if (items.size() >= maximumDiagnostics)
			{
				truncated = true;
				return;
			}
			items.push_back({ severity, std::move(message), std::move(span) });
		}

		void error(String message, SourceSpan span) { add(DiagnosticSeverity::Error, std::move(message), std::move(span)); }
		void warning(String message, SourceSpan span) { add(DiagnosticSeverity::Warning, std::move(message), std::move(span)); }
		[[nodiscard]] Bool hasErrors() const
		{
			return encounteredError || truncated;
		}

		Vector<UiDiagnostic> items;
		Bool truncated = false;
		Bool encounteredError = false;
	};
}
