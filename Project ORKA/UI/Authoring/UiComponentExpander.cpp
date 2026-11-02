#include "UiComponentExpander.hpp"

#include "UiMarkupParser.hpp"

#include <cctype>
#include <fstream>

namespace Ui
{
	namespace
	{
		String lower(String value)
		{
			for (Char& character : value)
				character = static_cast<Char>(std::tolower(static_cast<unsigned char>(character)));
			return value;
		}

		String trim(String value)
		{
			SizeT first = 0;
			while (first < value.size() && std::isspace(static_cast<unsigned char>(value[first])) != 0) ++first;
			SizeT last = value.size();
			while (last > first && std::isspace(static_cast<unsigned char>(value[last - 1])) != 0) --last;
			return value.substr(first, last - first);
		}

		Bool readMarkupFile(const Path& path, String& output, Vector<UiDiagnostic>& diagnostics)
		{
			std::ifstream stream(path, std::ios::binary | std::ios::ate);
			if (!stream.is_open())
			{
				if (diagnostics.size() < DiagnosticSink::maximumDiagnostics)
					diagnostics.push_back({ DiagnosticSeverity::Error,
						"Could not open component markup: " + path.string(), {} });
				return false;
			}
			const std::streamoff end = stream.tellg();
			if (end < 0 || static_cast<ULL>(end) > static_cast<ULL>(UiTemplateBudgets{}.maximumSourceBytes))
			{
				if (diagnostics.size() < DiagnosticSink::maximumDiagnostics)
					diagnostics.push_back({ DiagnosticSeverity::Error,
						"Component markup budget exceeded: " + path.string(), {} });
				return false;
			}
			output.resize(static_cast<SizeT>(end));
			stream.seekg(0, std::ios::beg);
			if (!output.empty()) stream.read(output.data(), static_cast<std::streamsize>(output.size()));
			return true;
		}

		Int findParentChildIndex(const UiMarkupDocument& document, const UInt childIndex)
		{
			for (UInt parentIndex = 0; parentIndex < document.nodes.size(); ++parentIndex)
			{
				const UiMarkupNode& parent = document.nodes[parentIndex];
				for (Int slot = 0; slot < static_cast<Int>(parent.children.size()); ++slot)
					if (parent.children[static_cast<SizeT>(slot)] == childIndex) return slot;
			}
			return -1;
		}

		UInt findParentIndex(const UiMarkupDocument& document, const UInt childIndex)
		{
			for (UInt parentIndex = 0; parentIndex < document.nodes.size(); ++parentIndex)
			{
				const UiMarkupNode& parent = document.nodes[parentIndex];
				for (const UInt index : parent.children)
					if (index == childIndex) return parentIndex;
			}
			return maxUInt;
		}

		void appendUniqueClasses(Vector<String>& target, const Vector<String>& extra)
		{
			for (const String& value : extra)
			{
				if (std::find(target.begin(), target.end(), value) == target.end())
					target.push_back(value);
			}
		}

		Map<String, UiMarkupAttribute> collectComponentProps(const UiMarkupNode& componentNode)
		{
			Map<String, UiMarkupAttribute> props;
			for (const UiMarkupAttribute& attribute : componentNode.attributes)
			{
				if (attribute.kind != UiAttributeKind::Literal &&
					attribute.kind != UiAttributeKind::PropertyBinding) continue;
				props[lower(attribute.name)] = attribute;
			}
			return props;
		}

		void applyPropPassthrough(UiMarkupNode& node, const Map<String, UiMarkupAttribute>& props)
		{
			for (UiMarkupAttribute& attribute : node.attributes)
			{
				if (attribute.kind != UiAttributeKind::PropertyBinding) continue;
				const String propName = trim(attribute.value);
				const auto found = props.find(lower(propName));
				if (found == props.end()) continue;
				const UiMarkupAttribute& source = found->second;
				if (source.kind == UiAttributeKind::PropertyBinding)
				{
					attribute.value = source.value;
					attribute.valueSpan = source.valueSpan;
				}
				else
				{
					attribute.kind = UiAttributeKind::Literal;
					attribute.value = source.value;
					attribute.valueSpan = source.valueSpan;
				}
			}
		}

