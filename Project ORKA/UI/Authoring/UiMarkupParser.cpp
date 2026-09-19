#include "UiMarkupParser.hpp"

#include <cctype>
#include <cstdlib>

namespace Ui
{
	namespace
	{
		Bool isAsciiNameStart(const Char c)
		{
			return std::isalpha(static_cast<unsigned char>(c)) != 0 || c == '_';
		}

		Bool isAsciiNamePart(const Char c)
		{
			return std::isalnum(static_cast<unsigned char>(c)) != 0 ||
				c == '_' || c == '-' || c == '.' || c == ':';
		}

		String asciiLower(String value)
		{
			for (Char& c : value)
				c = static_cast<Char>(std::tolower(static_cast<unsigned char>(c)));
			return value;
		}

		Bool isVoidTag(const String& name)
		{
			const String lower = asciiLower(name);
			return lower == "area" || lower == "base" || lower == "br" ||
				lower == "col" || lower == "embed" || lower == "hr" ||
				lower == "img" || lower == "input" || lower == "link" ||
				lower == "meta" || lower == "param" || lower == "source" ||
				lower == "track" || lower == "wbr";
		}

		Bool isComponentName(const String& name)
		{
			return !name.empty() &&
				(std::isupper(static_cast<unsigned char>(name[0])) != 0 ||
					name.find('-') != String::npos);
		}

		Bool isBindingName(const String& name)
		{
			if (name.empty() || !isAsciiNameStart(name[0])) return false;
			for (const Char c : name)
				if (!isAsciiNamePart(c)) return false;
			return true;
		}

		void appendUtf8(String& output, const U32 codePoint)
		{
			if (codePoint <= 0x7f) output += static_cast<Char>(codePoint);
			else if (codePoint <= 0x7ff)
			{
				output += static_cast<Char>(0xc0 | (codePoint >> 6));
				output += static_cast<Char>(0x80 | (codePoint & 0x3f));
			}
			else if (codePoint <= 0xffff)
			{
				output += static_cast<Char>(0xe0 | (codePoint >> 12));
				output += static_cast<Char>(0x80 | ((codePoint >> 6) & 0x3f));
				output += static_cast<Char>(0x80 | (codePoint & 0x3f));
			}
			else
			{
				output += static_cast<Char>(0xf0 | (codePoint >> 18));
				output += static_cast<Char>(0x80 | ((codePoint >> 12) & 0x3f));
				output += static_cast<Char>(0x80 | ((codePoint >> 6) & 0x3f));
				output += static_cast<Char>(0x80 | (codePoint & 0x3f));
			}
		}

		class MarkupReader
		{
		public:
			MarkupReader(
				const String& newSource,
				String newSourceName,
				const UiTemplateBudgets& newBudgets,
				DiagnosticSink& newDiagnostics)
				: source(newSource),
				sourceName(std::move(newSourceName)),
				budgets(newBudgets),
				diagnostics(newDiagnostics)
			{
				lineStarts.push_back(0);
				for (SizeT i = 0; i < source.size(); ++i)
					if (source[i] == '\n') lineStarts.push_back(i + 1);
			}

			UiMarkupDocument run()
			{
				UiMarkupNode root;
				root.kind = UiMarkupNodeKind::Document;
				root.name = "#document";
				root.span = span(0, source.size());
				document.nodes.push_back(std::move(root));
				stack.push_back(0);

				while (cursor < source.size() && !stopped)
				{
					skipSpace();
					if (cursor >= source.size()) break;
					if (source[cursor] == '}' && tryCloseControlBlock()) continue;
					if (startsWith("@for")) { parseForBlock(); continue; }
					if (startsWith("@if")) { parseIfBlock(); continue; }
					if (source[cursor] != '<') parseText();
					else if (startsWith("<!--")) parseComment();
					else if (startsWith("</")) parseClosingTag();
					else if (startsWith("<!"))
					{
						diagnostics.error("Declarations are not supported in UI templates.", span(cursor, 2));
						skipTag();
					}
					else parseOpeningTag();
				}
				for (SizeT i = stack.size(); i > 1; --i)
				{
					const UiMarkupNode& node = document.nodes[stack[i - 1]];
					if (node.kind == UiMarkupNodeKind::ForBlock)
						diagnostics.error("Unclosed @for block.", node.span);
					else if (node.kind == UiMarkupNodeKind::IfBlock)
						diagnostics.error("Unclosed @if block.", node.span);
					else
						diagnostics.error("Unclosed element <" + node.name + ">.", node.span);
				}
				return std::move(document);
			}

