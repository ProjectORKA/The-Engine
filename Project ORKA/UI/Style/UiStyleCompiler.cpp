#include "UiStyleCompiler.hpp"

#include <cctype>
#include <cstdlib>

namespace Ui
{
	namespace
	{
		String trim(const String& value)
		{
			SizeT first = 0;
			while (first < value.size() && std::isspace(static_cast<unsigned char>(value[first]))) ++first;
			SizeT last = value.size();
			while (last > first && std::isspace(static_cast<unsigned char>(value[last - 1]))) --last;
			return value.substr(first, last - first);
		}

		String lower(String value)
		{
			for (Char& c : value) c = static_cast<Char>(std::tolower(static_cast<unsigned char>(c)));
			return value;
		}

		Bool identifierStart(const Char c)
		{
			return std::isalpha(static_cast<unsigned char>(c)) != 0 || c == '_' || c == '-';
		}

		Bool identifierChar(const Char c)
		{
			return std::isalnum(static_cast<unsigned char>(c)) != 0 || c == '_' || c == '-';
		}

		Bool containsForbidden(const String& value)
		{
			const String text = lower(value);
			return text.find("url(") != String::npos || text.find("javascript:") != String::npos ||
				text.find("<script") != String::npos || text.find("@native") != String::npos ||
				text.find("@legacy") != String::npos || text.find("expression(") != String::npos ||
				text.find("-ork-") != String::npos;
		}

		Vector<String> splitTopLevel(const String& value, const Char delimiter)
		{
			Vector<String> result;
			SizeT start = 0;
			Int parentheses = 0;
			Char quote = 0;
			for (SizeT i = 0; i <= value.size(); ++i)
			{
				if (i < value.size())
				{
					const Char c = value[i];
					if (quote)
					{
						if (c == '\\') ++i;
						else if (c == quote) quote = 0;
					}
					else if (c == '"' || c == '\'') quote = c;
					else if (c == '(') ++parentheses;
					else if (c == ')') --parentheses;
				}
				if (i == value.size() || (value[i] == delimiter && parentheses == 0 && quote == 0))
				{
					result.push_back(trim(value.substr(start, i - start)));
					start = i + 1;
				}
			}
			return result;
		}

		Bool parseHexColor(const String& text, Color& color)
		{
			if (text.empty() || text[0] != '#' || (text.size() != 4 && text.size() != 5 && text.size() != 7 && text.size() != 9)) return false;
			auto nibble = [](const Char c) -> Int
				{
					if (c >= '0' && c <= '9') return c - '0';
					if (c >= 'a' && c <= 'f') return c - 'a' + 10;
					if (c >= 'A' && c <= 'F') return c - 'A' + 10;
					return -1;
				};
			Vector<Int> bytes;
			if (text.size() <= 5)
			{
				for (SizeT i = 1; i < text.size(); ++i)
				{
					const Int n = nibble(text[i]);
					if (n < 0) return false;
					bytes.push_back(n * 17);
				}
			}
			else
			{
				for (SizeT i = 1; i < text.size(); i += 2)
				{
					const Int a = nibble(text[i]), b = nibble(text[i + 1]);
					if (a < 0 || b < 0) return false;
					bytes.push_back(a * 16 + b);
				}
			}
			color = Color(bytes[0] / 255.0f, bytes[1] / 255.0f, bytes[2] / 255.0f,
				bytes.size() == 4 ? bytes[3] / 255.0f : 1.0f);
			return true;
		}

