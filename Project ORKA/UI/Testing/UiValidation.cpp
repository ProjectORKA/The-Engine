#include "UiValidation.hpp"
#include "../Inspector/UiLayoutDebugOverlay.hpp"
#include "../Layout/UiOverflowController.hpp"
#include "../Render/UiDisplayList.hpp"
#include "../Render/UiRenderer.hpp"
#include "../Theme/UiAppFlex.hpp"
#include "../Theme/UiAppTheme.hpp"
#include "../Text/UiSvgPathParser.hpp"
#include "../Text/UiSvgRasterizer.hpp"

#include <chrono>
#include <cmath>
#include <exception>
#include <memory>

namespace Ui::Testing
{
	namespace
	{
		using Clock = std::chrono::steady_clock;

		struct DeterministicRandom
		{
			U64 state;

			U64 next()
			{
				state ^= state >> 12;
				state ^= state << 25;
				state ^= state >> 27;
				return state * 2685821657736338717ULL;
			}

			UInt range(const UInt count)
			{
				return count ? static_cast<UInt>(next() % count) : 0;
			}
		};

		struct CaseContext
		{
			UiValidationCaseResult result;
			const UiValidationConfig& config;
			Clock::time_point started = Clock::now();

			Bool consume(const U64 count = 1)
			{
				result.operations += count;
				if (result.operations > config.maximumOperationsPerCase)
				{
					result.status = UiValidationStatus::BudgetExceeded;
					result.notes.push_back("Deterministic operation budget exceeded.");
					return false;
				}
				const U64 elapsed = static_cast<U64>(
					std::chrono::duration_cast<std::chrono::microseconds>(
						Clock::now() - started).count());
				if (elapsed > config.maximumMicrosecondsPerCase)
				{
					result.status = UiValidationStatus::BudgetExceeded;
					result.notes.push_back("Wall-clock safety budget exceeded.");
					return false;
				}
				return true;
			}

			void check(const Bool condition, String name,
				String expected = "true", String actual = "false")
			{
				if (!consume() || condition) return;
				result.status = UiValidationStatus::Failed;
				result.failures.push_back({
					std::move(name), std::move(expected), std::move(actual) });
			}
		};

		using CaseBody = Function<void(CaseContext&)>;

		UiValidationCaseResult runCase(const String& name, const U64 seed,
			const UiValidationConfig& config, const CaseBody& body)
		{
			CaseContext context{ { name, UiValidationStatus::Passed, seed }, config };
			try
			{
				body(context);
			}
			catch (const std::exception& error)
			{
				context.result.status = UiValidationStatus::Failed;
				context.result.failures.push_back({
					"Unhandled standard exception", "no exception", error.what() });
			}
			catch (...)
			{
				context.result.status = UiValidationStatus::Failed;
				context.result.failures.push_back({
					"Unhandled exception", "no exception", "unknown exception" });
			}
			context.result.elapsedMicroseconds = static_cast<U64>(
				std::chrono::duration_cast<std::chrono::microseconds>(
					Clock::now() - context.started).count());
			return context.result;
		}

		void handlesAndDeferredMutation(CaseContext& context)
		{
			UiDocument document;
			const UiNodeHandle root = document.root();
			const UiNodeHandle old = document.createNode(root, { .typeName = "old" });
			context.check(old.isValid(), "create initial node");
			context.check(document.destroyNode(old), "destroy initial node");
			const UiNodeHandle replacement =
				document.createNode(root, { .typeName = "replacement" });
			context.check(replacement.index == old.index, "free slot reused");
			context.check(replacement.generation != old.generation,
				"generation changes after reuse");
			context.check(!document.isValid(old), "stale handle rejected");

			const UiNodeHandle parent =
				document.createNode(root, { .typeName = "parent" });
			const SizeT before = document.nodeCount();
			Bool immediateWasRejected = false;
			Bool createDeferred = false;
			Bool destroyDeferred = false;
			document.traverse([&](const UiNodeHandle handle, UiNode&)
				{
					if (handle != parent) return;
					immediateWasRejected = !document.createNode(parent).isValid();
					createDeferred = document.deferCreate(parent, { .typeName = "deferred" });
					destroyDeferred = document.deferDestroy(replacement);
				});
			context.check(immediateWasRejected, "immediate traversal mutation rejected");
			context.check(createDeferred && destroyDeferred, "deferred operations accepted");
			context.check(document.nodeCount() == before, "deferred operations wait");
			document.applyDeferredMutations();
			context.check(document.nodeCount() == before, "deferred create/destroy balance");
			context.check(!document.isValid(replacement), "deferred destroy applied");
			context.check(document.get(parent)->childHandles().size() == 1,
				"deferred child created");
		}

		void expressionSandbox(CaseContext& context)
		{
			UiPropertyRegistry properties;
			context.check(properties.set("player.health", UiValue::fromNumber(7)),
				"register expression property");
			context.check(properties.set("player.name", UiValue::fromString("Orka")),
				"register string property");
			const UiExpressionCompileResult compiled =
				UiExpressionCompiler::compile(
					"player.health * 3 + 1 == 22 && player.name == \"Orka\"",
					"validation-expression");
			context.check(compiled.valid, "compile sandbox expression");
			const UiExpressionResult evaluated =
				UiExpressionVm::evaluate(compiled.program, properties, 128);
			context.check(evaluated.valid && evaluated.value.truthy(),
				"evaluate sandbox expression");
			const UiExpressionResult exhausted =
				UiExpressionVm::evaluate(compiled.program, properties, 1);
			context.check(!exhausted.valid, "instruction budget enforced");
			const UiExpressionCompileResult forbidden =
				UiExpressionCompiler::compile("system(\"format c:\")", "untrusted");
			context.check(!forbidden.valid, "calls excluded from grammar");
			const UiExpressionCompileResult oversized =
				UiExpressionCompiler::compile(
					String(UiExpressionCompiler::maximumSourceBytes + 1, 'x'), "oversized");
			context.check(!oversized.valid, "source byte budget enforced");
		}

		void malformedCorpus(CaseContext& context)
		{
			Vector<String> markup{
				"<panel><button></panel>",
				"<panel style=\"color:red\"></panel>",
				"<panel [innerhtml]=\"user.value\"></panel>",
				"@for (x of rows; track x.id) {",
				"<panel><",
				"<!-- unterminated",
				"<panel id=\"a\" id=\"b\"/>",
				"<panel (click)=\"not a command()\"/>",
				"}", // must not stall the parser (regression for malformed-corpus hang)
			};
			Vector<String> style{
				".a { color: ;",
				"#x > > .b { width: 1px; }",
				"@mixin broken( {",
				".a { $missing: ; width: $unknown; }",
				".a { width: calc((((((((1px); }",
				"@import \"unapproved.scss\";",
			};
			DeterministicRandom random{ context.result.seed ^ 0xf00dcafeULL };
			const String alphabet = "<>{}[]()*:#;\"'=/$ abcXYZ019\n";
			for (UInt sample = 0; sample < 48 && context.consume(); ++sample)
			{
				String value;
				const UInt length = 8 + random.range(120);
				value.reserve(length);
				for (UInt i = 0; i < length; ++i)
					value.push_back(alphabet[random.range(static_cast<UInt>(alphabet.size()))]);
				(sample & 1 ? markup : style).push_back(std::move(value));
			}
			for (UInt index = 0; index < markup.size() && context.consume(); ++index)
			{
				UiTemplateBudgets budgets;
				budgets.maximumSourceBytes = 256;
				budgets.maximumNodes = 32;
				budgets.maximumDepth = 8;
				const UiMarkupParseResult result =
					UiMarkupParser::parse(markup[index], "markup-corpus", budgets);
				context.check(result.diagnostics.size() <= DiagnosticSink::maximumDiagnostics,
					"markup diagnostics bounded");
				context.check(result.document.nodes.size() <= budgets.maximumNodes,
					"markup nodes bounded");
			}
			for (const String& source : style)
			{
				if (!context.consume()) break;
				const UiStyleCompileResult result =
					UiStyleCompiler::compile(source, "style-corpus");
				context.check(result.diagnostics.size() <= DiagnosticSink::maximumDiagnostics,
					"style diagnostics bounded");
				context.check(result.sheet.rules.size() <= UiStyleCompiler::maximumRules,
					"style rules bounded");
			}
		}

		void cascadeSpecificity(CaseContext& context)
		{
			const String source =
				"button { color: red; width: 10px; }\n"
				".primary { color: green; }\n"
				"panel > button.primary { width: 20px; }\n"
				"#save { color: blue; }\n"
				"#save:hover { color: white; }\n";
			const UiStyleCompileResult compiled =
				UiStyleCompiler::compile(source, "specificity.scss");
			context.check(compiled.valid, "compile cascade fixture");
			UiStyleNodeDescriptor parent{ .type = "panel" };
			UiStyleNodeDescriptor node{
				.type = "button", .id = "save", .classes = { "primary" },
				.pseudoStates = static_cast<UInt>(UiStylePseudoState::Hover),
				.parent = &parent
			};
			const UiResolvedStyle resolved = UiStyleCompiler::resolve(compiled.sheet, node);
			const UiStyleDeclaration* color = resolved.find("color");
			const UiStyleDeclaration* width = resolved.find("width");
			context.check(color && color->value == "white",
				"id plus pseudo wins cascade", "white", color ? color->value : "missing");
			context.check(width && width->value == "20px",
				"child selector wins type", "20px", width ? width->value : "missing");
		}