		private:
			const String& source;
			String sourceName;
			const UiTemplateBudgets& budgets;
			DiagnosticSink& diagnostics;
			UiMarkupDocument document;
			Vector<UInt> stack;
			Vector<SizeT> lineStarts;
			SizeT cursor = 0;
			SizeT totalTextBytes = 0;
			Bool stopped = false;

			SourceSpan span(const SizeT offset, const SizeT length) const
			{
				SourceSpan result{ sourceName, offset, length, 1, 1 };
				const auto line = std::upper_bound(lineStarts.begin(), lineStarts.end(), offset);
				const SizeT lineIndex = line == lineStarts.begin() ? 0 : static_cast<SizeT>(line - lineStarts.begin() - 1);
				result.line = static_cast<UInt>(lineIndex + 1);
				result.column = static_cast<UInt>(offset - lineStarts[lineIndex] + 1);
				return result;
			}

			Bool startsWith(const String& text) const
			{
				return source.compare(cursor, text.size(), text) == 0;
			}

			void skipSpace()
			{
				while (cursor < source.size() && std::isspace(static_cast<unsigned char>(source[cursor])) != 0) ++cursor;
			}

			void skipTag()
			{
				while (cursor < source.size() && source[cursor] != '>') ++cursor;
				if (cursor < source.size()) ++cursor;
			}

			String readName(SourceSpan* nameSpan = nullptr)
			{
				const SizeT start = cursor;
				if (cursor >= source.size() || !isAsciiNameStart(source[cursor])) return {};
				++cursor;
				while (cursor < source.size() && isAsciiNamePart(source[cursor])) ++cursor;
				if (nameSpan) *nameSpan = span(start, cursor - start);
				if (cursor - start > budgets.maximumNameBytes)
					diagnostics.error("Name budget exceeded.", span(start, cursor - start));
				return source.substr(start, cursor - start);
			}

			String decodeEntities(const SizeT start, const SizeT length)
			{
				String output;
				output.reserve(length);
				const SizeT end = start + length;
				for (SizeT i = start; i < end;)
				{
					if (source[i] != '&')
					{
						output += source[i++];
						continue;
					}
					const SizeT semicolon = source.find(';', i + 1);
					if (semicolon == String::npos || semicolon >= end || semicolon - i > 12)
					{
						diagnostics.error("Malformed entity reference.", span(i, 1));
						output += source[i++];
						continue;
					}
					const String entity = source.substr(i + 1, semicolon - i - 1);
					if (entity == "amp") output += '&';
					else if (entity == "lt") output += '<';
					else if (entity == "gt") output += '>';
					else if (entity == "quot") output += '"';
					else if (entity == "apos") output += '\'';
					else if (!entity.empty() && entity[0] == '#')
					{
						const Bool hexadecimal = entity.size() > 1 && (entity[1] == 'x' || entity[1] == 'X');
						const String digits = entity.substr(hexadecimal ? 2 : 1);
						Char* parseEnd = nullptr;
						const unsigned long value = std::strtoul(digits.c_str(), &parseEnd, hexadecimal ? 16 : 10);
						if (digits.empty() || !parseEnd || *parseEnd != '\0' || value == 0 ||
							value > 0x10ffff || (value >= 0xd800 && value <= 0xdfff))
						{
							diagnostics.error("Invalid numeric entity.", span(i, semicolon - i + 1));
							output += "\xef\xbf\xbd";
						}
						else appendUtf8(output, static_cast<U32>(value));
					}
					else
					{
						diagnostics.error("Unknown entity '&" + entity + ";'.", span(i, semicolon - i + 1));
						output.append(source, i, semicolon - i + 1);
					}
					i = semicolon + 1;
				}
				return output;
			}