		UiStyleValue parseValue(const String& raw)
		{
			UiStyleValue result;
			result.text = raw;
			const String value = trim(raw);
			const String folded = lower(value);
			if (folded == "auto") { result.kind = UiStyleValueKind::Auto; return result; }
			if (folded == "content") { result.kind = UiStyleValueKind::Content; return result; }
			if (parseHexColor(value, result.color)) { result.kind = UiStyleValueKind::Color; return result; }
			const Map<String, Color> colors{
				{"transparent", Color(0, 0, 0, 0)}, {"black", Color(0, 0, 0, 1)},
				{"white", Color(1, 1, 1, 1)}, {"red", Color(1, 0, 0, 1)},
				{"green", Color(0, 1, 0, 1)}, {"blue", Color(0, 0, 1, 1)}
			};
			if (const auto found = colors.find(folded); found != colors.end())
			{
				result.kind = UiStyleValueKind::Color;
				result.color = found->second;
				return result;
			}
			if (value.size() >= 2 && ((value.front() == '"' && value.back() == '"') ||
				(value.front() == '\'' && value.back() == '\'')))
			{
				result.kind = UiStyleValueKind::String;
				result.text = value.substr(1, value.size() - 2);
				return result;
			}
			Char* end = nullptr;
			result.number = std::strtod(value.c_str(), &end);
			if (end != value.c_str())
			{
				const String unit = lower(String(end));
				const Map<String, UiStyleUnit> units{
					{"", UiStyleUnit::None}, {"px", UiStyleUnit::Px}, {"dp", UiStyleUnit::Dp},
					{"%", UiStyleUnit::Percent}, {"em", UiStyleUnit::Em}, {"rem", UiStyleUnit::Rem},
					{"vw", UiStyleUnit::Vw}, {"vh", UiStyleUnit::Vh}, {"fr", UiStyleUnit::Fr}
				};
				if (const auto found = units.find(unit); found != units.end())
				{
					result.kind = UiStyleValueKind::Number;
					result.unit = found->second;
					return result;
				}
			}
			const SizeT open = folded.find('(');
			if (open != String::npos && value.back() == ')' &&
				(folded.starts_with("calc(") || folded.starts_with("min(") ||
				 folded.starts_with("max(") || folded.starts_with("clamp(")))
			{
				result.kind = UiStyleValueKind::Expression;
				String token;
				for (const Char c : value)
				{
					if (std::isspace(static_cast<unsigned char>(c)) || c == ',' || c == '(' || c == ')' ||
						c == '+' || c == '-' || c == '*' || c == '/')
					{
						if (!token.empty()) { result.tokens.push_back(token); token.clear(); }
						if (!std::isspace(static_cast<unsigned char>(c))) result.tokens.push_back(String(1, c));
					}
					else token += c;
				}
				if (!token.empty()) result.tokens.push_back(token);
				return result;
			}
			if (!value.empty() && identifierStart(value[0]) &&
				std::ranges::all_of(value, identifierChar))
			{
				result.kind = UiStyleValueKind::Identifier;
				return result;
			}
			result.kind = UiStyleValueKind::Expression;
			result.tokens = splitTopLevel(value, ' ');
			return result;
		}

		UInt pseudoBit(const String& name)
		{
			if (name == "hover") return static_cast<UInt>(UiStylePseudoState::Hover);
			if (name == "active") return static_cast<UInt>(UiStylePseudoState::Active);
			if (name == "focus") return static_cast<UInt>(UiStylePseudoState::Focus);
			if (name == "disabled") return static_cast<UInt>(UiStylePseudoState::Disabled);
			if (name == "checked") return static_cast<UInt>(UiStylePseudoState::Checked);
			if (name == "selected") return static_cast<UInt>(UiStylePseudoState::Selected);
			return 0;
		}