		UInt cloneMarkupNode(
			const UiMarkupDocument& source,
			const UInt sourceIndex,
			UiMarkupDocument& destination,
			const Map<String, UiMarkupAttribute>& props)
		{
			if (sourceIndex >= source.nodes.size()) return maxUInt;
			const UiMarkupNode& original = source.nodes[sourceIndex];
			UiMarkupNode copy = original;
			copy.children.clear();
			applyPropPassthrough(copy, props);
			const UInt destinationIndex = static_cast<UInt>(destination.nodes.size());
			destination.nodes.push_back(std::move(copy));
			for (const UInt child : original.children)
			{
				const UInt clonedChild = cloneMarkupNode(source, child, destination, props);
				if (clonedChild != maxUInt) destination.nodes[destinationIndex].children.push_back(clonedChild);
			}
			return destinationIndex;
		}

		UInt fragmentRootChild(const UiMarkupDocument& fragment)
		{
			if (fragment.nodes.empty()) return maxUInt;
			const UiMarkupNode& root = fragment.nodes[fragment.rootIndex];
			if (root.children.empty()) return maxUInt;
			if (root.children.size() == 1) return root.children.front();
			const UiMarkupNode& first = fragment.nodes[root.children.front()];
			if (lower(first.name) == "ui" && !first.children.empty()) return first.children.front();
			return root.children.front();
		}

		Vector<UInt> fragmentTopLevelChildren(const UiMarkupDocument& fragment)
		{
			Vector<UInt> result;
			if (fragment.nodes.empty()) return result;
			const UiMarkupNode& root = fragment.nodes[fragment.rootIndex];
			UInt containerIndex = fragment.rootIndex;
			if (root.children.size() == 1)
			{
				const UiMarkupNode& only = fragment.nodes[root.children.front()];
				if (lower(only.name) == "ui")
				{
					containerIndex = root.children.front();
					for (const UInt child : fragment.nodes[containerIndex].children)
						result.push_back(child);
					return result;
				}
			}
			for (const UInt child : root.children) result.push_back(child);
			return result;
		}
	}

	String preprocessUiImports(
		String markup,
		const Path& uiDirectory,
		UiComponentRegistry& registry,
		Vector<UiDiagnostic>& diagnostics)
	{
		String output;
		output.reserve(markup.size());
		SizeT lineStart = 0;
		while (lineStart <= markup.size())
		{
			SizeT lineEnd = markup.find('\n', lineStart);
			if (lineEnd == String::npos) lineEnd = markup.size();
			const String line = markup.substr(lineStart, lineEnd - lineStart);
			const String trimmed = trim(line);
			if (trimmed.starts_with("@import"))
			{
				const SizeT quoteStart = trimmed.find('"');
				const SizeT quoteEnd = quoteStart == String::npos ? String::npos : trimmed.find('"', quoteStart + 1);
				const SizeT asPosition = lower(trimmed).find(" as ");
				if (quoteStart == String::npos || quoteEnd == String::npos || asPosition == String::npos)
				{
					if (diagnostics.size() < DiagnosticSink::maximumDiagnostics)
						diagnostics.push_back({ DiagnosticSeverity::Error,
							"@import requires: @import \"file.ui.html\" as tag-name;", {} });
				}
				else
				{
					const String file = trimmed.substr(quoteStart + 1, quoteEnd - quoteStart - 1);
					String tag = trim(trimmed.substr(asPosition + 4));
					if (!tag.empty() && tag.back() == ';') tag.pop_back();
					tag = trim(tag);
					if (tag.empty())
					{
						if (diagnostics.size() < DiagnosticSink::maximumDiagnostics)
							diagnostics.push_back({ DiagnosticSeverity::Error,
								"@import tag name must be non-empty.", {} });
					}
					else registry.registerComponent(tag, uiDirectory / file);
				}
			}
			else
			{
				output.append(line);
				if (lineEnd < markup.size()) output.push_back('\n');
			}
			if (lineEnd == markup.size()) break;
			lineStart = lineEnd + 1;
		}
		return output;
	}