			String readQuotedValue(SourceSpan& valueSpan)
			{
				if (cursor >= source.size() || (source[cursor] != '"' && source[cursor] != '\''))
				{
					diagnostics.error("Attribute values must be quoted.", span(cursor, cursor < source.size() ? 1 : 0));
					valueSpan = span(cursor, 0);
					return {};
				}
				const Char quote = source[cursor++];
				const SizeT start = cursor;
				while (cursor < source.size() && source[cursor] != quote) ++cursor;
				valueSpan = span(start, cursor - start);
				if (cursor >= source.size())
				{
					diagnostics.error("Unterminated attribute value.", span(start, cursor - start));
					return decodeEntities(start, cursor - start);
				}
				String value = decodeEntities(start, cursor - start);
				++cursor;
				if (value.size() > budgets.maximumAttributeValueBytes)
					diagnostics.error("Attribute value budget exceeded.", valueSpan);
				return value;
			}

			Bool addNode(UiMarkupNode node, UInt* addedIndex = nullptr)
			{
				if (document.nodes.size() >= budgets.maximumNodes)
				{
					diagnostics.error("Markup node budget exceeded.", node.span);
					stopped = true;
					return false;
				}
				const UInt index = static_cast<UInt>(document.nodes.size());
				document.nodes.push_back(std::move(node));
				document.nodes[stack.back()].children.push_back(index);
				if (addedIndex) *addedIndex = index;
				return true;
			}

			Bool tryCloseControlBlock()
			{
				if (stack.size() <= 1) return false;
				const UiMarkupNode& open = document.nodes[stack.back()];
				if (open.kind != UiMarkupNodeKind::ForBlock && open.kind != UiMarkupNodeKind::IfBlock) return false;
				stack.pop_back();
				++cursor;
				return true;
			}

			void parseForBlock()
			{
				const SizeT start = cursor;
				cursor += 5; // "@for"
				skipSpace();
				if (cursor >= source.size() || source[cursor] != '(')
				{
					diagnostics.error("@for requires '(item of collection; track key)'.", span(start, cursor - start));
					return;
				}
				++cursor;
				skipSpace();
				const SizeT headerStart = cursor;
				SizeT depth = 1;
				while (cursor < source.size() && depth > 0)
				{
					if (source[cursor] == '(') ++depth;
					else if (source[cursor] == ')') --depth;
					if (depth > 0) ++cursor;
				}
				if (depth != 0)
				{
					diagnostics.error("Unterminated @for header.", span(start, cursor - start));
					return;
				}
				const String header = source.substr(headerStart, cursor - headerStart);
				++cursor; // ')'
				skipSpace();
				if (cursor >= source.size() || source[cursor] != '{')
				{
					diagnostics.error("@for block requires '{' after the header.", span(start, cursor - start));
					return;
				}
				++cursor; // '{'

				UiMarkupNode node;
				node.kind = UiMarkupNodeKind::ForBlock;
				node.name = "@for";
				node.span = span(start, cursor - start);
				UiMarkupAttribute forDirective;
				forDirective.kind = UiAttributeKind::ForDirective;
				forDirective.name = "for";
				forDirective.value = header;
				forDirective.valueSpan = span(headerStart, header.size());
				forDirective.span = node.span;
				node.attributes.push_back(std::move(forDirective));

				UInt index = 0;
				if (!addNode(std::move(node), &index)) return;
				if (stack.size() >= budgets.maximumDepth)
				{
					diagnostics.error("Markup nesting budget exceeded.", document.nodes[index].span);
					return;
				}
				stack.push_back(index);
			}