		Bool parseSelector(const String& text, UiStyleSelector& selector, String& error)
		{
			selector.text = trim(text);
			if (selector.text.empty() || containsForbidden(selector.text)) { error = "Empty or forbidden selector."; return false; }
			SizeT i = 0;
			Bool pendingSpace = false;
			while (i < selector.text.size())
			{
				while (i < selector.text.size() && std::isspace(static_cast<unsigned char>(selector.text[i]))) { pendingSpace = true; ++i; }
				if (i >= selector.text.size()) break;
				UiStyleCombinator combinator = selector.parts.empty() ? UiStyleCombinator::None :
					(pendingSpace ? UiStyleCombinator::Descendant : UiStyleCombinator::None);
				if (selector.text[i] == '>')
				{
					if (selector.parts.empty()) { error = "Selector cannot start with '>'."; return false; }
					combinator = UiStyleCombinator::Child;
					++i;
					while (i < selector.text.size() && std::isspace(static_cast<unsigned char>(selector.text[i]))) ++i;
				}
				UiStyleSelectorPart part;
				part.combinatorToLeft = combinator;
				Bool consumed = false;
				while (i < selector.text.size() && !std::isspace(static_cast<unsigned char>(selector.text[i])) && selector.text[i] != '>')
				{
					Char prefix = 0;
					if (selector.text[i] == '*' ) { consumed = true; ++i; continue; }
					if (selector.text[i] == '#' || selector.text[i] == '.' || selector.text[i] == ':') prefix = selector.text[i++];
					if (i >= selector.text.size() || !identifierStart(selector.text[i])) { error = "Unsupported selector construct."; return false; }
					const SizeT start = i++;
					while (i < selector.text.size() && identifierChar(selector.text[i])) ++i;
					const String name = selector.text.substr(start, i - start);
					if (prefix == '#') { if (!part.id.empty()) { error = "Multiple ids in selector part."; return false; } part.id = name; selector.specificity += 100; }
					else if (prefix == '.') { part.classes.push_back(name); selector.specificity += 10; }
					else if (prefix == ':')
					{
						if (name == "scope" && i < selector.text.size() && selector.text[i] == '(')
						{
							const SizeT close = selector.text.find(')', ++i);
							if (close == String::npos) { error = "Unclosed component scope marker."; return false; }
							part.componentScope = trim(selector.text.substr(i, close - i));
							if (part.componentScope.empty() || !std::ranges::all_of(part.componentScope, identifierChar))
							{ error = "Invalid component scope marker."; return false; }
							i = close + 1;
							selector.specificity += 10;
						}
						else
						{
							const UInt bit = pseudoBit(lower(name));
							if (!bit) { error = "Unsupported pseudo state ':" + name + "'."; return false; }
							part.pseudoStates |= bit;
							selector.specificity += 10;
						}
					}
					else { if (!part.type.empty()) { error = "Multiple types in selector part."; return false; } part.type = name; ++selector.specificity; }
					consumed = true;
				}
				if (!consumed) { error = "Empty selector part."; return false; }
				selector.parts.push_back(std::move(part));
				pendingSpace = false;
			}
			return !selector.parts.empty();
		}

		Bool hasClass(const UiStyleNodeDescriptor& node, const String& name)
		{
			return std::ranges::find(node.classes, name) != node.classes.end();
		}

		Bool matchesPart(const UiStyleSelectorPart& part, const UiStyleNodeDescriptor& node)
		{
			if (!part.type.empty() && part.type != node.type) return false;
			if (!part.id.empty() && part.id != node.id) return false;
			if (!part.componentScope.empty() && part.componentScope != node.componentScope) return false;
			if ((node.pseudoStates & part.pseudoStates) != part.pseudoStates) return false;
			for (const String& className : part.classes) if (!hasClass(node, className)) return false;
			return true;
		}

		Bool matchFromRight(const UiStyleSelector& selector, const SizeT partIndex, const UiStyleNodeDescriptor* node)
		{
			if (!node || !matchesPart(selector.parts[partIndex], *node)) return false;
			if (partIndex == 0) return true;
			const UiStyleCombinator combinator = selector.parts[partIndex].combinatorToLeft;
			if (combinator == UiStyleCombinator::Child)
				return matchFromRight(selector, partIndex - 1, node->parent);
			for (const UiStyleNodeDescriptor* ancestor = node->parent; ancestor; ancestor = ancestor->parent)
				if (matchFromRight(selector, partIndex - 1, ancestor)) return true;
			return false;
		}

		struct Mixin
		{
			Vector<String> parameters;
			String body;
			String sourceName;
			SourceSpan span;
		};

