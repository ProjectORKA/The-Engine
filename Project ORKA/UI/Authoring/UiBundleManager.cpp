#include "UiBundleManager.hpp"

#include <cctype>

namespace Ui
{
	namespace
	{
		struct TrimmedText
		{
			String text;
			SizeT offset = 0;
		};

		TrimmedText trimBundleText(const String& value, const SizeT baseOffset = 0)
		{
			SizeT first = 0;
			while (first < value.size() && std::isspace(static_cast<unsigned char>(value[first])) != 0) ++first;
			SizeT last = value.size();
			while (last > first && std::isspace(static_cast<unsigned char>(value[last - 1])) != 0) --last;
			return { value.substr(first, last - first), baseOffset + first };
		}

		void appendDiagnostics(Vector<UiDiagnostic>& target, const Vector<UiDiagnostic>& source)
		{
			for (const UiDiagnostic& diagnostic : source)
			{
				if (target.size() >= DiagnosticSink::maximumDiagnostics) return;
				target.push_back(diagnostic);
			}
		}

		void addDiagnostic(Vector<UiDiagnostic>& target, UiDiagnostic diagnostic)
		{
			if (target.size() < DiagnosticSink::maximumDiagnostics) target.push_back(std::move(diagnostic));
		}

		Bool readBoundedFile(const Path& path, const SizeT limit, String& output, Vector<UiDiagnostic>& diagnostics)
		{
			std::ifstream stream(path, std::ios::binary | std::ios::ate);
			const String sourceName = path.string();
			if (!stream.is_open())
			{
				addDiagnostic(diagnostics, { DiagnosticSeverity::Error, "Could not open UI source file.", { sourceName } });
				return false;
			}
			const std::streamoff end = stream.tellg();
			if (end < 0 || static_cast<ULL>(end) > static_cast<ULL>(limit))
			{
				addDiagnostic(diagnostics, { DiagnosticSeverity::Error, "UI source file budget exceeded.", { sourceName } });
				return false;
			}
			output.resize(static_cast<SizeT>(end));
			stream.seekg(0, std::ios::beg);
			if (!output.empty()) stream.read(output.data(), static_cast<std::streamsize>(output.size()));
			if (!stream.good() && !stream.eof())
			{
				addDiagnostic(diagnostics, { DiagnosticSeverity::Error, "Could not read UI source file.", { sourceName } });
				output.clear();
				return false;
			}
			return true;
		}

		Bool isCommandName(const String& value)
		{
			if (value.empty() || !(std::isalpha(static_cast<unsigned char>(value[0])) != 0 || value[0] == '_')) return false;
			for (const Char c : value)
				if (!(std::isalnum(static_cast<unsigned char>(c)) != 0 || c == '_' || c == '.')) return false;
			return true;
		}

		Bool isIdentifier(const String& value)
		{
			if (value.empty() ||
				!(std::isalpha(static_cast<unsigned char>(value[0])) != 0 || value[0] == '_'))
				return false;
			for (const Char c : value)
				if (!(std::isalnum(static_cast<unsigned char>(c)) != 0 || c == '_')) return false;
			return value != "true" && value != "false" && value != "null";
		}

		SourceSpan expressionSpan(const UiMarkupAttribute& attribute, const SizeT offset, const SizeT length)
		{
			SourceSpan result = attribute.valueSpan;
			result.offset += offset;
			result.length = length;
			for (SizeT i = 0; i < offset && i < attribute.value.size(); ++i)
			{
				if (attribute.value[i] == '\n')
				{
					++result.line;
					result.column = 1;
				}
				else ++result.column;
			}
			return result;
		}

		void rebaseExpression(
			UiExpressionCompileResult& expression,
			const UiMarkupAttribute& attribute,
			const SizeT valueOffset,
			const SizeT sourceLength)
		{
			const SourceSpan base = expressionSpan(attribute, valueOffset, sourceLength);
			expression.program.span = base;
			for (UiDiagnostic& diagnostic : expression.diagnostics)
			{
				if (diagnostic.severity == DiagnosticSeverity::Error)
					expression.valid = false;
				diagnostic.span.source = attribute.valueSpan.source;
				diagnostic.span.offset += base.offset;
				if (diagnostic.span.line == 1)
					diagnostic.span.column += base.column - 1;
				diagnostic.span.line += base.line - 1;
			}
		}

		struct ForParts
		{
			TrimmedText item;
			TrimmedText collection;
			TrimmedText key;
			Bool valid = false;
		};