		void layoutConstraints(CaseContext& context)
		{
			UiDocument document;
			UiLayoutEngine layout(document);
			const UiNodeHandle root = document.root();
			const UiNodeHandle flex = document.createNode(root, { .typeName = "flex" });
			const UiNodeHandle flexA = document.createNode(flex, { .typeName = "a" });
			const UiNodeHandle flexB = document.createNode(flex, { .typeName = "b" });
			const UiNodeHandle grid = document.createNode(root, { .typeName = "grid" });
			const UiNodeHandle gridA = document.createNode(grid, { .typeName = "a" });
			const UiNodeHandle gridB = document.createNode(grid, { .typeName = "b" });
			const UiNodeHandle overlay = document.createNode(root, { .typeName = "overlay" });
			const UiNodeHandle overlayA = document.createNode(overlay, { .typeName = "a" });

			UiLayoutStyle rootStyle;
			rootStyle.flexDirection = UiAxis::Column;
			layout.setStyle(root, rootStyle);
			UiLayoutStyle flexStyle;
			flexStyle.height = UiSizeSpec::fixed(80);
			flexStyle.gap = Vec2(10, 0);
			layout.setStyle(flex, flexStyle);
			UiLayoutStyle childStyle;
			childStyle.width = UiSizeSpec::fixed(40);
			layout.setStyle(flexA, childStyle);
			childStyle.flexGrow = 1;
			layout.setStyle(flexB, childStyle);

			UiLayoutStyle gridStyle;
			gridStyle.display = UiDisplay::Grid;
			gridStyle.height = UiSizeSpec::fixed(80);
			gridStyle.gridColumns = {
				{ UiSizeSpec::fixed(50) }, { UiSizeSpec::fr(1) }
			};
			layout.setStyle(grid, gridStyle);
			layout.setStyle(gridA, {});
			UiLayoutStyle secondGrid;
			secondGrid.gridColumn = 1;
			layout.setStyle(gridB, secondGrid);

			UiLayoutStyle overlayStyle;
			overlayStyle.display = UiDisplay::Overlay;
			overlayStyle.height = UiSizeSpec::fixed(40);
			layout.setStyle(overlay, overlayStyle);
			UiLayoutStyle overlayChild;
			overlayChild.width = UiSizeSpec::percent(0.5f);
			overlayChild.height = UiSizeSpec::percent(1);
			layout.setStyle(overlayA, overlayChild);

			layout.layout(UiConstraints::tight(Vec2(300, 200)));
			const UiNode* flexNode = document.get(flex);
			const UiNode* flexBNode = document.get(flexB);
			const UiNode* gridANode = document.get(gridA);
			const UiNode* gridBNode = document.get(gridB);
			const UiNode* overlayNode = document.get(overlayA);
			context.check(flexNode && flexNode->computed.size.x <= 300.01f,
				"flex respects root width");
			context.check(flexBNode && flexBNode->computed.size.x >= 40,
				"flex growth preserves minimum basis");
			context.check(gridANode && gridBNode &&
				gridBNode->computed.position.x >= gridANode->computed.position.x + 49,
				"grid columns placed independently");
			context.check(overlayNode && std::abs(overlayNode->computed.size.x - 150) < 0.1f,
				"overlay percentage constrained");
			context.check(layout.diagnostics().empty(), "valid layout has no diagnostics");
		}

		void textUtf8WrapFit(CaseContext& context)
		{
			UiTextPrepareOptions prepare;
			prepare.glyphAdvance = [](U32 codepoint)
				{ return codepoint == ' ' ? 300.0f : 600.0f; };
			const UiPreparedText text =
				UiPreparedText::prepareUtf8("Hello \xf0\x9f\x8c\x8d UTF-8 wrap test", prepare);
			context.check(!text.hadMalformedUtf8(), "valid UTF-8 accepted");
			const UiTextLineLayout lineLayout(text);
			UiTextLayoutOptions options;
			options.fontSize = 16;
			const UiTextLayoutResult wrapped = lineLayout.layout(55, options);
			context.check(wrapped.lines.size() > 1, "text wraps deterministically");
			UiTextFitOptions fit;
			fit.mode = UiTextFit::Shrink;
			fit.preferredSize = 30;
			fit.minimumSize = 8;
			const UiTextLayoutResult fitted =
				lineLayout.fit(Vec2(100, 35), options, fit);
			context.check(fitted.size.x <= 100.01f && fitted.size.y <= 35.01f,
				"fit respects definite box");
			const UiPreparedText malformed =
				UiPreparedText::prepareUtf8(String("\xf0\x28\x8c\x28", 4));
			context.check(malformed.hadMalformedUtf8(), "malformed UTF-8 replaced safely");
		}

		void overflowScrollContainer(CaseContext& context)
		{
			UiDocument document;
			UiLayoutEngine layout(document);
			UiEventRouter router(document);
			UiOverflowController overflow(document, layout, router);

			const UiNodeHandle root = document.root();
			const UiNodeHandle scroller = document.createNode(root, { .typeName = "scroller" });
			const UiNodeHandle first = document.createNode(scroller, { .typeName = "item" });
			const UiNodeHandle second = document.createNode(scroller, { .typeName = "item" });
			const UiNodeHandle third = document.createNode(scroller, { .typeName = "item" });

			UiLayoutStyle rootStyle;
			rootStyle.flexDirection = UiAxis::Column;
			rootStyle.height = UiSizeSpec::fixed(200);
			layout.setStyle(root, rootStyle);

			UiLayoutStyle scrollStyle;
			scrollStyle.flexGrow = 1;
			scrollStyle.flexShrink = 1;
			scrollStyle.overflowY = UiOverflow::Auto;
			scrollStyle.gap = Vec2(0, 10);
			layout.setStyle(scroller, scrollStyle);

			const Float itemHeight = 80;
			for (const UiNodeHandle item : { first, second, third })
			{
				UiLayoutStyle itemStyle;
				itemStyle.height = UiSizeSpec::fixed(itemHeight);
				itemStyle.flexShrink = 0;
				layout.setStyle(item, itemStyle);
			}

			layout.layout(UiConstraints::tight(Vec2(200, 200)));
			overflow.synchronizeAfterLayout();
			layout.layout(UiConstraints::tight(Vec2(200, 200)));
			overflow.synchronizeAfterLayout();

			const UiNode* scrollNode = document.get(scroller);
			const UiNode* firstNode = document.get(first);
			const UiNode* secondNode = document.get(second);
			const UiNode* thirdNode = document.get(third);
			context.check(scrollNode && firstNode && secondNode && thirdNode,
				"overflow scroll fixture laid out");
			if (!scrollNode || !firstNode || !secondNode || !thirdNode) return;

			const Float expectedContentHeight = itemHeight * 3 + 10 * 2;
			const Float firstYBeforeScroll = firstNode->computed.position.y;
			context.check(std::abs(firstNode->computed.size.y - itemHeight) < 0.1f,
				"scroll child keeps fixed height");
			context.check(std::abs(secondNode->computed.size.y - itemHeight) < 0.1f,
				"second scroll child keeps fixed height");
			context.check(std::abs(thirdNode->computed.size.y - itemHeight) < 0.1f,
				"third scroll child keeps fixed height");
			context.check(scrollNode->computed.scrollExtent.y > scrollNode->computed.contentSize.y + 1,
				"scroll extent exceeds viewport");
			context.check(std::abs(scrollNode->computed.scrollExtent.y - expectedContentHeight) < 1.0f,
				"scroll extent matches stacked child heights");
			context.check(scrollNode->computed.scroll.verticalGutter > 0,
				"overflow scroll reserves vertical gutter");
			context.check(scrollNode->computed.scroll.viewport.x <
				scrollNode->computed.contentSize.x,
				"scroll viewport shrinks for gutter");

			UiScrollState scroll;
			scroll.setMetrics(
				scrollNode->computed.scrollExtent, scrollNode->computed.scroll.viewport);
			context.check(scroll.maximumOffset().y > 0, "overflow scroll container is scrollable");

			const UiScrollConsumption wheelDown = scroll.consumeScroll(Vec2(0, 40));
			context.check(wheelDown.consumed.y > 0, "wheel scroll consumes vertical delta");
			context.check(scroll.offset().y > 0, "wheel scroll advances offset");

			if (UiNode* mutableScroll = document.get(scroller))
				mutableScroll->computed.scroll.offset = scroll.offset();
			document.markDirty(scroller, DirtyFlag::Arrange);
			layout.layout(UiConstraints::tight(Vec2(200, 200)));

			const UiNode* firstAfterScroll = document.get(first);
			context.check(firstAfterScroll &&
				firstAfterScroll->computed.position.y < firstYBeforeScroll - 1,
				"scroll offset shifts content upward");
		}

		void scrollTextLayoutClip(CaseContext& context)
		{
			UiDocument document;
			UiLayoutEngine layout(document);
			UiEventRouter router(document);
			UiOverflowController overflow(document, layout, router);

			const UiNodeHandle root = document.root();
			const UiNodeHandle scroller = document.createNode(root, { .typeName = "scroller" });
			const UiNodeHandle label = document.createNode(scroller, { .typeName = "Text" });

			UiLayoutStyle rootStyle;
			rootStyle.flexDirection = UiAxis::Column;
			rootStyle.height = UiSizeSpec::fixed(120);
			layout.setStyle(root, rootStyle);

			UiLayoutStyle scrollStyle;
			scrollStyle.flexGrow = 1;
			scrollStyle.overflowY = UiOverflow::Auto;
			layout.setStyle(scroller, scrollStyle);

			UiLayoutStyle labelStyle;
			labelStyle.height = UiSizeSpec::fixed(200);
			labelStyle.flexShrink = 0;
			layout.setStyle(label, labelStyle);

			if (UiNode* labelNode = document.get(label))
				labelNode->authored.text = "Scroll clip regression label";

			layout.layout(UiConstraints::tight(Vec2(160, 120)));
			overflow.synchronizeAfterLayout();
			layout.layout(UiConstraints::tight(Vec2(160, 120)));
			overflow.synchronizeAfterLayout();

			const UiNode* scrollNode = document.get(scroller);
			const UiNode* labelNode = document.get(label);
			context.check(scrollNode && labelNode, "scroll text fixture laid out");
			if (!scrollNode || !labelNode) return;

			const UiRect scrollport = scrollportRect(
				{ scrollNode->computed.contentPosition, scrollNode->computed.contentSize },
				scrollNode->computed.scroll.verticalGutter,
				scrollNode->computed.scroll.horizontalGutter);
			context.check(labelNode->computed.clipSize.x > 0 && labelNode->computed.clipSize.y > 0,
				"text descendant receives layout clip inside scroll container");
			context.check(std::abs(labelNode->computed.clipPosition.x - scrollport.position.x) < 0.5f &&
				std::abs(labelNode->computed.clipPosition.y - scrollport.position.y) < 0.5f &&
				std::abs(labelNode->computed.clipSize.x - scrollport.size.x) < 0.5f &&
				std::abs(labelNode->computed.clipSize.y - scrollport.size.y) < 0.5f,
				"text descendant clip matches scrollport");

			UiTextPainter painter({}, {}, {});
			UiDisplayList list;
			UiTextPaintStyle style;
			style.fontSize = 16;
			style.padding = Vec2(0);
			context.check(painter.paint(*labelNode, list, style), "scroll text paints");
			UiRenderPacket packet;
			Vector<UiRenderDiagnostic> diagnostics;
			context.check(UiDisplayListFinalizer::finalize(list, packet, diagnostics),
				"scroll text display list finalizes");
			context.check(!packet.batches().empty(), "scroll text produces draw batches");
			context.check(packet.batches().front().clipState != 0,
				"scroll text glyphs inherit a clip state");
		}