		struct CompileContext
		{
			DiagnosticSink diagnostics;
			UiCompiledStyleSheet sheet;
			Map<String, String> variables;
			Map<String, Mixin> mixins;
			Vector<String> importStack;
			const UiStyleCompileOptions& options;
			UInt sourceOrder = 0;
			SizeT importedBytes = 0;
			SizeT tokenCount = 0;
		};

		class Parser
		{
		public:
			Parser(CompileContext& newContext, const String& newSource, String newName, const SizeT newImportDepth)
				: context(newContext), source(newSource), sourceName(std::move(newName)), importDepth(newImportDepth) {}

			void run(const Vector<String>& selectors = {}) { parseBlock(selectors, 0, false); }

		private:
			CompileContext& context;
			const String& source;
			String sourceName;
			SizeT importDepth = 0;
			SizeT cursor = 0;

			SourceSpan span(const SizeT offset, const SizeT length) const
			{
				SourceSpan result{ sourceName, offset, length, 1, 1 };
				for (SizeT i = 0; i < offset && i < source.size(); ++i)
					if (source[i] == '\n') { ++result.line; result.column = 1; } else ++result.column;
				return result;
			}

			void skipTrivia()
			{
				while (cursor < source.size())
				{
					if (std::isspace(static_cast<unsigned char>(source[cursor]))) { ++cursor; continue; }
					if (source.compare(cursor, 2, "//") == 0)
					{
						const SizeT end = source.find('\n', cursor + 2);
						cursor = end == String::npos ? source.size() : end + 1;
						continue;
					}
					if (source.compare(cursor, 2, "/*") == 0)
					{
						const SizeT end = source.find("*/", cursor + 2);
						if (end == String::npos) { context.diagnostics.error("Unterminated style comment.", span(cursor, source.size() - cursor)); cursor = source.size(); }
						else cursor = end + 2;
						continue;
					}
					break;
				}
			}

			String substitute(String value, const SourceSpan& at, const Map<String, String>* locals = nullptr)
			{
				SizeT position = 0;
				for (SizeT replacements = 0; replacements < UiStyleCompiler::maximumVariables &&
					(position = value.find('$', position)) != String::npos; ++replacements)
				{
					SizeT end = position + 1;
					while (end < value.size() && identifierChar(value[end])) ++end;
					const String name = value.substr(position + 1, end - position - 1);
					auto local = locals ? locals->find(name) : Map<String, String>::const_iterator{};
					const Bool hasLocal = locals && local != locals->end();
					const auto global = context.variables.find(name);
					if (name.empty() || (!hasLocal && global == context.variables.end()))
					{
						context.diagnostics.error("Unknown style variable '$" + name + "'.", at);
						position = end;
						continue;
					}
					const String& replacement = hasLocal ? local->second : global->second;
					value.replace(position, end - position, replacement);
					position += replacement.size();
				}
				return value;
			}

			Vector<String> combine(const Vector<String>& parents, const Vector<String>& children, const SourceSpan& at)
			{
				if (parents.empty()) return children;
				Vector<String> result;
				for (const String& parent : parents) for (const String& child : children)
				{
					String combined = child;
					SizeT marker = 0;
					Bool found = false;
					while ((marker = combined.find('&', marker)) != String::npos)
					{
						found = true;
						combined.replace(marker, 1, parent);
						marker += parent.size();
					}
					if (!found) combined = parent + " " + child;
					if (combined.find('&') != String::npos) context.diagnostics.error("Invalid parent selector.", at);
					result.push_back(std::move(combined));
				}
				return result;
			}

			void emit(const Vector<String>& selectors, const Vector<UiStyleDeclaration>& declarations, const SourceSpan& at)
			{
				for (const String& selectorText : selectors)
				{
					if (context.sheet.rules.size() >= UiStyleCompiler::maximumRules)
					{ context.diagnostics.error("Style rule budget exceeded.", at); return; }
					UiStyleSelector parsed;
					String error;
					if (!parseSelector(selectorText, parsed, error))
					{ context.diagnostics.error(error, at); continue; }
					UiStyleRule rule;
					rule.selector = selectorText;
					rule.parsedSelector = std::move(parsed);
					rule.declarations = declarations;
					rule.specificity = rule.parsedSelector.specificity;
					rule.sourceOrder = context.sourceOrder++;
					rule.span = at;
					context.sheet.rules.push_back(std::move(rule));
				}
			}