			void parseIfBlock()
			{
				const SizeT start = cursor;
				cursor += 4; // "@if"
				skipSpace();
				if (cursor >= source.size() || source[cursor] != '(')
				{
					diagnostics.error("@if requires '(condition)'.", span(start, cursor - start));
					return;
				}
				++cursor;
				skipSpace();
				const SizeT headerStart = cursor;
				SizeT depth = 1;
				while (cursor < source.size() && depth > 0)
				{
					if (source[cursor] == '(') ++depth;
					else if (source[cursor] == ')') --depth;
					if (depth > 0) ++cursor;
				}
				if (depth != 0)
				{
					diagnostics.error("Unterminated @if condition.", span(start, cursor - start));
					return;
				}
				const String condition = source.substr(headerStart, cursor - headerStart);
				++cursor; // ')'
				skipSpace();
				if (cursor >= source.size() || source[cursor] != '{')
				{
					diagnostics.error("@if block requires '{' after the condition.", span(start, cursor - start));
					return;
				}
				++cursor; // '{'

				UiMarkupNode node;
				node.kind = UiMarkupNodeKind::IfBlock;
				node.name = "@if";
				node.span = span(start, cursor - start);
				UiMarkupAttribute ifDirective;
				ifDirective.kind = UiAttributeKind::IfDirective;
				ifDirective.name = "if";
				ifDirective.value = condition;
				ifDirective.valueSpan = span(headerStart, condition.size());
				ifDirective.span = node.span;
				node.attributes.push_back(std::move(ifDirective));

				UInt index = 0;
				if (!addNode(std::move(node), &index)) return;
				if (stack.size() >= budgets.maximumDepth)
				{
					diagnostics.error("Markup nesting budget exceeded.", document.nodes[index].span);
					return;
				}
				stack.push_back(index);
			}

			void parseText()
			{
				const SizeT start = cursor;
				while (cursor < source.size() && source[cursor] != '<' && source[cursor] != '}' &&
					!(source[cursor] == '@' && (startsWith("@for") || startsWith("@if")))) ++cursor;
				if (cursor == start)
				{
					// Lone control characters (for example a stray '}') must not stall the parser.
					diagnostics.error("Unexpected character in markup text.", span(cursor, 1));
					++cursor;
					return;
				}
				UiMarkupNode node;
				node.kind = UiMarkupNodeKind::Text;
				node.text = decodeEntities(start, cursor - start);
				node.span = span(start, cursor - start);
				if (node.text.size() > budgets.maximumTextNodeBytes)
					diagnostics.error("Text node budget exceeded.", node.span);
				if (node.text.size() > budgets.maximumTotalTextBytes - std::min(totalTextBytes, budgets.maximumTotalTextBytes))
					diagnostics.error("Total template text budget exceeded.", node.span);
				else totalTextBytes += node.text.size();
				(void)addNode(std::move(node));
			}

			void parseComment()
			{
				const SizeT start = cursor;
				const SizeT end = source.find("-->", cursor + 4);
				if (end == String::npos)
				{
					diagnostics.error("Unterminated comment.", span(start, source.size() - start));
					cursor = source.size();
					return;
				}
				UiMarkupNode node;
				node.kind = UiMarkupNodeKind::Comment;
				node.text = source.substr(cursor + 4, end - cursor - 4);
				node.span = span(start, end + 3 - start);
				if (node.text.size() > budgets.maximumTextNodeBytes)
					diagnostics.error("Comment budget exceeded.", node.span);
				if (node.text.size() > budgets.maximumTotalTextBytes - std::min(totalTextBytes, budgets.maximumTotalTextBytes))
					diagnostics.error("Total template text budget exceeded.", node.span);
				else totalTextBytes += node.text.size();
				cursor = end + 3;
				(void)addNode(std::move(node));
			}

			void parseClosingTag()
			{
				const SizeT start = cursor;
				cursor += 2;
				skipSpace();
				const String name = readName();
				skipSpace();
				if (cursor >= source.size() || source[cursor] != '>')
				{
					diagnostics.error("Malformed closing tag.", span(start, cursor - start));
					while (cursor < source.size() && source[cursor] != '>') ++cursor;
				}
				if (cursor < source.size()) ++cursor;
				if (stack.size() <= 1)
				{
					diagnostics.error("Closing tag has no open element.", span(start, cursor - start));
					return;
				}
				const UiMarkupNode& openNode = document.nodes[stack.back()];
				const Bool matches = openNode.kind == UiMarkupNodeKind::Component ?
					openNode.name == name : asciiLower(openNode.name) == asciiLower(name);
				if (!matches)
				{
					diagnostics.error("Closing tag </" + name + "> does not match <" + openNode.name + ">.", span(start, cursor - start));
					return;
				}
				stack.pop_back();
			}