		void layoutDebugOverlay(CaseContext& context)
		{
			UiDocument document;
			UiLayoutEngine layout(document);
			UiEventRouter router(document);
			UiOverflowController overflow(document, layout, router);

			const UiNodeHandle root = document.root();
			const UiNodeHandle row = document.createNode(root, { .typeName = "row" });
			const UiNodeHandle first = document.createNode(row, { .typeName = "item" });
			const UiNodeHandle second = document.createNode(row, { .typeName = "item" });

			UiLayoutStyle rootStyle;
			rootStyle.height = UiSizeSpec::fixed(120);
			layout.setStyle(root, rootStyle);

			UiLayoutStyle rowStyle;
			rowStyle.display = UiDisplay::Flex;
			rowStyle.flexDirection = UiAxis::Row;
			rowStyle.gap = Vec2(12, 0);
			rowStyle.padding = UiEdges{ 8, 8, 8, 8 };
			layout.setStyle(row, rowStyle);

			for (const UiNodeHandle item : { first, second })
			{
				UiLayoutStyle itemStyle;
				itemStyle.width = UiSizeSpec::fixed(48);
				itemStyle.height = UiSizeSpec::fixed(48);
				itemStyle.flexShrink = 0;
				layout.setStyle(item, itemStyle);
			}

			layout.layout(UiConstraints::tight(Vec2(200, 120)));
			overflow.synchronizeAfterLayout();

			const UiNode* rowNode = document.get(row);
			context.check(rowNode != nullptr, "layout debug fixture has flex row target");
			if (!rowNode) return;

			UiEvent move;
			move.type = UiEventType::PointerMove;
			move.position = DVec2(
				rowNode->computed.position.x + rowNode->computed.size.x * 0.5,
				rowNode->computed.position.y + rowNode->computed.size.y * 0.5);
			(void)router.dispatch(move);
			context.check(router.hoveredNode().index == row.index,
				"hovered node accessor matches flex row target");

			UiDisplayList displayList;
			const Inspector::UiLayoutDebugOverlayResult painted =
				Inspector::UiLayoutDebugOverlay::paintHovered(
					document, layout, row, displayList);
			context.check(painted.valid && painted.painted, "layout debug paints flex row with internal gaps");
			context.check(displayList.commands().size() >= 5,
				"layout debug overlay includes internal gap geometry");

			UiRenderPacket packet;
			Vector<UiRenderDiagnostic> diagnostics;
			context.check(UiDisplayListFinalizer::finalize(displayList, packet, diagnostics),
				"layout debug overlay finalizes");

			// Cross-axis center alignment gives siblings different origins on the same line.
			UiLayoutStyle centeredRowStyle = rowStyle;
			centeredRowStyle.alignItems = UiAlign::Center;
			layout.setStyle(row, centeredRowStyle);
			UiLayoutStyle tallItemStyle;
			tallItemStyle.width = UiSizeSpec::fixed(48);
			tallItemStyle.height = UiSizeSpec::fixed(72);
			tallItemStyle.flexShrink = 0;
			layout.setStyle(first, tallItemStyle);
			layout.layout(UiConstraints::tight(Vec2(200, 120)));

			UiDisplayList centeredList;
			const Inspector::UiLayoutDebugOverlayResult centeredPainted =
				Inspector::UiLayoutDebugOverlay::paintHovered(
					document, layout, row, centeredList);
			context.check(centeredPainted.valid && centeredPainted.painted,
				"layout debug paints gaps when cross-axis alignment offsets siblings");
			context.check(centeredList.commands().size() >= 5,
				"center-aligned flex row still includes internal gap geometry");
		}

		void flexMinimumContentSize(CaseContext& context)
		{
			UiDocument document;
			UiLayoutEngine layout(document);
			const UiNodeHandle root = document.root();
			const UiNodeHandle column = document.createNode(root, { .typeName = "column" });
			const UiNodeHandle first = document.createNode(column, { .typeName = "item" });
			const UiNodeHandle second = document.createNode(column, { .typeName = "item" });

			UiLayoutStyle rootStyle;
			rootStyle.flexDirection = UiAxis::Column;
			layout.setStyle(root, rootStyle);

			UiLayoutStyle columnStyle;
			columnStyle.height = UiSizeSpec::fixed(100);
			columnStyle.gap = Vec2(0, 0);
			layout.setStyle(column, columnStyle);

			const UiIntrinsicMeasurer itemMeasurer = [](const UiNode&, const UiConstraints&)
				{
					return UiIntrinsicMeasure{ Vec2(0, 80), Vec2(200, 80), -1 };
				};
			UiLayoutStyle itemStyle;
			itemStyle.height = UiSizeSpec::fixed(80);
			for (const UiNodeHandle item : { first, second })
			{
				layout.setStyle(item, itemStyle);
				layout.setIntrinsicMeasurer(item, itemMeasurer);
			}

			layout.layout(UiConstraints::tight(Vec2(200, 200)));
			const UiNode* firstNode = document.get(first);
			const UiNode* secondNode = document.get(second);
			context.check(firstNode && secondNode, "minimum-size fixture laid out");
			context.check(firstNode && firstNode->computed.size.y >= 79.9f,
				"flex item respects min-content height");
			context.check(secondNode && secondNode->computed.size.y >= 79.9f,
				"second flex item respects min-content height");
		}

		void scrollingAndVirtualList(CaseContext& context)
		{
			context.check(overflowClips(UiOverflow::Hidden, 100, 50), "hidden always clips");
			context.check(!overflowClips(UiOverflow::Visible, 100, 50), "visible never clips");
			context.check(overflowClips(UiOverflow::Auto, 100, 50), "auto clips when overflowing");
			context.check(!overflowClips(UiOverflow::Auto, 50, 100), "auto stays visible when content fits");
			context.check(overflowShowsScrollbar(UiOverflow::Scroll, 50, 100),
				"scroll policy reserves scrollbar track");
			context.check(!overflowShowsScrollbar(UiOverflow::Auto, 50, 100),
				"auto hides scrollbar when content fits");

			const UiScrollGutterReservation gutters = reserveScrollGutters(
				UiOverflow::Auto, UiOverflow::Auto, Vec2(500, 2000), Vec2(200, 300));
			context.check(gutters.vertical > 0 && gutters.viewport.x < 200,
				"scroll gutter shrinks viewport width");
			context.check(gutters.showVertical, "scroll gutter shows vertical bar when overflowing");

			UiScrollState scroll;
			scroll.setMetrics(Vec2(500, 2000), Vec2(200, 300));
			const UiScrollConsumption consumption =
				scroll.consumeScroll(Vec2(700, 2500));
			context.check(scroll.offset() == Vec2(300, 1700), "scroll clamps to extent");
			context.check(consumption.unconsumed.x > 0 && consumption.unconsumed.y > 0,
				"scroll reports chained remainder");

			UiVirtualList list(20);
			context.check(list.initialize(100000, 20), "initialize 100k virtual rows");
			const UiVirtualRange range = list.visibleRange(500000, 400, 40);
			context.check(range.first < range.last && range.last - range.first < 64,
				"virtual range remains bounded");
			const UiScrollAnchor anchor = list.captureAnchor(500000);
			const U64 key = list.keyAt(range.first);
			const UiVirtualMeasurementResult measured =
				list.setMeasuredHeightPreservingAnchor(key, 37, anchor, 500000);
			context.check(measured.accepted && measured.changed,
				"measured row accepted");
			context.check(std::isfinite(measured.correctedScrollOffset),
				"anchor correction finite");
		}

		void focusDismissAndModifiedActivation(CaseContext& context)
		{
			constexpr Int keyEnter = 257;
			constexpr Int modifierAlt = 0x0004;

			UiDocument document;
			const UiNodeHandle root = document.root();
			const UiNodeHandle button = document.createNode(root, { .typeName = "button" });
			document.get(root)->computed.size = Vec2(400, 200);
			document.get(button)->computed.position = Vec2(10, 10);
			document.get(button)->computed.size = Vec2(100, 50);

			UiEventRouter router(document);
			UiInteractionData buttonInteraction;
			buttonInteraction.focusable = true;
			buttonInteraction.hitTestVisible = true;
			buttonInteraction.tabIndex = 0;
			router.setInteraction(button, buttonInteraction);
			router.setInteraction(root, { .hitTestVisible = true });

			Int activations = 0;
			(void)router.addListener(button, UiRoutedEventType::Activate,
				[&](UiRoutedEvent&) { ++activations; });

			UiEvent down;
			down.type = UiEventType::PointerButton;
			down.action = UiEventAction::Pressed;
			down.pointerId = 1;
			down.position = DVec2(20, 20);
			(void)router.dispatch(down);
			context.check(router.focusedNode() == button, "pointer down focuses button");

			down.position = DVec2(300, 150);
			(void)router.dispatch(down);
			context.check(!router.focusedNode().isValid(), "pointer down on empty area clears focus");

			down.position = DVec2(20, 20);
			(void)router.dispatch(down);
			context.check(router.focusedNode() == button, "button can be focused again");

			UiEvent enter;
			enter.type = UiEventType::Key;
			enter.action = UiEventAction::Pressed;
			enter.code = keyEnter;
			(void)router.dispatch(enter);
			context.check(activations == 1, "Enter activates focused control");

			enter.modifiers = modifierAlt;
			(void)router.dispatch(enter);
			context.check(activations == 1, "modified Enter does not activate focused control");
		}

		void secondaryMouseButtonDoesNotActivate(CaseContext& context)
		{
			constexpr Int secondaryMouseButton = 1;

			UiDocument document;
			const UiNodeHandle root = document.root();
			const UiNodeHandle button = document.createNode(root, { .typeName = "button" });
			document.get(root)->computed.size = Vec2(400, 200);
			document.get(button)->computed.position = Vec2(10, 10);
			document.get(button)->computed.size = Vec2(100, 50);

			UiEventRouter router(document);
			UiInteractionData buttonInteraction;
			buttonInteraction.focusable = true;
			buttonInteraction.hitTestVisible = true;
			router.setInteraction(button, buttonInteraction);
			router.setInteraction(root, { .hitTestVisible = true });

			Int activations = 0;
			(void)router.addListener(button, UiRoutedEventType::Activate,
				[&](UiRoutedEvent&) { ++activations; });

			UiEvent down;
			down.type = UiEventType::PointerButton;
			down.action = UiEventAction::Pressed;
			down.pointerId = 1;
			down.position = DVec2(20, 20);
			down.code = secondaryMouseButton;
			(void)router.dispatch(down);

			UiEvent up = down;
			up.action = UiEventAction::Released;
			(void)router.dispatch(up);
			context.check(activations == 0, "secondary mouse button does not activate control");

			down.code = 0;
			(void)router.dispatch(down);
			up.code = 0;
			(void)router.dispatch(up);
			context.check(activations == 1, "primary mouse button activates control");
		}