			String readBalancedBody(const SourceSpan& at)
			{
				const SizeT start = cursor;
				Int depth = 1;
				Char quote = 0;
				while (cursor < source.size() && depth)
				{
					const Char c = source[cursor++];
					if (quote)
					{
						if (c == '\\' && cursor < source.size()) ++cursor;
						else if (c == quote) quote = 0;
					}
					else if (c == '"' || c == '\'') quote = c;
					else if (c == '{') ++depth;
					else if (c == '}') --depth;
				}
				if (depth) { context.diagnostics.error("Unclosed style block.", at); return source.substr(start); }
				return source.substr(start, cursor - start - 1);
			}

			void defineMixin(const String& header, const SourceSpan& at)
			{
				String signature = trim(header.substr(6));
				const SizeT open = signature.find('(');
				const SizeT close = signature.rfind(')');
				const String name = trim(signature.substr(0, open));
				if (name.empty() || open == String::npos || close == String::npos || close < open)
				{ context.diagnostics.error("Mixin requires name and parameter list.", at); readBalancedBody(at); return; }
				if (context.mixins.size() >= UiStyleCompiler::maximumMixins && !context.mixins.contains(name))
				{ context.diagnostics.error("Mixin budget exceeded.", at); readBalancedBody(at); return; }
				Mixin mixin;
				for (String parameter : splitTopLevel(signature.substr(open + 1, close - open - 1), ','))
				{
					parameter = trim(parameter);
					if (parameter.empty()) continue;
					if (parameter[0] != '$' || parameter.size() == 1 ||
						!std::ranges::all_of(parameter.substr(1), identifierChar))
					{ context.diagnostics.error("Invalid mixin parameter.", at); }
					else mixin.parameters.push_back(parameter.substr(1));
				}
				if (mixin.parameters.size() > UiStyleCompiler::maximumMixinParameters)
					context.diagnostics.error("Mixin parameter budget exceeded.", at);
				mixin.body = readBalancedBody(at);
				mixin.sourceName = sourceName;
				mixin.span = at;
				context.mixins[name] = std::move(mixin);
			}

			void includeMixin(const String& statement, const Vector<String>& selectors, const SizeT depth, const SourceSpan& at)
			{
				String call = trim(statement.substr(8));
				const SizeT open = call.find('('), close = call.rfind(')');
				const String name = trim(call.substr(0, open));
				const auto found = context.mixins.find(name);
				if (open == String::npos || close == String::npos || found == context.mixins.end())
				{ context.diagnostics.error("Unknown or malformed mixin include '" + name + "'.", at); return; }
				const Vector<String> arguments = splitTopLevel(call.substr(open + 1, close - open - 1), ',');
				if (arguments.size() != found->second.parameters.size())
				{ context.diagnostics.error("Mixin argument count mismatch.", at); return; }
				Map<String, String> locals;
				for (SizeT i = 0; i < arguments.size(); ++i) locals[found->second.parameters[i]] = substitute(arguments[i], at);
				String expanded = found->second.body;
				for (const auto& [parameter, argument] : locals)
				{
					SizeT position = 0;
					for (SizeT replacements = 0;
						replacements < UiStyleCompiler::maximumVariables &&
						(position = expanded.find("$" + parameter, position)) != String::npos;
						++replacements)
					{
						expanded.replace(position, parameter.size() + 1, argument);
						position += argument.size();
					}
				}
				Parser nested(context, expanded, found->second.sourceName, importDepth);
				nested.parseBlock(selectors, depth + 1, false);
			}