		ForParts parseFor(const UiMarkupAttribute& attribute, Vector<UiDiagnostic>& diagnostics)
		{
			ForParts result;
			const String& value = attribute.value;
			const SizeT semicolon = value.find(';');
			if (semicolon == String::npos)
			{
				addDiagnostic(diagnostics, { DiagnosticSeverity::Error,
					"@for requires 'item of expression; track expression'.", attribute.valueSpan });
				return result;
			}
			const String firstPart = value.substr(0, semicolon);
			const SizeT ofPosition = firstPart.find(" of ");
			if (ofPosition == String::npos)
			{
				addDiagnostic(diagnostics, { DiagnosticSeverity::Error,
					"@for requires an 'item of expression' clause.", attribute.valueSpan });
				return result;
			}
			result.item = trimBundleText(firstPart.substr(0, ofPosition));
			result.collection = trimBundleText(
				firstPart.substr(ofPosition + 4),
				ofPosition + 4);

			const TrimmedText trailing = trimBundleText(value.substr(semicolon + 1), semicolon + 1);
			if (trailing.text.compare(0, 6, "track ") != 0)
			{
				addDiagnostic(diagnostics, { DiagnosticSeverity::Error,
					"@for requires a stable 'track' expression.", expressionSpan(attribute, trailing.offset, trailing.text.size()) });
				return result;
			}
			result.key = trimBundleText(trailing.text.substr(6), trailing.offset + 6);
			if (result.item.text.empty() || result.collection.text.empty() || result.key.text.empty())
			{
				addDiagnostic(diagnostics, { DiagnosticSeverity::Error,
					"@for item, collection, and track expression must be non-empty.", attribute.valueSpan });
				return result;
			}
			result.valid = true;
			return result;
		}
	}

	UiCompiledBundle UiBundleCompiler::compile(
		const U64 generation,
		const UiBundleSources& sources,
		const UiCommandRegistrySnapshot& commands,
		const UiTemplateBudgets& budgets)
	{
		UiCompiledBundle result;
		result.generation = generation;
		const UiMarkupParseResult markup = UiMarkupParser::parse(sources.markup, sources.markupName, budgets);
		result.blueprint = markup.document;
		appendDiagnostics(result.diagnostics, markup.diagnostics);
		const UiStyleCompileResult style = UiStyleCompiler::compile(
			sources.style, sources.styleName, sources.styleOptions);
		result.styles = style.sheet;
		appendDiagnostics(result.diagnostics, style.diagnostics);
		SizeT expressionCount = 0;
		SizeT expressionSourceBytes = 0;

		for (UInt nodeIndex = 0; static_cast<SizeT>(nodeIndex) < result.blueprint.nodes.size(); ++nodeIndex)
		{
			const UiMarkupNode& node = result.blueprint.nodes[nodeIndex];
			if (node.kind != UiMarkupNodeKind::Element && node.kind != UiMarkupNodeKind::Component &&
				node.kind != UiMarkupNodeKind::ForBlock && node.kind != UiMarkupNodeKind::IfBlock) continue;
			for (const UiMarkupAttribute& attribute : node.attributes)
			{
				if (result.bindings.size() + result.loops.size() + result.events.size() >= budgets.maximumBindings)
				{
					addDiagnostic(result.diagnostics, { DiagnosticSeverity::Error, "Bundle binding budget exceeded.", attribute.span });
					break;
				}

				if (attribute.kind == UiAttributeKind::PropertyBinding || attribute.kind == UiAttributeKind::IfDirective)
				{
					if (++expressionCount > budgets.maximumExpressions ||
						attribute.value.size() > budgets.maximumExpressionSourceBytes -
							std::min(expressionSourceBytes, budgets.maximumExpressionSourceBytes))
					{
						addDiagnostic(result.diagnostics, { DiagnosticSeverity::Error,
							"Template expression budget exceeded.", attribute.valueSpan });
						continue;
					}
					expressionSourceBytes += attribute.value.size();
					UiExpressionCompileResult expression = UiExpressionCompiler::compile(attribute.value, attribute.span.source);
					rebaseExpression(expression, attribute, 0, attribute.value.size());
					appendDiagnostics(result.diagnostics, expression.diagnostics);
					if (expression.valid)
						result.bindings.push_back({ nodeIndex, attribute.kind, attribute.name, std::move(expression.program) });
				}
				else if (attribute.kind == UiAttributeKind::ForDirective)
				{
					const ForParts parts = parseFor(attribute, result.diagnostics);
					if (!parts.valid) continue;
					if (!isIdentifier(parts.item.text))
					{
						addDiagnostic(result.diagnostics, { DiagnosticSeverity::Error,
							"@for item must be a simple identifier.",
							expressionSpan(attribute, parts.item.offset, parts.item.text.size()) });
						continue;
					}
					const SizeT sourceBytes = parts.collection.text.size() + parts.key.text.size();
					if (expressionCount + 2 > budgets.maximumExpressions ||
						sourceBytes > budgets.maximumExpressionSourceBytes -
							std::min(expressionSourceBytes, budgets.maximumExpressionSourceBytes))
					{
						addDiagnostic(result.diagnostics, { DiagnosticSeverity::Error,
							"Template expression budget exceeded.", attribute.valueSpan });
						continue;
					}
					expressionCount += 2;
					expressionSourceBytes += sourceBytes;
					UiExpressionCompileResult collection =
						UiExpressionCompiler::compile(parts.collection.text, attribute.span.source);
					UiExpressionCompileResult key =
						UiExpressionCompiler::compile(parts.key.text, attribute.span.source);
					rebaseExpression(collection, attribute, parts.collection.offset, parts.collection.text.size());
					rebaseExpression(key, attribute, parts.key.offset, parts.key.text.size());
					appendDiagnostics(result.diagnostics, collection.diagnostics);
					appendDiagnostics(result.diagnostics, key.diagnostics);
					const String itemPrefix = parts.item.text + ".";
					Bool keyUsesItem = false;
					for (const String& property : key.program.properties)
						if (property == parts.item.text || property.compare(0, itemPrefix.size(), itemPrefix) == 0)
						{
							keyUsesItem = true;
							break;
						}
					if (key.valid && !keyUsesItem)
						addDiagnostic(result.diagnostics, { DiagnosticSeverity::Error,
							"@for track expression must derive from the loop item to remain stable.", key.program.span });
					else if (collection.valid && key.valid)
						result.loops.push_back({
							nodeIndex,
							parts.item.text,
							std::move(collection.program),
							std::move(key.program) });
				}
				else if (attribute.kind == UiAttributeKind::EventBinding)
				{
					const String commandName = trimBundleText(attribute.value).text;
					const auto command = commands.commands.find(commandName);
					if (!isCommandName(commandName))
						addDiagnostic(result.diagnostics, { DiagnosticSeverity::Error, "Event binding requires a named command.", attribute.span });
					else if (command == commands.commands.end())
						addDiagnostic(result.diagnostics, { DiagnosticSeverity::Error, "Event command is not allowlisted: " + commandName, attribute.span });
					else if (!command->second.signature.arguments.empty())
						addDiagnostic(result.diagnostics, { DiagnosticSeverity::Error, "Markup event commands must have a zero-argument signature.", attribute.span });
					else result.events.push_back({ nodeIndex, attribute.name, commandName, command->second.version });
				}
			}
		}

		Bool hasErrors = !markup.valid || !style.valid;
		for (const UiDiagnostic& diagnostic : result.diagnostics)
			if (diagnostic.severity == DiagnosticSeverity::Error) { hasErrors = true; break; }
		result.valid = generation != 0 && !hasErrors;
		return result;
	}