		void eventPhasesFocusController(CaseContext& context)
		{
			UiDocument document;
			const UiNodeHandle root = document.root();
			const UiNodeHandle panel = document.createNode(root, { .typeName = "panel" });
			const UiNodeHandle left = document.createNode(panel, { .typeName = "button" });
			const UiNodeHandle right = document.createNode(panel, { .typeName = "button" });
			document.get(root)->computed.size = Vec2(400, 200);
			document.get(panel)->computed.size = Vec2(400, 200);
			document.get(left)->computed.position = Vec2(10, 10);
			document.get(left)->computed.size = Vec2(100, 50);
			document.get(right)->computed.position = Vec2(200, 10);
			document.get(right)->computed.size = Vec2(100, 50);

			UiEventRouter router(document);
			UiInteractionData buttonInteraction;
			buttonInteraction.focusable = true;
			buttonInteraction.tabIndex = 0;
			router.setInteraction(left, buttonInteraction);
			router.setInteraction(right, buttonInteraction);
			Vector<String> phases;
			(void)router.addListener(root, UiRoutedEventType::PointerDown,
				[&](UiRoutedEvent& event)
				{ phases.push_back(event.phase == UiEventPhase::Capture ? "capture" : "other"); },
				{ .capture = true });
			(void)router.addListener(left, UiRoutedEventType::PointerDown,
				[&](UiRoutedEvent& event)
				{
					phases.push_back(event.phase == UiEventPhase::Target ? "target" : "other");
					event.focusTarget = true;
				});
			(void)router.addListener(root, UiRoutedEventType::PointerDown,
				[&](UiRoutedEvent& event)
				{ phases.push_back(event.phase == UiEventPhase::Bubble ? "bubble" : "other"); });
			UiEvent down;
			down.type = UiEventType::PointerButton;
			down.action = UiEventAction::Pressed;
			down.position = DVec2(20, 20);
			down.pointerId = 1;
			(void)router.dispatch(down);
			context.check(phases == Vector<String>{ "capture", "target", "bubble" },
				"capture target bubble order");
			context.check(router.focusedNode() == left, "pointer action sets focus");
			context.check(router.moveFocusSpatial(UiNavigationDirection::Right),
				"controller spatial navigation succeeds");
			context.check(router.focusedNode() == right,
				"controller navigation selects right neighbor");
		}

		void textEditing(CaseContext& context)
		{
			UiTextEditLimits limits;
			limits.maximumBytes = 128;
			limits.maximumUndoEntries = 8;
			UiTextEditModel edit(limits);
			context.check(edit.setText("A\xf0\x9f\x99\x82" "B"), "set UTF-8 edit text");
			edit.setSelection(edit.text().size(), edit.text().size());
			context.check(edit.deleteBackward(100), "delete grapheme");
			context.check(edit.text() == "A\xf0\x9f\x99\x82", "cluster deletion is UTF-8 safe");
			context.check(edit.undo(), "undo edit");
			context.check(edit.text() == "A\xf0\x9f\x99\x82" "B", "undo restores value");
			context.check(edit.redo(), "redo edit");
			edit.compositionStart();
			context.check(edit.compositionUpdate("\xe3\x81\x82"), "composition update");
			context.check(edit.isComposing(), "composition state exposed");
			edit.compositionCancel();
			context.check(!edit.isComposing(), "composition cancel clears state");
			context.check(edit.text() == "A\xf0\x9f\x99\x82", "composition cancel restores value");
			context.check(!edit.insert(String("\xff", 1)), "invalid UTF-8 edit rejected");
		}

		Bool validationRasterizer(const UiGlyphRasterRequest& request,
			UiGlyphRasterResult& result)
		{
			result.key = request.key;
			result.format = UiGlyphPixelFormat::R8;
			result.width = 10;
			result.height = 10;
			result.padding = 1;
			result.pixels.assign(100, static_cast<Byte>(request.key.glyphId));
			result.succeeded = true;
			return true;
		}

		void glyphRenderModeAndMetadata(CaseContext& context)
		{
			UiGlyphRenderContext render;
			render.pixelSize = 16;
			context.check(selectUiGlyphRenderMode(render) ==
				UiGlyphRenderMode::HintedGrayscale,
				"small aligned screen text selects grayscale");
			render.pixelSize = 48;
			context.check(selectUiGlyphRenderMode(render) ==
				UiGlyphRenderMode::Msdf,
				"large screen text selects MSDF");
			render.transformed = true;
			context.check(selectUiGlyphRenderMode(render) ==
				UiGlyphRenderMode::Mtsdf,
				"transformed text selects MTSDF");
			render.colorGlyph = true;
			context.check(selectUiGlyphRenderMode(render) ==
				UiGlyphRenderMode::ColorRgba,
				"color glyph overrides distance field");

			const UiGlyphDistanceFieldMetadata grayscale =
				uiGlyphDistanceFieldMetadata(UiGlyphRenderMode::HintedGrayscale, 7);
			const UiGlyphDistanceFieldMetadata msdf =
				uiGlyphDistanceFieldMetadata(UiGlyphRenderMode::Msdf, 7);
			const UiGlyphDistanceFieldMetadata mtsdf =
				uiGlyphDistanceFieldMetadata(UiGlyphRenderMode::Mtsdf, 7);
			context.check(!grayscale.enabled && grayscale.pixelRange == 0,
				"grayscale has no distance metadata");
			context.check(msdf.enabled && !msdf.trueDistanceAlpha &&
				msdf.pixelRange == 7 &&
				uiGlyphPixelFormat(UiGlyphRenderMode::Msdf) ==
					UiGlyphPixelFormat::RGB8,
				"MSDF metadata and format agree");
			context.check(mtsdf.enabled && mtsdf.trueDistanceAlpha &&
				mtsdf.pixelRange == 7 &&
				uiGlyphPixelFormat(UiGlyphRenderMode::Mtsdf) ==
					UiGlyphPixelFormat::MtsdfRGBA8,
				"MTSDF metadata and format agree");
			const UiResourceRef mtsdfResource = makeUiGlyphResource(
				9, UiGlyphPixelFormat::MtsdfRGBA8, 2, 3);
			U32 serviceId = 0;
			UiGlyphPixelFormat format = UiGlyphPixelFormat::R8;
			UInt page = 0;
			context.check(mtsdfResource.pool == UiTexturePool::DistanceFieldGlyphs &&
				decodeUiGlyphResource(mtsdfResource, serviceId, format, page) &&
				serviceId == 9 && format == UiGlyphPixelFormat::MtsdfRGBA8 &&
				page == 2,
				"MTSDF RGBA resource remains distance-field routed");

#ifndef ORKA_UI_ENABLE_ADVANCED_TEXT
			UiGlyphRasterRequest request;
			request.key.glyphId = 65;
			request.key.mode = UiGlyphRenderMode::Msdf;
			UiGlyphRasterResult result;
			context.check(!rasterUiGlyph(request, result) &&
				result.failure == UiGlyphRasterFailure::BackendUnavailable,
				"default build reports unavailable advanced rasterizer");
#endif
		}

		void atlasAllocationEviction(CaseContext& context)
		{
			UiGlyphSkylineAllocator allocator(32, 32);
			UiGlyphRect first;
			UiGlyphRect second;
			context.check(allocator.allocate(16, 16, first), "atlas first allocation");
			context.check(allocator.allocate(16, 16, second), "atlas second allocation");
			context.check(first != second, "atlas allocations do not overlap identically");
			UiGlyphCacheConfig config;
			config.pageWidth = 16;
			config.pageHeight = 16;
			config.maximumR8Pages = 1;
			config.maximumRGB8Pages = 0;
			config.maximumRGBA8Pages = 0;
			config.workerCount = 0;
			config.rasterizer = validationRasterizer;
			UiGlyphCache cache(config);
			Vector<UiGlyphRasterRequest> requests;
			for (UInt glyph = 1; glyph <= 4; ++glyph)
			{
				UiGlyphRasterRequest request;
				request.key.faceId = 1;
				request.key.glyphId = glyph;
				request.padding = 1;
				requests.push_back(request);
			}
			UiGlyphAtlasHandle original;
			(void)cache.warmUp({ requests.front() },
				[](UiGlyphPixelFormat, UInt, UInt, const UiGlyphRect&, const Byte*, SizeT) {});
			context.check(cache.lookup(requests.front().key, original),
				"atlas handle initially resident");
			(void)cache.warmUp(
				Vector<UiGlyphRasterRequest>(requests.begin() + 1, requests.end()),
				[](UiGlyphPixelFormat, UInt, UInt, const UiGlyphRect&, const Byte*, SizeT) {});
			const UiGlyphCacheStats stats = cache.stats();
			context.check(stats.pageEvictions >= 1, "atlas eviction counted");
			context.check(!cache.isHandleCurrent(original), "evicted handle invalidated");
		}

		void glyphResourceTransition(CaseContext& context)
		{
			UiGlyphCacheConfig config;
			config.pageWidth = 32;
			config.pageHeight = 32;
			config.maximumR8Pages = 1;
			config.maximumRGB8Pages = 0;
			config.maximumRGBA8Pages = 0;
			config.workerCount = 1;
			config.rasterizer = validationRasterizer;
			config.missingGlyph.faceId = 7;
			config.missingGlyph.glyphId = 0;
			UiGlyphCache cache(config);
			auto upload = [](UiGlyphPixelFormat, UInt, UInt,
				const UiGlyphRect&, const Byte*, SizeT) {};
			for (UInt attempt = 0; attempt < 10000; ++attempt)
			{
				(void)cache.drainUploads(upload);
				UiGlyphAtlasHandle missing;
				if (cache.lookup(config.missingGlyph, missing)) break;
				std::this_thread::yield();
			}
			UiGlyphAtlasHandle missing;
			context.check(cache.lookup(config.missingGlyph, missing),
				"deterministic missing glyph becomes resident");

			UiGlyphRasterRequest request;
			request.key = config.missingGlyph;
			request.key.glyphId = 65;
			context.check(cache.request(request), "async glyph request accepted");
			const UiGlyphAtlasHandle fallback =
				cache.lookupOrMissing(request.key);
			context.check(fallback.page == missing.page &&
				fallback.pageGeneration == missing.pageGeneration &&
				fallback.rect == missing.rect,
				"pending glyph resolves to resident missing glyph");

			UiGlyphAtlasHandle resident;
			for (UInt attempt = 0; attempt < 10000; ++attempt)
			{
				(void)cache.drainUploads(upload);
				if (cache.lookup(request.key, resident)) break;
				std::this_thread::yield();
			}
			context.check(resident.isValid() && resident.rect != missing.rect,
				"pending glyph transitions to its resident handle");

			const UiResourceRef resource = makeUiGlyphResource(
				23, resident.format, resident.page, resident.pageGeneration);
			U32 serviceId = 0;
			UiGlyphPixelFormat format = UiGlyphPixelFormat::RGBA8;
			UInt page = maxUInt;
			context.check(decodeUiGlyphResource(
				resource, serviceId, format, page) &&
				serviceId == 23 && format == resident.format &&
				page == resident.page,
				"glyph resource format and page decode");
			UiResourceRef stale = resource;
			++stale.generation;
			context.check(stale != resource &&
				stale.generation != resident.pageGeneration,
				"glyph resource generation distinguishes stale handle");
		}