			UiMarkupAttribute readAttribute()
			{
				const SizeT start = cursor;
				UiMarkupAttribute attribute;
				if (source[cursor] == '[' || source[cursor] == '(')
				{
					const Char opening = source[cursor++];
					const Char closing = opening == '[' ? ']' : ')';
					const SizeT nameStart = cursor;
					while (cursor < source.size() && source[cursor] != closing) ++cursor;
					if (cursor >= source.size())
					{
						diagnostics.error("Unterminated binding attribute.", span(start, cursor - start));
						attribute.span = span(start, cursor - start);
						return attribute;
					}
					attribute.name = source.substr(nameStart, cursor - nameStart);
					attribute.nameSpan = span(nameStart, cursor - nameStart);
					++cursor;
					attribute.kind = opening == '[' ? UiAttributeKind::PropertyBinding : UiAttributeKind::EventBinding;
				}
				else if (source[cursor] == '*')
				{
					++cursor;
					attribute.name = readName(&attribute.nameSpan);
					if (attribute.name == "if")
						diagnostics.error("Use '@if (condition) { ... }' instead of '*if'.", span(start, cursor - start));
					else if (attribute.name == "for")
						diagnostics.error("Use '@for (item of collection; track key) { ... }' instead of '*for'.", span(start, cursor - start));
					else diagnostics.error("Unknown structural directive '*" + attribute.name + "'.", span(start, cursor - start));
				}
				else attribute.name = readName(&attribute.nameSpan);

				if (attribute.name.empty())
					diagnostics.error("Expected attribute name.", span(start, std::max(SizeT(1), cursor - start)));
				if (attribute.name.size() > budgets.maximumNameBytes)
					diagnostics.error("Attribute name budget exceeded.", attribute.nameSpan);
				skipSpace();
				if (cursor < source.size() && source[cursor] == '=')
				{
					++cursor;
					skipSpace();
					attribute.value = readQuotedValue(attribute.valueSpan);
				}
				else if (attribute.kind != UiAttributeKind::Literal)
					diagnostics.error("Bindings and directives require a value.", span(start, cursor - start));
				else attribute.valueSpan = span(cursor, 0);

				attribute.span = span(start, cursor - start);
				return attribute;
			}

			void validateAttribute(UiMarkupNode& node, const UiMarkupAttribute& attribute)
			{
				const String lower = asciiLower(attribute.name);
				for (SizeT i = 0; i + 1 < node.attributes.size(); ++i)
					if (asciiLower(node.attributes[i].name) == lower &&
						node.attributes[i].kind == attribute.kind)
					{
						diagnostics.error("Duplicate attribute '" + attribute.name + "'.", attribute.nameSpan);
						break;
					}

				if (attribute.kind == UiAttributeKind::Literal)
				{
					if (lower == "style")
						diagnostics.error("Inline style attributes are not allowed.", attribute.nameSpan);
					else if (lower.size() > 2 && lower[0] == 'o' && lower[1] == 'n')
						diagnostics.error("Native event attributes are not allowed; use '(event)=\"Command\"'.", attribute.nameSpan);
					else if (lower == "srcdoc" || lower == "innerhtml" || lower == "outerhtml" ||
						lower == "__proto__" || lower == "prototype" || lower == "constructor")
						diagnostics.error("Unsafe attribute name is reserved.", attribute.nameSpan);
					if (lower == "id") node.id = attribute.value;
					else if (lower == "class")
					{
						SizeT at = 0;
						while (at < attribute.value.size())
						{
							while (at < attribute.value.size() &&
								std::isspace(static_cast<unsigned char>(attribute.value[at])) != 0) ++at;
							const SizeT first = at;
							while (at < attribute.value.size() &&
								std::isspace(static_cast<unsigned char>(attribute.value[at])) == 0) ++at;
							if (at > first) node.classes.push_back(attribute.value.substr(first, at - first));
						}
					}
				}
				else if (attribute.kind == UiAttributeKind::PropertyBinding)
				{
					if (!isBindingName(attribute.name) || lower == "style" || lower == "innerhtml" || lower == "outerhtml" ||
						lower == "srcdoc" || lower == "native" || lower == "__proto__" ||
						lower == "prototype" || lower == "constructor" ||
						(lower.size() > 2 && lower[0] == 'o' && lower[1] == 'n'))
						diagnostics.error("Unsafe property binding target is reserved.", attribute.nameSpan);
				}
				else if (attribute.kind == UiAttributeKind::EventBinding &&
					(!isBindingName(attribute.name) || lower == "native" ||
						lower == "script" || lower == "__proto__" ||
						lower == "prototype" || lower == "constructor"))
					diagnostics.error("Unsafe or malformed event name.", attribute.nameSpan);

				if (attribute.kind == UiAttributeKind::IfDirective ||
					attribute.kind == UiAttributeKind::ForDirective)
				for (SizeT i = 0; i + 1 < node.attributes.size(); ++i)
					if (node.attributes[i].kind == UiAttributeKind::IfDirective ||
						node.attributes[i].kind == UiAttributeKind::ForDirective)
					{
						diagnostics.error("Only one structural directive is allowed per element.", attribute.nameSpan);
						break;
					}
			}