			void import(const String& statement, const SourceSpan& at)
			{
				String requested = trim(statement.substr(7));
				if (requested.size() >= 2 && (requested.front() == '"' || requested.front() == '\'') && requested.back() == requested.front())
					requested = requested.substr(1, requested.size() - 2);
				else { context.diagnostics.error("Import path must be a quoted resolver key.", at); return; }
				if (!context.options.importResolver)
				{ context.diagnostics.error("Imports require an approved resolver.", at); return; }
				if (importDepth >= context.options.maximumImportDepth)
				{ context.diagnostics.error("Import depth budget exceeded.", at); return; }
				String importedSource, importedName;
				if (!context.options.importResolver(requested, sourceName, importedSource, importedName))
				{ context.diagnostics.error("Import was rejected by resolver: '" + requested + "'.", at); return; }
				if (importedName.empty()) importedName = requested;
				if (std::ranges::find(context.importStack, importedName) != context.importStack.end())
				{ context.diagnostics.error("Style import cycle detected at '" + importedName + "'.", at); return; }
				context.importedBytes += importedSource.size();
				if (context.importedBytes > context.options.maximumImportedBytes || importedSource.size() > UiStyleCompiler::maximumSourceBytes)
				{ context.diagnostics.error("Imported style source budget exceeded.", at); return; }
				context.importStack.push_back(importedName);
				Parser nested(context, importedSource, importedName, importDepth + 1);
				nested.run();
				context.importStack.pop_back();
			}

			void statement(const String& text, const Vector<String>& selectors,
				Vector<UiStyleDeclaration>& declarations, const SizeT depth, const SourceSpan& at)
			{
				if (text.empty()) return;
				if (++context.tokenCount > UiStyleCompiler::maximumTokens)
				{ context.diagnostics.error("Style token budget exceeded.", at); return; }
				if (containsForbidden(text))
				{ context.diagnostics.error("Forbidden script, URL, native, or legacy construct.", at); return; }
				if (text.starts_with("@import")) { if (!selectors.empty()) context.diagnostics.error("Imports are only allowed at the document root.", at); else import(text, at); return; }
				if (text.starts_with("@include")) { includeMixin(text, selectors, depth, at); return; }
				const SizeT colon = text.find(':');
				if (colon == String::npos) { context.diagnostics.error("Style declaration requires ':'.", at); return; }
				const String property = trim(text.substr(0, colon));
				String value = substitute(trim(text.substr(colon + 1)), at);
				if (property.starts_with("$"))
				{
					const String name = property.substr(1);
					if (name.empty() || !std::ranges::all_of(name, identifierChar))
					{ context.diagnostics.error("Invalid variable name.", at); return; }
					if (context.variables.size() >= UiStyleCompiler::maximumVariables && !context.variables.contains(name))
					{ context.diagnostics.error("Style variable budget exceeded.", at); return; }
					context.variables[name] = std::move(value);
					return;
				}
				if (selectors.empty()) { context.diagnostics.error("Declaration must be inside a selector.", at); return; }
				if (property.empty() || (!property.starts_with("--") && !identifierStart(property[0])) ||
					!std::ranges::all_of(property, identifierChar))
				{ context.diagnostics.error("Invalid style property.", at); return; }
				if (declarations.size() >= UiStyleCompiler::maximumDeclarationsPerRule)
				{ context.diagnostics.error("Declaration budget exceeded.", at); return; }
				declarations.push_back({ property, value, parseValue(value), at });
			}