		void displayListValidationBatching(CaseContext& context)
		{
			const UiQuadGeometry quad{ .position = Vec2(0), .size = Vec2(10) };
			UiDisplayList valid;
			for (UInt i = 0; i < 16; ++i)
				context.check(valid.addSolidQuad(quad,
					UiLinearColor::fromStraightLinear(Vec4(1))),
					"append batchable solid quad");
			UiRenderPacket packet;
			Vector<UiRenderDiagnostic> diagnostics;
			context.check(UiDisplayListFinalizer::finalize(valid, packet, diagnostics),
				"valid display list finalizes");
			context.check(packet.batches().size() == 1,
				"compatible draws batch", "1",
				std::to_string(packet.batches().size()));
			context.check(packet.batches()[0].drawCommandCount == 16,
				"batch retains draw count");

			UiDisplayList malformed;
			context.check(malformed.pushClip(UiClipRect{ Vec2(0), Vec2(10) }),
				"push malformed fixture clip");
			UiRenderPacket rejectedPacket;
			Vector<UiRenderDiagnostic> rejectedDiagnostics;
			context.check(!UiDisplayListFinalizer::finalize(
				malformed, rejectedPacket, rejectedDiagnostics),
				"unbalanced display list rejected");
			context.check(!rejectedDiagnostics.empty(), "render diagnostic returned");
		}

		void fallbackTextPainting(CaseContext& context)
		{
			Vector<Byte> atlas;
			UInt atlasWidth = 0;
			UInt atlasHeight = 0;
			UiTextPainter::buildFallbackAtlas(atlas, atlasWidth, atlasHeight);
			context.check(atlasWidth == 128 && atlasHeight == 48,
				"fallback atlas dimensions");
			context.check(std::find(atlas.begin(), atlas.end(), Byte(255)) != atlas.end(),
				"fallback atlas contains glyph coverage");

			UiNode node;
			node.authored.text = "A \nB";
			node.authored.text.push_back(static_cast<Char>(0xff));
			node.authored.text += "\xe2\x98\x83";
			node.computed.position = Vec2(10, 20);
			node.computed.size = Vec2(120, 60);

			const UiResourceRef advancedResource{
				991, UiTexturePool::GrayscaleGlyphs, 7 };
			UiTextPainter painter(
				[advancedResource](const U32 codepoint, const Float pixelSize,
					UiTextGlyphRenderInfo& glyph)
				{
					if (codepoint != 0x2603) return false;
					glyph = UiTextPainter::fallbackGlyph('?', pixelSize);
					glyph.resource = advancedResource;
					return true;
				});
			UiDisplayList list;
			UiTextPaintStyle style;
			style.fontSize = 16;
			context.check(painter.paint(node, list, style), "fallback text paints");
			context.check(list.instances().size() == 4,
				"spaces and newlines advance without glyphs", "4",
				std::to_string(list.instances().size()));
			context.check(list.resources().size() == 2,
				"advanced and fallback resources coexist");
			context.check(list.resources()[0] == builtinFallbackFontResource(),
				"malformed UTF-8 selects replacement fallback");
			context.check(list.resources()[1] == advancedResource,
				"resident advanced provider resource selected");
			context.check(list.instances()[0].colors[0].premultiplied.r <=
				list.instances()[0].colors[0].premultiplied.a,
				"text color is premultiplied");

			UiRenderPacket packet;
			Vector<UiRenderDiagnostic> diagnostics;
			context.check(UiDisplayListFinalizer::finalize(
				list, packet, diagnostics), "fallback text display list finalizes");
			context.check(packet.batches().size() == 1 &&
				packet.batches()[0].pipeline == UiPipeline::GrayscaleGlyph,
				"fallback glyphs remain batched");
		}

		void measuredPaintedTextAgreement(CaseContext& context)
		{
			UiFontService service;
			UiTextPainter painter({}, {}, service.shapeProvider());
			UiNode wrappedNode;
			wrappedNode.authored.text = "AA AA AA";
			wrappedNode.computed.position = Vec2(0);
			UiTextPaintStyle wrappedStyle;
			wrappedStyle.fontSize = 16;
			wrappedStyle.padding = Vec2(0);
			UiTextLayoutResult wrappedLayout;
			const UiIntrinsicMeasure wrappedMeasure = service.measureIntrinsic(
				wrappedNode, UiConstraints::loose(Vec2(40, 200)),
				wrappedStyle, &wrappedLayout);
			wrappedNode.computed.size = wrappedMeasure.preferred;
			wrappedNode.computed.contentSize = wrappedMeasure.preferred;
			UiDisplayList wrappedPaint;
			context.check(painter.paint(wrappedNode, wrappedPaint, wrappedStyle),
				"wrapped shared layout paints");
			Vector<Float> paintedRows;
			for (const UiQuadInstance& instance : wrappedPaint.instances())
				if (std::ranges::find(paintedRows, instance.geometry.position.y) ==
					paintedRows.end())
					paintedRows.push_back(instance.geometry.position.y);
			context.check(paintedRows.size() == wrappedLayout.lines.size(),
				"wrapped measured and painted line counts agree",
				std::to_string(wrappedLayout.lines.size()),
				std::to_string(paintedRows.size()));
			context.check(wrappedLayout.size.y ==
				wrappedLayout.lines.front().height * wrappedLayout.lines.size(),
				"wrapped measured line height is complete");
			context.check(paintedRows.size() < 2 ||
				std::abs((paintedRows[1] - paintedRows[0]) -
					wrappedLayout.lines.front().height) < 0.01f,
				"wrapped painted baselines use measured line height");

			UiNode fittedNode;
			fittedNode.authored.text = "AAAAAA";
			fittedNode.computed.position = Vec2(0);
			fittedNode.computed.size = Vec2(60, 20);
			fittedNode.computed.contentSize = fittedNode.computed.size;
			UiTextPaintStyle fittedStyle;
			fittedStyle.fontSize = 16;
			fittedStyle.padding = Vec2(0);
			fittedStyle.wrap = false;
			fittedStyle.fit.mode = UiTextFit::Shrink;
			fittedStyle.fit.minimumSize = 8;
			fittedStyle.fit.maximumSize = 16;
			UiTextLayoutResult fittedLayout;
			(void)service.measureIntrinsic(fittedNode,
				UiConstraints::tight(fittedNode.computed.size),
				fittedStyle, &fittedLayout);
			UiDisplayList fittedPaint;
			context.check(painter.paint(fittedNode, fittedPaint, fittedStyle),
				"fitted shared layout paints");
			context.check(fittedLayout.fontSize < fittedStyle.fontSize,
				"shrink fit selected smaller measured font");
			context.check(fittedPaint.instances().size() == 6,
				"shrink fit painted every glyph");
			if (fittedPaint.instances().size() >= 2)
				context.check(std::abs(
					(fittedPaint.instances()[1].geometry.position.x -
					 fittedPaint.instances()[0].geometry.position.x) -
					fittedLayout.fontSize * 0.75f) < 0.01f,
					"shrink painted advance matches measured advance");
		}

		void hotReloadRetainsPrevious(CaseContext& context)
		{
			UiBundleManager manager;
			const U64 firstGeneration = manager.requestGeneration();
			UiBundleSources validSources{
				"<ui><label [text]=\"player.name\"/></ui>", "valid.ui.html"
			};
			auto valid = std::make_shared<UiCompiledBundle>(
				UiBundleCompiler::compile(firstGeneration, validSources));
			context.check(valid->valid, "valid reload candidate compiles");
			context.check(manager.publish(valid), "valid reload candidate published");
			const auto previous = manager.activeBundle();
			const U64 secondGeneration = manager.requestGeneration();
			UiBundleSources invalidSources{
				"<ui><label [innerhtml]=\"player.name\"></ui>", "invalid.ui.html"
			};
			auto invalid = std::make_shared<UiCompiledBundle>(
				UiBundleCompiler::compile(secondGeneration, invalidSources));
			context.check(!invalid->valid, "invalid reload candidate rejected by compiler");
			context.check(!manager.publish(invalid), "invalid reload candidate not published");
			context.check(manager.activeBundle() == previous,
				"previous valid bundle remains active");
		}

		void alignClasses(CaseContext& context)
		{
			UiDocument document;
			UiLayoutEngine layout(document);
			const UiNodeHandle root = document.root();
			const UiNodeHandle columnBox = document.createNode(root, { .typeName = "column-box" });
			const UiNodeHandle columnChild = document.createNode(columnBox, { .typeName = "column-child" });
			const UiNodeHandle rowBox = document.createNode(root, { .typeName = "row-box" });
			const UiNodeHandle rowChild = document.createNode(rowBox, { .typeName = "row-child" });

			if (UiNode* columnNode = document.get(columnBox))
				columnNode->authored.classes = { UiAppFlexClass::alignCenter };
			if (UiNode* rowNode = document.get(rowBox))
				rowNode->authored.classes = { UiAppFlexClass::row, UiAppFlexClass::alignTopRight };

			for (const UiNodeHandle box : { columnBox, rowBox })
			{
				UiLayoutStyle boxStyle;
				boxStyle.width = UiSizeSpec::fixed(100);
				boxStyle.height = UiSizeSpec::fixed(100);
				layout.setStyle(box, boxStyle);
			}
			for (const UiNodeHandle child : { columnChild, rowChild })
			{
				UiLayoutStyle childStyle;
				childStyle.width = UiSizeSpec::fixed(20);
				childStyle.height = UiSizeSpec::fixed(20);
				layout.setStyle(child, childStyle);
			}

			layout.layout(UiConstraints::tight(Vec2(220, 120)));
			const UiNode* centeredChild = document.get(columnChild);
			const UiNode* cornerChild = document.get(rowChild);
			context.check(centeredChild && cornerChild, "align fixture laid out");
			context.check(centeredChild && std::abs(centeredChild->computed.position.x - 40) < 0.1f,
				"align-center positions child on cross axis");
			context.check(centeredChild && std::abs(centeredChild->computed.position.y - 40) < 0.1f,
				"align-center positions child on main axis");
			context.check(cornerChild && std::abs(cornerChild->computed.position.x - 80) < 0.1f,
				"row align-top-right positions child on main axis");
			context.check(cornerChild && std::abs(cornerChild->computed.position.y - 0) < 0.1f,
				"row align-top-right positions child on cross axis");
		}