			void parseOpeningTag()
			{
				const SizeT start = cursor++;
				skipSpace();
				const String name = readName();
				if (name.empty())
				{
					diagnostics.error("Expected element name.", span(start, 1));
					while (cursor < source.size() && source[cursor] != '>') ++cursor;
					if (cursor < source.size()) ++cursor;
					return;
				}

				UiMarkupNode node;
				node.name = name;
				node.kind = isComponentName(name) ? UiMarkupNodeKind::Component : UiMarkupNodeKind::Element;
				node.span = span(start, 0);
				if (asciiLower(name) == "script")
					diagnostics.error("Script elements are not allowed in UI templates.", span(start + 1, name.size()));
				Bool selfClosing = false;
				while (cursor < source.size())
				{
					skipSpace();
					if (cursor >= source.size())
					{
						diagnostics.error("Unterminated opening tag.", span(start, cursor - start));
						break;
					}
					if (startsWith("/>")) { cursor += 2; selfClosing = true; break; }
					if (source[cursor] == '>') { ++cursor; break; }
					if (node.attributes.size() >= budgets.maximumAttributesPerElement)
					{
						diagnostics.error("Attribute budget exceeded.", span(cursor, 1));
						skipTag();
						break;
					}
					const SizeT before = cursor;
					node.attributes.push_back(readAttribute());
					validateAttribute(node, node.attributes.back());
					if (cursor == before)
					{
						diagnostics.error("Parser made no progress in opening tag.", span(cursor, 1));
						++cursor;
					}
					if (cursor >= source.size()) break;
				}
				node.span.length = cursor - start;
				UInt index = 0;
				if (!addNode(std::move(node), &index)) return;
				if (!selfClosing && !isVoidTag(name))
				{
					if (stack.size() >= budgets.maximumDepth)
						diagnostics.error("Markup nesting budget exceeded.", document.nodes[index].span);
					else stack.push_back(index);
				}
			}
		};
	}

	UiMarkupParseResult UiMarkupParser::parse(
		const String& source,
		const String& sourceName,
		const UiTemplateBudgets& budgets)
	{
		UiMarkupParseResult result;
		DiagnosticSink diagnostics;
		if (budgets.maximumNodes == 0 || budgets.maximumDepth == 0)
			diagnostics.error("Template budgets must allow at least one node and one level.", { sourceName });
		else if (source.size() > budgets.maximumSourceBytes)
			diagnostics.error("Markup source budget exceeded.", { sourceName, 0, source.size(), 1, 1 });
		else
		{
			MarkupReader reader(source, sourceName, budgets, diagnostics);
			result.document = reader.run();
		}
		result.valid = !diagnostics.hasErrors();
		result.diagnostics = std::move(diagnostics.items);
		return result;
	}
}