			void parseBlock(const Vector<String>& selectors, const SizeT depth, const Bool expectsClose)
			{
				if (depth > UiStyleCompiler::maximumDepth)
				{ context.diagnostics.error("Style nesting budget exceeded.", span(cursor, 0)); return; }
				Vector<UiStyleDeclaration> declarations;
				const SizeT blockStart = cursor;
				Bool closed = !expectsClose;
				auto flush = [&]()
					{
						if (!declarations.empty()) { emit(selectors, declarations, span(blockStart, cursor - blockStart)); declarations.clear(); }
					};
				while (cursor < source.size())
				{
					skipTrivia();
					if (cursor >= source.size()) break;
					if (source[cursor] == '}')
					{
						++cursor;
						if (!expectsClose) context.diagnostics.error("Unexpected '}' in style source.", span(cursor - 1, 1));
						else { closed = true; break; }
						continue;
					}
					const SizeT start = cursor;
					Char quote = 0;
					Int parentheses = 0;
					while (cursor < source.size())
					{
						const Char c = source[cursor];
						if (quote)
						{
							if (c == '\\') { cursor += std::min(SizeT(2), source.size() - cursor); continue; }
							if (c == quote) quote = 0;
						}
						else if (c == '"' || c == '\'') quote = c;
						else if (c == '(') ++parentheses;
						else if (c == ')') { if (--parentheses < 0) context.diagnostics.error("Unexpected ')'.", span(cursor, 1)); }
						else if (parentheses == 0 && (c == '{' || c == ';' || c == '}')) break;
						++cursor;
					}
					const String header = trim(source.substr(start, cursor - start));
					if (cursor >= source.size())
					{ if (!header.empty()) context.diagnostics.error("Expected ';' or '{'.", span(start, cursor - start)); break; }
					const Char delimiter = source[cursor++];
					const SourceSpan at = span(start, cursor - start);
					if (delimiter == ';')
					{
						if (header.starts_with("@include") || header.starts_with("@import")) flush();
						statement(header, selectors, declarations, depth, at);
						continue;
					}
					if (delimiter == '}')
					{
						context.diagnostics.error("Style statement is missing ';'.", at);
						if (expectsClose) closed = true;
						break;
					}
					flush();
					if (header.starts_with("@mixin"))
					{
						if (!selectors.empty()) context.diagnostics.error("Mixins must be declared at document root.", at);
						defineMixin(header, at);
						continue;
					}
					if (header.starts_with("@"))
					{
						context.diagnostics.error("Unsupported style at-rule.", at);
						readBalancedBody(at);
						continue;
					}
					const Vector<String> children = splitTopLevel(substitute(header, at), ',');
					parseBlock(combine(selectors, children, at), depth + 1, true);
				}
				flush();
				if (!closed) context.diagnostics.error("Unclosed style block.", span(blockStart, cursor - blockStart));
			}
		};

		void addIndex(Vector<UInt>& values, const UInt index)
		{
			if (std::ranges::find(values, index) == values.end()) values.push_back(index);
		}

		Vector<String> nodeKeys(const UiStyleNodeDescriptor& node)
		{
			Vector<String> keys;
			if (!node.id.empty()) keys.push_back("#" + node.id);
			if (!node.type.empty()) keys.push_back(node.type);
			for (const String& item : node.classes) keys.push_back("." + item);
			if (!node.componentScope.empty()) keys.push_back(":scope(" + node.componentScope + ")");
			return keys;
		}

		String resolveCustomValue(String value, const Map<String, String>& custom, SizeT depth = 0)
		{
			if (depth > 16) return value;
			SizeT position = 0;
			while ((position = value.find("var(", position)) != String::npos)
			{
				const SizeT close = value.find(')', position + 4);
				if (close == String::npos) break;
				const Vector<String> arguments = splitTopLevel(value.substr(position + 4, close - position - 4), ',');
				const auto found = custom.find(trim(arguments[0]));
				const String replacement = found != custom.end() ? found->second : (arguments.size() > 1 ? arguments[1] : "");
				value.replace(position, close - position + 1, resolveCustomValue(replacement, custom, depth + 1));
			}
			return value;
		}
	}

	Bool UiCompiledStyleSheet::matches(const UiStyleRule& rule, const UiStyleNodeDescriptor& node) const
	{
		return !rule.parsedSelector.parts.empty() &&
			matchFromRight(rule.parsedSelector, rule.parsedSelector.parts.size() - 1, &node);
	}