		void flexBasisWrapClasses(CaseContext& context)
		{
			UiDocument document;
			UiLayoutEngine layout(document);
			const UiNodeHandle root = document.root();
			const UiNodeHandle row = document.createNode(root, { .typeName = "row" });
			const UiNodeHandle first = document.createNode(row, { .typeName = "item" });
			const UiNodeHandle second = document.createNode(row, { .typeName = "item" });
			const UiNodeHandle third = document.createNode(row, { .typeName = "item" });

			if (UiNode* rowNode = document.get(row))
				rowNode->authored.classes = {
					UiAppFlexClass::row, UiAppFlexClass::wrap, UiAppFlexClass::g10};
			for (const UiNodeHandle item : { first, second, third })
			{
				if (UiNode* node = document.get(item))
					node->authored.classes = { UiAppFlexClass::b100 };
				UiLayoutStyle itemStyle;
				itemStyle.flexShrink = 0;
				itemStyle.height = UiSizeSpec::fixed(48);
				layout.setStyle(item, itemStyle);
			}

			layout.layout(UiConstraints::tight(Vec2(250, 200)));
			const UiNode* firstNode = document.get(first);
			const UiNode* secondNode = document.get(second);
			const UiNode* thirdNode = document.get(third);
			context.check(layout.style(first).flexBasis == UiSizeSpec::fixed(100),
				"b100 applies flex-basis");
			context.check(firstNode && secondNode && thirdNode,
				"basis wrap fixture laid out");
			context.check(firstNode && secondNode && thirdNode &&
				(std::abs(firstNode->computed.position.y - secondNode->computed.position.y) < 0.1f ||
					std::abs(secondNode->computed.position.y - thirdNode->computed.position.y) < 0.1f),
				"b100 wrap keeps at least two items on one line");
			context.check(firstNode && thirdNode &&
				std::abs(firstNode->computed.position.y - thirdNode->computed.position.y) > 0.1f,
				"b100 wraps overflow item");
			context.check(firstNode && std::abs(firstNode->computed.size.x - 100) < 0.1f,
				"b100 sizes item main axis");
		}

		void flexBasisWrapMinContent(CaseContext& context)
		{
			UiDocument document;
			UiLayoutEngine layout(document);
			const UiNodeHandle root = document.root();
			const UiNodeHandle row = document.createNode(root, { .typeName = "row" });
			const UiNodeHandle first = document.createNode(row, { .typeName = "item" });
			const UiNodeHandle second = document.createNode(row, { .typeName = "item" });
			const UiNodeHandle third = document.createNode(row, { .typeName = "item" });

			if (UiNode* rowNode = document.get(row))
				rowNode->authored.classes = {
					UiAppFlexClass::row, UiAppFlexClass::wrap, UiAppFlexClass::g10};
			for (const UiNodeHandle item : { first, second, third })
			{
				if (UiNode* node = document.get(item))
					node->authored.classes = { UiAppFlexClass::b100 };
				UiLayoutStyle itemStyle;
				itemStyle.flexGrow = 1;
				itemStyle.flexShrink = 0;
				itemStyle.height = UiSizeSpec::fixed(48);
				layout.setStyle(item, itemStyle);
				layout.setIntrinsicMeasurer(item, [](const UiNode&, const UiConstraints&)
					{
						return UiIntrinsicMeasure{ Vec2(120, 0), Vec2(120, 48), -1 };
					});
			}

			layout.layout(UiConstraints::tight(Vec2(350, 200)));
			const UiNode* firstNode = document.get(first);
			const UiNode* secondNode = document.get(second);
			const UiNode* thirdNode = document.get(third);
			context.check(firstNode && secondNode && thirdNode,
				"basis min-content wrap fixture laid out");
			context.check(firstNode && secondNode && thirdNode &&
				(std::abs(firstNode->computed.position.y - secondNode->computed.position.y) < 0.1f ||
					std::abs(secondNode->computed.position.y - thirdNode->computed.position.y) < 0.1f),
				"basis wrap respects min-content on a shared line");
			context.check(firstNode && thirdNode &&
				std::abs(firstNode->computed.position.y - thirdNode->computed.position.y) > 0.1f,
				"basis wrap moves min-content overflow item");
			context.check(firstNode && secondNode && thirdNode &&
				firstNode->computed.position.x + firstNode->computed.size.x <= 350.1f &&
				secondNode->computed.position.x + secondNode->computed.size.x <= 350.1f &&
				thirdNode->computed.position.x + thirdNode->computed.size.x <= 350.1f,
				"basis wrap keeps items inside container");
		}

		void flexGrowInStretchColumn(CaseContext& context)
		{
			UiDocument document;
			UiLayoutEngine layout(document);
			const UiNodeHandle root = document.root();
			const UiNodeHandle column = document.createNode(root, { .typeName = "column" });
			const UiNodeHandle row = document.createNode(column, { .typeName = "row" });
			Vector<UiNodeHandle> items;
			for (Int index = 0; index < 4; ++index)
				items.push_back(document.createNode(row, { .typeName = "item" }));

			if (UiNode* columnNode = document.get(column))
				columnNode->authored.classes = { UiAppFlexClass::col, UiAppFlexClass::g10 };
			if (UiNode* rowNode = document.get(row))
			{
				rowNode->authored.classes = {
					UiAppFlexClass::row, UiAppFlexClass::wrap, UiAppFlexClass::g10};
			}
			for (const UiNodeHandle item : items)
			{
				if (UiNode* node = document.get(item))
					node->authored.classes = {
						UiAppFlexClass::fg1, UiAppFlexClass::mw0, UiAppFlexClass::b100};
				UiLayoutStyle itemStyle;
				itemStyle.flexGrow = 1;
				itemStyle.flexShrink = 0;
				itemStyle.height = UiSizeSpec::fixed(48);
				layout.setStyle(item, itemStyle);
			}

			layout.layout(UiConstraints::tight(Vec2(540, 200)));
			const UiNode* rowNode = document.get(row);
			const UiNode* firstItem = document.get(items[0]);
			context.check(rowNode && firstItem, "stretch column grow fixture laid out");
			context.check(rowNode && std::abs(rowNode->computed.size.x - 540) < 0.5f,
				"column stretch gives row child full container width");
			context.check(firstItem && firstItem->computed.size.x > 120.0f,
				"row child items grow to fill stretched width");
		}

		void flexBasisWrapBalance(CaseContext& context)
		{
			UiDocument document;
			UiLayoutEngine layout(document);
			const UiNodeHandle root = document.root();
			const UiNodeHandle row = document.createNode(root, { .typeName = "row" });
			Vector<UiNodeHandle> items;
			items.reserve(11);
			for (Int index = 0; index < 11; ++index)
				items.push_back(document.createNode(row, { .typeName = "item" }));

			if (UiNode* rowNode = document.get(row))
			{
				rowNode->authored.classes = {
					UiAppFlexClass::row, UiAppFlexClass::wrap, UiAppFlexClass::g10};
			}
			for (const UiNodeHandle item : items)
			{
				if (UiNode* node = document.get(item))
					node->authored.classes = { UiAppFlexClass::b100 };
				UiLayoutStyle itemStyle;
				itemStyle.flexGrow = 1;
				itemStyle.flexShrink = 0;
				itemStyle.height = UiSizeSpec::fixed(48);
				layout.setStyle(item, itemStyle);
			}

			layout.layout(UiConstraints::tight(Vec2(540, 300)));
			const UiNode* first = document.get(items[0]);
			const UiNode* ninth = document.get(items[8]);
			const UiNode* eleventh = document.get(items[10]);
			context.check(first && ninth && eleventh,
				"balanced wrap fixture laid out");
			context.check(ninth && eleventh &&
				std::abs(ninth->computed.position.y - eleventh->computed.position.y) < 0.1f,
				"balanced wrap keeps trailing items on the same line");
			context.check(first && eleventh &&
				first->computed.position.y < eleventh->computed.position.y - 0.1f,
				"balanced wrap still breaks across lines");
			context.check(eleventh && eleventh->computed.size.x < 200.0f,
				"balanced wrap avoids single-item row stretch");
			context.check(ninth && ninth->computed.size.x > 100.5f,
				"balanced wrap grows items to fill leftover line space");
		}

		void flexBasisWrapBalanceDeterministic(CaseContext& context)
		{
			UiDocument document;
			UiLayoutEngine layout(document);
			const UiNodeHandle root = document.root();
			const UiNodeHandle row = document.createNode(root, { .typeName = "row" });
			Vector<UiNodeHandle> items;
			items.reserve(12);
			for (Int index = 0; index < 12; ++index)
				items.push_back(document.createNode(row, { .typeName = "item" }));

			if (UiNode* rowNode = document.get(row))
			{
				rowNode->authored.classes = {
					UiAppFlexClass::row, UiAppFlexClass::wrap, UiAppFlexClass::g10};
			}
			for (const UiNodeHandle item : items)
			{
				if (UiNode* node = document.get(item))
					node->authored.classes = { UiAppFlexClass::b100 };
				UiLayoutStyle itemStyle;
				itemStyle.flexGrow = 1;
				itemStyle.flexShrink = 0;
				itemStyle.height = UiSizeSpec::fixed(48);
				layout.setStyle(item, itemStyle);
			}

			const UiConstraints constraints = UiConstraints::tight(Vec2(540, 300));
			layout.layout(constraints);
			Vector<Vec2> firstPositions;
			Vector<Vec2> firstSizes;
			firstPositions.reserve(items.size());
			firstSizes.reserve(items.size());
			for (const UiNodeHandle item : items)
			{
				const UiNode* node = document.get(item);
				context.check(node, "balanced wrap deterministic fixture laid out");
				if (!node) return;
				firstPositions.push_back(node->computed.position);
				firstSizes.push_back(node->computed.size);
			}

			layout.layout(constraints);
			for (SizeT index = 0; index < items.size(); ++index)
			{
				const UiNode* node = document.get(items[index]);
				context.check(node, "balanced wrap deterministic relayout");
				if (!node) continue;
				context.check(
					std::abs(node->computed.position.x - firstPositions[index].x) < 0.01f &&
					std::abs(node->computed.position.y - firstPositions[index].y) < 0.01f &&
					std::abs(node->computed.size.x - firstSizes[index].x) < 0.01f &&
					std::abs(node->computed.size.y - firstSizes[index].y) < 0.01f,
					"balanced wrap relayout matches prior geometry");
			}

			for (SizeT left = 0; left + 1 < items.size(); ++left)
			{
				const UiNode* leftNode = document.get(items[left]);
				const UiNode* rightNode = document.get(items[left + 1]);
				if (!leftNode || !rightNode) continue;
				const Bool sameRow =
					std::abs(leftNode->computed.position.y - rightNode->computed.position.y) < 0.1f;
				context.check(!sameRow ||
					leftNode->computed.position.x <= rightNode->computed.position.x + 0.01f,
					"balanced wrap keeps equal-size item order on a line");
			}
		}