	UiBundleLoadResult UiBundleCompiler::loadFile(
		const Path& markupPath,
		const Path& stylePath,
		const UiTemplateBudgets& budgets)
	{
		UiBundleLoadResult result;
		result.sources.markupName = markupPath.string();
		result.sources.styleName = stylePath.string();
		const Bool markupValid = readBoundedFile(
			markupPath,
			budgets.maximumSourceBytes,
			result.sources.markup,
			result.diagnostics);
		const Bool styleValid = readBoundedFile(
			stylePath,
			UiStyleCompiler::maximumSourceBytes,
			result.sources.style,
			result.diagnostics);

		const Path styleRoot = std::filesystem::weakly_canonical(stylePath).parent_path();
		result.sources.styleOptions.importResolver =
			[styleRoot](const String& requested, const String& fromSource, String& source, String& sourceName)
			{
				if (requested.empty()) return false;
				const Path from = fromSource.empty() ? styleRoot : Path(fromSource).parent_path();
				const Path candidate = std::filesystem::weakly_canonical(from / requested);
				const auto rootText = styleRoot.lexically_normal().generic_string();
				const auto candidateText = candidate.lexically_normal().generic_string();
				if (candidateText.compare(0, rootText.size(), rootText) != 0 ||
					(candidateText.size() > rootText.size() && candidateText[rootText.size()] != '/'))
					return false;
				Vector<UiDiagnostic> ignored;
				if (!readBoundedFile(candidate, UiStyleCompiler::maximumSourceBytes, source, ignored))
					return false;
				sourceName = candidate.string();
				return true;
			};
		result.valid = markupValid && styleValid;
		return result;
	}

	U64 UiBundleManager::requestGeneration()
	{
		LockGuard lock(mutex);
		++requested;
		if (requested == 0) ++requested;
		return requested;
	}

	Bool UiBundleManager::publish(SharedPointer<const UiCompiledBundle> candidate)
	{
		if (!candidate || !candidate->valid) return false;
		LockGuard lock(mutex);
		if (candidate->generation != requested) return false;
		active = std::move(candidate);
		return true;
	}

	SharedPointer<const UiCompiledBundle> UiBundleManager::activeBundle() const
	{
		LockGuard lock(mutex);
		return active;
	}

	U64 UiBundleManager::requestedGeneration() const
	{
		LockGuard lock(mutex);
		return requested;
	}
}