	Vector<UInt> UiCompiledStyleSheet::matchingRules(const UiStyleNodeDescriptor& node) const
	{
		Vector<UInt> candidates = universalRuleIndex;
		for (const String& key : nodeKeys(node))
			if (const auto found = rightmostSelectorIndex.find(key); found != rightmostSelectorIndex.end())
				for (const UInt index : found->second) addIndex(candidates, index);
		std::erase_if(candidates, [&](const UInt index) { return index >= rules.size() || !matches(rules[index], node); });
		return candidates;
	}

	const UiStyleDeclaration* UiResolvedStyle::find(const String& property) const
	{
		const auto found = declarations.find(property);
		return found == declarations.end() ? nullptr : &found->second;
	}

	UiStyleCompileResult UiStyleCompiler::compile(const String& source, const String& sourceName)
	{
		return compile(source, sourceName, {});
	}

	UiStyleCompileResult UiStyleCompiler::compile(
		const String& source, const String& sourceName, const UiStyleCompileOptions& options)
	{
		UiStyleCompileResult result;
		CompileContext context{ {}, {}, {}, {}, {}, options };
		if (source.size() > maximumSourceBytes)
			context.diagnostics.error("Style source budget exceeded.", { sourceName, 0, source.size(), 1, 1 });
		else if (containsForbidden(source))
			context.diagnostics.error("Forbidden script, URL, native, or legacy construct.", { sourceName, 0, source.size(), 1, 1 });
		else
		{
			context.importStack.push_back(sourceName);
			Parser parser(context, source, sourceName, 0);
			parser.run();
		}
		for (UInt i = 0; i < context.sheet.rules.size(); ++i)
		{
			const UiStyleSelectorPart& right = context.sheet.rules[i].parsedSelector.parts.back();
			Bool indexed = false;
			if (!right.id.empty()) { addIndex(context.sheet.rightmostSelectorIndex["#" + right.id], i); indexed = true; }
			if (!right.type.empty()) { addIndex(context.sheet.rightmostSelectorIndex[right.type], i); indexed = true; }
			for (const String& className : right.classes)
			{ addIndex(context.sheet.rightmostSelectorIndex["." + className], i); indexed = true; }
			if (!right.componentScope.empty())
			{ addIndex(context.sheet.rightmostSelectorIndex[":scope(" + right.componentScope + ")"], i); indexed = true; }
			if (!indexed) context.sheet.universalRuleIndex.push_back(i);
		}
		result.sheet = std::move(context.sheet);
		result.valid = !context.diagnostics.hasErrors();
		result.diagnostics = std::move(context.diagnostics.items);
		return result;
	}

	UiResolvedStyle UiStyleCompiler::resolve(
		const UiCompiledStyleSheet& sheet, const UiStyleNodeDescriptor& node, const UiStyleCascadeOptions& options)
	{
		UiResolvedStyle result;
		if (options.parent) result.customProperties = options.parent->customProperties;
		Vector<UInt> matches = sheet.matchingRules(node);
		std::ranges::sort(matches, [&](const UInt a, const UInt b)
			{
				const UiStyleRule& left = sheet.rules[a];
				const UiStyleRule& right = sheet.rules[b];
				return left.specificity == right.specificity ? left.sourceOrder < right.sourceOrder : left.specificity < right.specificity;
			});
		for (const UInt index : matches)
			for (const UiStyleDeclaration& declaration : sheet.rules[index].declarations)
				if (declaration.property.starts_with("--")) result.customProperties[declaration.property] = declaration.value;
				else result.declarations[declaration.property] = declaration;
		if (options.parent && options.inherits)
			for (const auto& [property, declaration] : options.parent->declarations)
				if (options.inherits(property) && !result.declarations.contains(property))
					result.declarations[property] = declaration;
		for (auto& [property, declaration] : result.declarations)
		{
			declaration.value = resolveCustomValue(declaration.value, result.customProperties);
			declaration.typedValue = parseValue(declaration.value);
		}
		return result;
	}
}