		void authoredRuntimeIntegration(CaseContext& context)
		{
			UiDocument document;
			UiLayoutEngine layout(document);
			UiEventRouter router(document);
			UiPropertyRegistry properties;
			UiCommandRegistry commands;
			UInt activations = 0;
			context.check(commands.registerCommand("Rows.Activate", {},
				[&activations](const Vector<UiValue>&) { ++activations; return true; }),
				"register authored command");
			context.check(properties.set("show", UiValue::fromBool(true)), "set condition");
			context.check(properties.set("rows", UiValue::fromArray({
				UiValue::fromObject({{"id", UiValue::fromString("a")}, {"name", UiValue::fromString("Alpha")}}),
				UiValue::fromObject({{"id", UiValue::fromString("b")}, {"name", UiValue::fromString("Beta")}})
				})), "set authored rows");

			UiBundleSources sources;
			sources.markup =
				"<ui>@if (show) {@for (row of rows; track row.id) {"
				"<button class=\"row\" (activate)=\"Rows.Activate\"><label [text]=\"row.name\"/></button>"
				"}}</ui>";
			sources.markupName = "authored-runtime.ui.html";
			sources.style = ".row { width: 120px; min-height: 32dp; background-color: #224466; }";
			sources.styleName = "authored-runtime.ui.scss";
			const UiCompiledBundle bundle = UiBundleCompiler::compile(
				1, sources, commands.snapshot());
			context.check(bundle.valid, "compile authored runtime fixture");

			UiTemplateInstantiator instantiator(document, router, properties, commands);
			context.check(instantiator.reconcile(bundle), "instantiate authored runtime fixture");
			UiStyleApplicationAdapter styles(document, layout);
			styles.apply(bundle.styles);

			UiNodeHandle alpha;
			UiNodeHandle firstButton;
			document.traverse([&](const UiNodeHandle handle, UiNode& node)
				{
					if (node.authored.text == "Alpha") alpha = handle;
					if (!firstButton.isValid() && node.authored.typeName == "Button") firstButton = handle;
				});
			context.check(alpha.isValid(), "loop item binding instantiated");
			context.check(firstButton.isValid() && styles.paintTokens(firstButton) &&
				styles.paintTokens(firstButton)->hasBackground, "cascade paint token adapted");
			context.check(firstButton.isValid() &&
				layout.style(firstButton).width == UiSizeSpec::fixed(120), "cascade layout style adapted");

			if (UiNode* alphaNode = document.get(alpha)) alphaNode->state.focused = true;
			context.check(properties.set("rows", UiValue::fromArray({
				UiValue::fromObject({{"id", UiValue::fromString("b")}, {"name", UiValue::fromString("Beta")}}),
				UiValue::fromObject({{"id", UiValue::fromString("a")}, {"name", UiValue::fromString("Alpha")}})
				})), "reorder authored rows");
			context.check(instantiator.reconcile(bundle), "reconcile keyed authored rows");
			context.check(document.isValid(alpha) && document.get(alpha)->state.focused,
				"keyed loop retains local state");
			context.check(document.isValid(alpha) && document.get(alpha)->authored.text == "Alpha",
				"loop text binding survives second reconcile");

			context.check(commands.invoke("Rows.Activate", {}) && activations == 1,
				"typed allowlisted command invokes");
		}
	}

	void tablerIconRasterization(CaseContext& context)
	{
		Vector<UiSvgSubpath> parsed;
		context.check(
			UiSvgPathParser::parsePathData("M5 12l5 5l10 -10", parsed) &&
				!parsed.empty() && parsed.front().points.size() >= 2,
			"parse compact Tabler stroke path");
		context.check(
			UiSvgPathParser::parsePathData("M7 4v16l13 -8l-13 -8", parsed) &&
				!parsed.empty() && parsed.front().points.size() >= 2,
			"parse moveto followed by explicit vertical line");

		UiSvgDocument document;
		document.viewBoxWidth = 24;
		document.viewBoxHeight = 24;
		document.strokeWidth = 2;
		document.subpaths = std::move(parsed);
		UiSvgRasterResult raster;
		context.check(
			UiSvgRasterizer::rasterize(document, 18, raster) && !raster.rgba.empty(),
			"rasterize Tabler stroke icon");

		UInt opaquePixels = 0;
		for (SizeT index = 3; index < raster.rgba.size(); index += 4)
			if (raster.rgba[index] > 0) ++opaquePixels;
		context.check(opaquePixels > 0, "rasterized icon has visible alpha coverage");
	}

		void controlLayoutPaddingDebugOverlay(CaseContext& context)
		{
			UiDocument document;
			UiLayoutEngine layout(document);
			UiEventRouter router(document);

			const UiNodeHandle root = document.root();
			const UiNodeHandle button = document.createNode(root, { .typeName = "Button", .text = "Dark mode" });

			UiLayoutStyle buttonStyle;
			buttonStyle.width = UiSizeSpec::fixed(108 - UiAppFlex::p10 * 2);
			buttonStyle.height = UiSizeSpec::fixed(UiAppTypography::bodyLine);
			layout.setStyle(button, buttonStyle);

			layout.layout(UiConstraints::tight(Vec2(200, 80)));

			const UiNode* buttonNode = document.get(button);
			context.check(buttonNode != nullptr, "control padding fixture has button");
			if (!buttonNode) return;

			context.check(std::abs(buttonNode->computed.size.x - 108.0f) < 0.5f,
				"button border width includes default control padding");
			context.check(std::abs(buttonNode->computed.size.y - UiAppLayout::controlRowHeight) < 0.5f,
				"button border height includes default control padding");
			context.check(std::abs(buttonNode->computed.contentSize.x - (108.0f - UiAppFlex::p10 * 2)) < 0.5f,
				"button content width excludes default control padding");
			context.check(std::abs(buttonNode->computed.contentSize.y - UiAppTypography::bodyLine) < 0.5f,
				"button content height excludes default control padding");

			UiDisplayList displayList;
			const Inspector::UiLayoutDebugOverlayResult painted =
				Inspector::UiLayoutDebugOverlay::paintHovered(
					document, layout, button, displayList);
			context.check(painted.valid && painted.painted, "layout debug paints control button");
			context.check(displayList.commands().size() >= 2,
				"layout debug overlay separates padding and content for controls");
		}

		void explicitSizePaddingContentBox(CaseContext& context)
		{
			UiDocument document;
			UiLayoutEngine layout(document);

			const UiNodeHandle root = document.root();
			const UiNodeHandle icon = document.createNode(root, { .typeName = "Icon" });

			UiLayoutStyle iconStyle;
			iconStyle.width = UiSizeSpec::fixed(28);
			iconStyle.height = UiSizeSpec::fixed(28);
			iconStyle.padding = UiEdges{ 10, 10, 10, 10 };
			layout.setStyle(icon, iconStyle);

			layout.layout(UiConstraints::tight(Vec2(200, 200)));

			const UiNode* iconNode = document.get(icon);
			context.check(iconNode != nullptr, "icon padding fixture exists");
			if (!iconNode) return;

			context.check(std::abs(iconNode->computed.size.x - 48.0f) < 0.5f,
				"explicit width includes horizontal padding");
			context.check(std::abs(iconNode->computed.size.y - 48.0f) < 0.5f,
				"explicit height includes vertical padding");
			context.check(std::abs(iconNode->computed.contentSize.x - 28.0f) < 0.5f,
				"content width preserves explicit size");
			context.check(std::abs(iconNode->computed.contentSize.y - 28.0f) < 0.5f,
				"content height preserves explicit size");
		}

		void flexAutoMinHeaderRow(CaseContext& context)
		{
			UiDocument document;
			UiLayoutEngine layout(document);
			const UiNodeHandle root = document.root();
			const UiNodeHandle header = document.createNode(root, { .typeName = "header" });
			const UiNodeHandle label = document.createNode(header, { .typeName = "label" });
			const UiNodeHandle list = document.createNode(root, { .typeName = "list" });

			UiLayoutStyle columnStyle;
			columnStyle.flexDirection = UiAxis::Column;
			layout.setStyle(root, columnStyle);

			UiLayoutStyle headerStyle;
			headerStyle.flexDirection = UiAxis::Row;
			layout.setStyle(header, headerStyle);

			layout.setIntrinsicMeasurer(label, [](const UiNode&, const UiConstraints&)
				{
					return UiIntrinsicMeasure{ Vec2(80, 24), Vec2(80, 24), 20 };
				});

			UiLayoutStyle listStyle;
			listStyle.flexGrow = 1;
			layout.setStyle(list, listStyle);

			layout.layout(UiConstraints::tight(Vec2(300, 120)));
			const UiNode* headerNode = document.get(header);
			const UiNode* labelNode = document.get(label);
			context.check(headerNode && labelNode, "auto-min header fixture laid out");
			context.check(headerNode && headerNode->computed.size.y >= 24.0f,
				"header row respects min-content height");
			context.check(labelNode && labelNode->computed.size.y >= 23.9f,
				"header label keeps line height");
		}

		void flexAutoMinContainer(CaseContext& context)
		{
			UiDocument document;
			UiLayoutEngine layout(document);
			const UiNodeHandle root = document.root();
			const UiNodeHandle column = document.createNode(root, { .typeName = "column" });
			const UiNodeHandle row = document.createNode(column, { .typeName = "row" });
			const UiNodeHandle label = document.createNode(row, { .typeName = "label" });
			const UiNodeHandle filler = document.createNode(column, { .typeName = "filler" });

			UiLayoutStyle columnStyle;
			columnStyle.flexDirection = UiAxis::Column;
			layout.setStyle(column, columnStyle);
			layout.setStyle(root, columnStyle);

			UiLayoutStyle rowStyle;
			rowStyle.flexDirection = UiAxis::Row;
			layout.setStyle(row, rowStyle);

			layout.setIntrinsicMeasurer(label, [](const UiNode&, const UiConstraints&)
				{
					return UiIntrinsicMeasure{ Vec2(100, 24), Vec2(100, 24), 20 };
				});

			UiLayoutStyle fillerStyle;
			fillerStyle.flexGrow = 1;
			layout.setStyle(filler, fillerStyle);

			layout.layout(UiConstraints::tight(Vec2(200, 80)));
			const UiNode* rowNode = document.get(row);
			context.check(rowNode && rowNode->computed.size.y >= 24.0f,
				"nested row propagates min-content height");
		}