	Bool expandRegisteredComponents(
		UiMarkupDocument& document,
		const Path& uiDirectory,
		UiComponentRegistry& registry,
		const UiTemplateBudgets& budgets,
		Vector<UiDiagnostic>& diagnostics)
	{
		Bool expandedAny = true;
		Int passes = 0;
		while (expandedAny && passes < 32)
		{
			expandedAny = false;
			++passes;
			for (UInt nodeIndex = 0; nodeIndex < document.nodes.size(); ++nodeIndex)
			{
				UiMarkupNode& componentNode = document.nodes[nodeIndex];
				if (componentNode.kind != UiMarkupNodeKind::Component) continue;
				const UiRegisteredComponent* definition = registry.find(componentNode.name);
				if (!definition) continue;

				String fragmentSource;
				if (!readMarkupFile(definition->markupPath, fragmentSource, diagnostics)) return false;
				const UiMarkupParseResult fragmentParse =
					UiMarkupParser::parse(fragmentSource, definition->markupPath.string(), budgets);
				for (const UiDiagnostic& diagnostic : fragmentParse.diagnostics)
				{
					if (diagnostics.size() >= DiagnosticSink::maximumDiagnostics) break;
					diagnostics.push_back(diagnostic);
				}
				if (!fragmentParse.valid) return false;

				const Map<String, UiMarkupAttribute> props = collectComponentProps(componentNode);
				const Vector<UInt> fragmentChildren = fragmentTopLevelChildren(fragmentParse.document);
				if (fragmentChildren.empty())
				{
					if (diagnostics.size() < DiagnosticSink::maximumDiagnostics)
						diagnostics.push_back({ DiagnosticSeverity::Error,
							"Component fragment has no content: " + componentNode.name, componentNode.span });
					return false;
				}

				// cloneMarkupNode appends to document.nodes and can reallocate it.
				// Snapshot anything still needed from this node before that happens.
				const String componentName = componentNode.name;
				const String componentId = componentNode.id;
				const Vector<String> componentClasses = componentNode.classes;
				const SourceSpan componentSpan = componentNode.span;

				Vector<UInt> clonedRoots;
				clonedRoots.reserve(fragmentChildren.size());
				for (const UInt fragmentChild : fragmentChildren)
				{
					const UInt cloned = cloneMarkupNode(
						fragmentParse.document, fragmentChild, document, props);
					if (cloned == maxUInt) return false;
					clonedRoots.push_back(cloned);
				}

				if (!componentClasses.empty())
				{
					UiMarkupNode& primary = document.nodes[clonedRoots.front()];
					appendUniqueClasses(primary.classes, componentClasses);
				}
				if (!componentId.empty())
					document.nodes[clonedRoots.front()].id = componentId;

				const UInt parentIndex = findParentIndex(document, nodeIndex);
				if (parentIndex == maxUInt)
				{
					if (diagnostics.size() < DiagnosticSink::maximumDiagnostics)
						diagnostics.push_back({ DiagnosticSeverity::Error,
							"Component node has no parent: " + componentName, componentSpan });
					return false;
				}
				UiMarkupNode& parent = document.nodes[parentIndex];
				const Int slot = findParentChildIndex(document, nodeIndex);
				if (slot < 0) return false;

				parent.children.erase(parent.children.begin() + slot);
				for (Int insert = static_cast<Int>(clonedRoots.size()) - 1; insert >= 0; --insert)
					parent.children.insert(parent.children.begin() + slot, clonedRoots[static_cast<SizeT>(insert)]);

				UiMarkupNode& expanded = document.nodes[nodeIndex];
				expanded.kind = UiMarkupNodeKind::Comment;
				expanded.name.clear();
				expanded.id.clear();
				expanded.classes.clear();
				expanded.children.clear();
				expanded.attributes.clear();
				expandedAny = true;
				break;
			}
		}
		return true;
	}
}