		void flexExplicitMinZeroScroll(CaseContext& context)
		{
			UiDocument document;
			UiLayoutEngine layout(document);
			UiEventRouter router(document);
			UiOverflowController overflow(document, layout, router);
			const UiNodeHandle root = document.root();
			const UiNodeHandle scroller = document.createNode(root, { .typeName = "scroller" });
			const UiNodeHandle first = document.createNode(scroller, { .typeName = "item" });
			const UiNodeHandle second = document.createNode(scroller, { .typeName = "item" });
			const UiNodeHandle third = document.createNode(scroller, { .typeName = "item" });

			UiLayoutStyle rootStyle;
			rootStyle.flexDirection = UiAxis::Column;
			rootStyle.height = UiSizeSpec::fixed(200);
			layout.setStyle(root, rootStyle);

			UiLayoutStyle scrollStyle;
			scrollStyle.flexGrow = 1;
			scrollStyle.flexShrink = 1;
			scrollStyle.minimumHeight = UiSizeSpec::fixed(0);
			scrollStyle.overflowY = UiOverflow::Auto;
			scrollStyle.gap = Vec2(0, 10);
			layout.setStyle(scroller, scrollStyle);

			const Float itemHeight = 80;
			for (const UiNodeHandle item : { first, second, third })
			{
				UiLayoutStyle itemStyle;
				itemStyle.height = UiSizeSpec::fixed(itemHeight);
				itemStyle.flexShrink = 0;
				layout.setStyle(item, itemStyle);
			}

			layout.layout(UiConstraints::tight(Vec2(200, 200)));
			overflow.synchronizeAfterLayout();
			layout.layout(UiConstraints::tight(Vec2(200, 200)));
			overflow.synchronizeAfterLayout();

			const UiNode* scrollNode = document.get(scroller);
			context.check(scrollNode, "explicit min-zero scroll fixture laid out");
			if (!scrollNode) return;

			const Float contentExtent = itemHeight * 3 + 10 * 2;
			context.check(scrollNode->computed.scrollExtent.y > scrollNode->computed.contentSize.y + 1,
				"scroll extent exceeds viewport");
			context.check(std::abs(scrollNode->computed.scrollExtent.y - contentExtent) < 1.0f,
				"scroll extent matches stacked child heights");
			context.check(scrollNode->computed.size.y < contentExtent - 1.0f,
				"explicit min-zero scroll child shrinks below content extent");
		}

		void flexMinW0AllowsShrink(CaseContext& context)
		{
			UiDocument document;
			UiLayoutEngine layout(document);
			const UiNodeHandle root = document.root();
			const UiNodeHandle row = document.createNode(root, { .typeName = "row" });
			const UiNodeHandle item = document.createNode(row, { .typeName = "item" });
			const UiNodeHandle spacer = document.createNode(row, { .typeName = "spacer" });

			UiLayoutStyle rowStyle;
			rowStyle.flexDirection = UiAxis::Row;
			rowStyle.width = UiSizeSpec::fixed(120);
			layout.setStyle(row, rowStyle);
			layout.setStyle(root, rowStyle);

			if (UiNode* itemNode = document.get(item))
				itemNode->authored.classes = { UiAppFlexClass::mw0 };
			UiLayoutStyle itemStyle;
			itemStyle.flexGrow = 1;
			layout.setStyle(item, itemStyle);
			layout.setIntrinsicMeasurer(item, [](const UiNode&, const UiConstraints&)
				{
					return UiIntrinsicMeasure{ Vec2(200, 24), Vec2(200, 24), 20 };
				});

			UiLayoutStyle spacerStyle;
			spacerStyle.width = UiSizeSpec::fixed(40);
			spacerStyle.flexShrink = 0;
			layout.setStyle(spacer, spacerStyle);

			layout.layout(UiConstraints::tight(Vec2(120, 48)));
			const UiNode* itemNode = document.get(item);
			context.check(itemNode && itemNode->computed.size.x < 200.0f,
				"min-w-0 allows shrink below text min width");
			context.check(layout.resolvedStyle(item).minimumWidth == UiSizeSpec::fixed(0),
				"min-w-0 class resolves to zero minimum width");
		}

		void gameLauncherHeaderSurvival(CaseContext& context)
		{
			UiDocument document;
			UiLayoutEngine layout(document);
			const UiNodeHandle root = document.root();
			const UiNodeHandle shell = document.createNode(root, { .typeName = "shell" });
			const UiNodeHandle header = document.createNode(shell, { .typeName = "header" });
			const UiNodeHandle headerLabel = document.createNode(header, { .typeName = "label" });
			const UiNodeHandle title = document.createNode(shell, { .typeName = "title" });
			const UiNodeHandle listPanel = document.createNode(shell, { .typeName = "list-panel" });
			const UiNodeHandle list = document.createNode(listPanel, { .typeName = "list" });

			UiLayoutStyle shellStyle;
			shellStyle.flexDirection = UiAxis::Column;
			shellStyle.gap = Vec2(0, 10);
			layout.setStyle(shell, shellStyle);
			layout.setStyle(root, shellStyle);

			UiLayoutStyle headerStyle;
			headerStyle.flexDirection = UiAxis::Row;
			layout.setStyle(header, headerStyle);

			layout.setIntrinsicMeasurer(headerLabel, [](const UiNode&, const UiConstraints&)
				{
					return UiIntrinsicMeasure{ Vec2(200, 32), Vec2(200, 32), 24 };
				});
			layout.setIntrinsicMeasurer(title, [](const UiNode&, const UiConstraints&)
				{
					return UiIntrinsicMeasure{ Vec2(200, 28), Vec2(200, 28), 22 };
				});

			UiLayoutStyle listPanelStyle;
			listPanelStyle.flexGrow = 1;
			listPanelStyle.minimumHeight = UiSizeSpec::fixed(0);
			listPanelStyle.overflowY = UiOverflow::Auto;
			layout.setStyle(listPanel, listPanelStyle);

			UiLayoutStyle listStyle;
			listStyle.flexDirection = UiAxis::Row;
			listStyle.flexWrap = UiWrap::Wrap;
			layout.setStyle(list, listStyle);

			layout.layout(UiConstraints::tight(Vec2(400, 200)));
			const UiNode* headerNode = document.get(header);
			const UiNode* titleNode = document.get(title);
			context.check(headerNode && headerNode->computed.size.y >= 32.0f,
				"launcher header survives short viewport");
			context.check(titleNode && titleNode->computed.size.y >= 28.0f,
				"launcher title survives short viewport");

			layout.layout(UiConstraints::tight(Vec2(400, 400)));
			context.check(headerNode && headerNode->computed.size.y >= 32.0f,
				"launcher header survives tall viewport");
		}

	UiValidationReport runModernUiValidation(const UiValidationConfig& config)
	{
		UiValidationReport report;
		report.seed = config.seed;
		const Vector<std::pair<String, CaseBody>> cases{
			{ "generational-handles-deferred-mutation", handlesAndDeferredMutation },
			{ "expression-sandbox", expressionSandbox },
			{ "malformed-deterministic-corpus", malformedCorpus },
			{ "cascade-specificity", cascadeSpecificity },
			{ "flex-grid-overlay-constraints", layoutConstraints },
			{ "flex-minimum-content-size", flexMinimumContentSize },
			{ "flex-auto-min-header-row", flexAutoMinHeaderRow },
			{ "flex-auto-min-container", flexAutoMinContainer },
			{ "flex-explicit-min-zero-scroll", flexExplicitMinZeroScroll },
			{ "flex-min-w0-allows-shrink", flexMinW0AllowsShrink },
			{ "game-launcher-header-survival", gameLauncherHeaderSurvival },
			{ "overflow-scroll-container", overflowScrollContainer },
			{ "scroll-text-layout-clip", scrollTextLayoutClip },
			{ "layout-debug-overlay", layoutDebugOverlay },
			{ "control-layout-padding-debug-overlay", controlLayoutPaddingDebugOverlay },
			{ "explicit-size-padding-content-box", explicitSizePaddingContentBox },
			{ "align-classes", alignClasses },
			{ "flex-basis-wrap-classes", flexBasisWrapClasses },
			{ "flex-basis-wrap-min-content", flexBasisWrapMinContent },
			{ "flex-grow-in-stretch-column", flexGrowInStretchColumn },
			{ "flex-basis-wrap-balanced", flexBasisWrapBalance },
			{ "flex-basis-wrap-balanced-deterministic", flexBasisWrapBalanceDeterministic },
			{ "text-utf8-wrap-fit", textUtf8WrapFit },
			{ "scrolling-virtual-list", scrollingAndVirtualList },
			{ "focus-dismiss-modified-activation", focusDismissAndModifiedActivation },
			{ "secondary-mouse-button-no-activate", secondaryMouseButtonDoesNotActivate },
			{ "event-phases-focus-controller", eventPhasesFocusController },
			{ "text-editing-undo-composition", textEditing },
			{ "glyph-render-mode-distance-metadata", glyphRenderModeAndMetadata },
			{ "atlas-allocation-eviction", atlasAllocationEviction },
			{ "glyph-resource-fallback-transition", glyphResourceTransition },
			{ "display-list-validation-batching", displayListValidationBatching },
			{ "fallback-text-painting", fallbackTextPainting },
			{ "measured-painted-text-agreement", measuredPaintedTextAgreement },
			{ "hot-reload-retains-valid-bundle", hotReloadRetainsPrevious },
			{ "authored-runtime-integration", authoredRuntimeIntegration },
			{ "tabler-icon-rasterization", tablerIconRasterization },
		};
		for (SizeT index = 0; index < cases.size(); ++index)
		{
			if (!config.includeExpensiveCases &&
				(cases[index].first == "malformed-deterministic-corpus" ||
				 cases[index].first == "atlas-allocation-eviction"))
			{
				UiValidationCaseResult skipped;
				skipped.name = cases[index].first;
				skipped.seed = config.seed + index;
				skipped.status = UiValidationStatus::Skipped;
				skipped.notes.push_back("Excluded by includeExpensiveCases.");
				report.cases.push_back(std::move(skipped));
				++report.skipped;
				continue;
			}
			UiValidationCaseResult result =
				runCase(cases[index].first, config.seed + index, config, cases[index].second);
			switch (result.status)
			{
			case UiValidationStatus::Passed: ++report.passed; break;
			case UiValidationStatus::Failed: ++report.failed; break;
			case UiValidationStatus::Skipped: ++report.skipped; break;
			case UiValidationStatus::BudgetExceeded: ++report.budgetExceeded; break;
			}
			report.cases.push_back(std::move(result));
		}
		return report;
	}
}
